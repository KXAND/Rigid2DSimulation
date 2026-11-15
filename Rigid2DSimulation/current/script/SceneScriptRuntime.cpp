#include "SceneScriptRuntime.h"

#include <climits>
#include <cmath>
#include <iostream>

#include "Body.h"
#include "BodyConfig.h"
#include "Connection.h"
#include "ConnectionConfig.h"
#include "Creator.h"
#include "PhysicsWorld.h"
#include "SceneBuilders.h"
#include "SceneCanvas.h"
#include "SceneEditor.h"
#include "SceneParameter.h"
#include "SceneRenderer.h"
#include "ScriptError.h"
#include "ShapeBuiltins.h"
#include "ConfigUtils.h"

namespace
{
    void ApplyGridConfig(Value const& config, PhysicsWorld& world)
    {
        auto gridSizeValue = FindConfigValue(config, L"gridSize");
        auto widthValue = FindConfigValue(config, L"sceneWidth");
        auto heightValue = FindConfigValue(config, L"sceneHeight");
        if (!gridSizeValue && !widthValue && !heightValue)
        {
            return;
        }

        // 先收集整组候选值，避免拒绝配置后留下新旧值混用的网格。
        double gridSize = world.gridSize;
        int width = world.rectScene.width;
        int height = world.rectScene.height;
        if (gridSizeValue)
        {
            if (gridSizeValue->type != L"num" || !std::isfinite(gridSizeValue->number) || gridSizeValue->number <= 0)
            {
                std::wcerr << L"SetConfig: gridSize must be a finite positive number; keeping previous grid configuration.\n";
                return;
            }
            gridSize = gridSizeValue->number;
        }
        // 场景宽高存为 int，必须在转换前拒绝无效值；合法小数仍按旧规则截断。
        if (widthValue)
        {
            if (widthValue->type != L"num" || !std::isfinite(widthValue->number) ||
                widthValue->number < 1 || widthValue->number > INT_MAX)
            {
                std::wcerr << L"SetConfig: sceneWidth must be a finite number in [1, INT_MAX]; keeping previous grid configuration.\n";
                return;
            }
            width = static_cast<int>(widthValue->number);
        }
        if (heightValue)
        {
            if (heightValue->type != L"num" || !std::isfinite(heightValue->number) ||
                heightValue->number < 1 || heightValue->number > INT_MAX)
            {
                std::wcerr << L"SetConfig: sceneHeight must be a finite number in [1, INT_MAX]; keeping previous grid configuration.\n";
                return;
            }
            height = static_cast<int>(heightValue->number);
        }
        if (!PhysicsWorld::IsGridConfigValid(gridSize, width, height))
        {
            std::wcerr << L"SetConfig: scene grid dimensions exceed supported limits; keeping previous grid configuration.\n";
            return;
        }
        world.gridSize = gridSize;
        world.rectScene.width = width;
        world.rectScene.height = height;
    }

    class MotionPresetExecutionScope
    {
    public:
        explicit MotionPresetExecutionScope(bool& isActive) : isActive(isActive), wasActive(isActive)
        {
            isActive = true;
        }

        ~MotionPresetExecutionScope()
        {
            isActive = wasActive;
        }

        MotionPresetExecutionScope(MotionPresetExecutionScope const&) = delete;
        MotionPresetExecutionScope& operator=(MotionPresetExecutionScope const&) = delete;

    private:
        bool& isActive;
        bool wasActive;
    };
}

std::wstring const& SceneScriptRuntime::GetError() const
{
    return lastError;
}

std::wstring SceneScriptRuntime::ConsumeError()
{
    if (!hasPendingError)
    {
        return {};
    }
    hasPendingError = false;
    return lastError;
}

void SceneScriptRuntime::ClearError()
{
    lastError.clear();
    hasPendingError = false;
}

void SceneScriptRuntime::RequireSceneMutationAllowed(wchar_t const* functionName) const
{
    if (isExecutingMotionPreset)
    {
        throw ScriptError(std::wstring(functionName) + L" is not allowed in a motion preset.");
    }
}

void SceneScriptRuntime::ResetScope(SceneScriptBindings bindings)
{
    ClearError();
    currentScope.clear();
    InitializeDefaultFunctions(bindings);
    parameters.clear();
    frameDeltaTime = 0;
    currentScope[L"deltaTime"] = std::make_shared<Value>(0);
}

void SceneScriptRuntime::ResetSceneVariables(SceneCanvas const& canvas)
{
    debugString.clear();
    currentScope[L"backgroundLeft"] = std::make_shared<Value>(canvas.topLeft.x);
    currentScope[L"backgroundTop"] = std::make_shared<Value>(canvas.topLeft.y);
    currentScope[L"backgroundWidth"] = std::make_shared<Value>(canvas.width);
    currentScope[L"backgroundHeight"] = std::make_shared<Value>(canvas.height);
}

void SceneScriptRuntime::SetFrameDeltaTime(double deltaTime)
{
    frameDeltaTime = deltaTime;
}

void SceneScriptRuntime::SetSimulationTime(double time)
{
    currentScope[L"time"] = std::make_shared<Value>(time);
}

void SceneScriptRuntime::ReportError(std::wstring const& context, ScriptError const& error)
{
    lastError = context + L": " + error.GetMessage();
    hasPendingError = true;
}

bool SceneScriptRuntime::TryExecuteScript(std::wstring const& text, std::wstring const& context)
{
    ClearError();
    try
    {
        Execute(currentScope, Compile(text));
        return true;
    }
    catch (ScriptError const& error)
    {
        ReportError(context, error);
        return false;
    }
}

bool SceneScriptRuntime::ExecuteSceneScript()
{
    return TryExecuteScript(scriptText, L"Scene script");
}

bool SceneScriptRuntime::ApplySceneScript()
{
    scriptText = temporaryScriptText;
    return ExecuteSceneScript();
}

void SceneScriptRuntime::Update()
{
    if (currentScope.find(L"current.debugString") != currentScope.end())
    {
        debugString = currentScope[L"current.debugString"]->stringValue;
    }
    // 先清理参数列表，再将面板参数同步到脚本作用域。
    parameters.erase(
        remove_if(parameters.begin(), parameters.end(),
                  [](std::shared_ptr<SceneParameter> parameter)
                  {
                      return parameter->isMarkedForDeletion;
                  }),
        parameters.end());

    for (auto& parameter : parameters)
    {
        currentScope[parameter->name] = std::make_shared<Value>(parameter->value);
    }

    // 系统帧时长优先于同名面板参数；脚本与物理推进使用同一个时长。
    currentScope[L"deltaTime"] = std::make_shared<Value>(frameDeltaTime);

    // 本帧脚本只执行一次，读取刚同步的参数和时长。
    if (lastError.empty() && currentScope.find(L"Update") != currentScope.end())
    {
        try
        {
            auto updateFunction = *currentScope[L"Update"];
            if (updateFunction.type != L"fun")
            {
                throw ScriptError(L"Update must be a script function.");
            }
            RequireArgumentCount(L"Update", updateFunction.arguments.size(), 0);
            // 保留自动 Update 在场景作用域内执行的既有规则。
            Execute(currentScope, updateFunction.bodyStatements);
        }
        catch (ScriptError const& error)
        {
            ReportError(L"Update", error);
        }
    }
}

void SceneScriptRuntime::SetConfig(Value const& config, SceneScriptBindings bindings)
{
    TryReadConfigNumber(config, L"volume", bindings.volume);
    TryReadConfigNumber(config, L"energyMultiplier", bindings.sceneRenderer.energyMultiplier);
    TryReadConfigNumber(config, L"chargeMultiplier", bindings.sceneRenderer.chargeMultiplier);
    TryReadConfigNumber(config, L"parallelEpsilon", bindings.physicsWorld.parallelEpsilon);
    if (auto value = FindConfigValue(config, L"maxRealDeltaTime"))
    {
        if (value->type == L"num" && std::isfinite(value->number) && value->number > 0)
        {
            bindings.maxRealDeltaTime = value->number;
        }
        else
        {
            std::wcerr << L"SetConfig: maxRealDeltaTime must be a finite positive number; keeping previous value.\n";
        }
    }
    TryReadConfigNumber(config, L"hasElectrostatic", bindings.physicsWorld.hasElectrostatic);
    TryReadConfigNumber(config, L"coulombConstant", bindings.physicsWorld.coulombConstant);
    if (auto value = FindConfigValue(config, L"stepCount"))
    {
        // 必须先校验再转 int；下面的最小值限制无法补救越界转换。
        if (value->type == L"num" && std::isfinite(value->number) &&
            value->number >= INT_MIN && value->number <= INT_MAX)
        {
            bindings.physicsWorld.stepNum = static_cast<int>(value->number);
        }
        else
        {
            std::wcerr << L"SetConfig: stepCount must be a finite number in [INT_MIN, INT_MAX]; keeping previous value.\n";
        }
    }
    bindings.physicsWorld.stepNum = max(1, bindings.physicsWorld.stepNum);
    ApplyGridConfig(config, bindings.physicsWorld);
    TryReadConfigNumber(config, L"useEqualPositionCorrection", bindings.physicsWorld.useEqualPositionCorrection);
    TryReadConfigNumber(config, L"time", bindings.physicsWorld.simulationTime);
    TryReadConfigNumber(config, L"isTrackShown", bindings.isTrackShown);
    if (auto value = FindConfigValue(config, L"sceneLeft"))
    {
        bindings.physicsWorld.rectScene.topLeftPosition.x = value->number;
    }
    if (auto value = FindConfigValue(config, L"sceneTop"))
    {
        bindings.physicsWorld.rectScene.topLeftPosition.y = value->number;
    }
    if (auto value = FindConfigValue(config, L"gravity"))
    {
        bindings.physicsWorld.gravity = ToVector2(*value);
    }

    bindings.physicsWorld.isSceneChanged = true;
}

void SceneScriptRuntime::InitializeDefaultFunctions(SceneScriptBindings bindings)
{
    auto setCreatorFunction = [this, bindings](vector<std::shared_ptr<Value>> const& arguments)
    {
        RequireSceneMutationAllowed(L"SetCreator");
        bindings.editor.RequestCreatorConfig(*arguments[0]);
        return std::make_shared<Value>();
    };
    currentScope[L"SetCreator"] = std::make_shared<Value>(setCreatorFunction, 1);

    auto createBodyFunction = [this, bindings](vector<std::shared_ptr<Value>> const& arguments)
    {
        RequireSceneMutationAllowed(L"CreateBody");
        CreateBody(bindings.physicsWorld, *arguments[0], *arguments[1]);
        return std::make_shared<Value>();
    };
    currentScope[L"CreateBody"] = std::make_shared<Value>(createBodyFunction, 2);

    auto boxFunction = [](vector<std::shared_ptr<Value>> const& arguments)
    {
        return Box(*arguments[0], *arguments[1]);
    };
    currentScope[L"Box"] = std::make_shared<Value>(boxFunction, 2);

    auto ballFunction = [](vector<std::shared_ptr<Value>> const& arguments)
    {
        return Ball(*arguments[0]);
    };
    currentScope[L"Ball"] = std::make_shared<Value>(ballFunction, 1);

    auto parallelogramFunction = [](vector<std::shared_ptr<Value>> const& arguments)
    {
        return Parallelogram(*arguments[0], *arguments[1], *arguments[2]);
    };
    currentScope[L"Parallelogram"] = std::make_shared<Value>(parallelogramFunction, 3);

    auto trapezoidFunction = [](vector<std::shared_ptr<Value>> const& arguments)
    {
        return Trapezoid(*arguments[0], *arguments[1], *arguments[2]);
    };
    currentScope[L"Trapezoid"] = std::make_shared<Value>(trapezoidFunction, 3);

    auto regularPolygonFunction = [](vector<std::shared_ptr<Value>> const& arguments)
    {
        return RegularPolygon(*arguments[0], *arguments[1]);
    };
    currentScope[L"RegularPolygon"] = std::make_shared<Value>(regularPolygonFunction, 2);

    auto crossFunction = [](vector<std::shared_ptr<Value>> const& arguments)
    {
        return Cross(*arguments[0], *arguments[1]);
    };
    currentScope[L"Cross"] = std::make_shared<Value>(crossFunction, 2);

    auto getBodyCountFunction = [bindings](vector<std::shared_ptr<Value>> const& arguments)
    {
        return std::make_shared<Value>(bindings.physicsWorld.GetBodies().size());
    };
    currentScope[L"GetBodyCount"] = std::make_shared<Value>(getBodyCountFunction, 0);

    auto createConnectionFunction = [this, bindings](vector<std::shared_ptr<Value>> const& arguments)
    {
        RequireSceneMutationAllowed(L"CreateConnection");
        CreateConnection(bindings.physicsWorld, *arguments[0]);
        return std::make_shared<Value>();
    };
    currentScope[L"CreateConnection"] = std::make_shared<Value>(createConnectionFunction, 1);

    auto setConfigFunction = [this, bindings](vector<std::shared_ptr<Value>> const& arguments)
    {
        RequireSceneMutationAllowed(L"SetConfig");
        SetConfig(*arguments[0], bindings);
        return std::make_shared<Value>();
    };
    currentScope[L"SetConfig"] = std::make_shared<Value>(setConfigFunction, 1);

    auto boundaryFunction = [this, bindings](vector<std::shared_ptr<Value>> const& arguments)
    {
        RequireSceneMutationAllowed(L"Boundary");
        Boundary(bindings.physicsWorld, bindings.sceneCanvas.GetViewport(), *arguments[0], *arguments[1]);
        return std::make_shared<Value>();
    };
    currentScope[L"Boundary"] = std::make_shared<Value>(boundaryFunction, 2);

    auto framedBoxFunction = [](vector<std::shared_ptr<Value>> const& arguments)
    {
        return FramedBox(*arguments[0], *arguments[1], *arguments[2]);
    };
    currentScope[L"FramedBox"] = std::make_shared<Value>(framedBoxFunction, 3);

    auto gearFunction = [this, bindings](vector<std::shared_ptr<Value>> const& arguments)
    {
        RequireSceneMutationAllowed(L"Gear");
        Gear(bindings.physicsWorld, *arguments[0], *arguments[1], *arguments[2], *arguments[3]);
        return std::make_shared<Value>();
    };
    currentScope[L"Gear"] = std::make_shared<Value>(gearFunction, 4);

    auto strandFunction = [this, bindings](vector<std::shared_ptr<Value>> const& arguments)
    {
        RequireSceneMutationAllowed(L"Strand");
        Strand(bindings.physicsWorld, *arguments[0], *arguments[1], *arguments[2], *arguments[3], *arguments[4], *arguments[5]);
        return std::make_shared<Value>();
    };
    currentScope[L"Strand"] = std::make_shared<Value>(strandFunction, 6);

    auto necklaceFunction = [this, bindings](vector<std::shared_ptr<Value>> const& arguments)
    {
        RequireSceneMutationAllowed(L"Necklace");
        Necklace(bindings.physicsWorld, *arguments[0], *arguments[1], *arguments[2], *arguments[3], *arguments[4], *arguments[5]);
        return std::make_shared<Value>();
    };
    currentScope[L"Necklace"] = std::make_shared<Value>(necklaceFunction, 6);
}

bool SceneScriptRuntime::RefreshBodyScript(Body& body)
{
    currentScope[L"body"] = std::make_shared<Value>();
    if (!TryExecuteScript(body.scriptData.scriptText, L"Body script"))
    {
        return false;
    }
    if (currentScope.find(L"body") == currentScope.end())
    {
        return true;
    }
    auto const& bodyConfig = *currentScope[L"body"];
    auto properties = ParseBodyProperties(bodyConfig, body.GetProperties());

    // 这些单独放是因为它们无法只能通过 scriptText 获得，无法保存。
    TryReadConfigNumber(bodyConfig, L"hasPositionPreset", body.scriptData.hasPositionPreset);
    TryReadConfigNumber(bodyConfig, L"hasAnglePreset", body.scriptData.hasAnglePreset);
    TryReadConfigNumber(bodyConfig, L"hasAngularVelocityPreset", body.scriptData.hasAngularVelocityPreset);
    if (auto value = FindConfigValue(bodyConfig, L"positionProgram"))
    {
        body.scriptData.positionProgram = std::make_shared<Program>(value->bodyStatements);
    }
    if (auto value = FindConfigValue(bodyConfig, L"angleProgram"))
    {
        body.scriptData.angleProgram = std::make_shared<Program>(value->bodyStatements);
    }
    if (auto value = FindConfigValue(bodyConfig, L"angularVelocityProgram"))
    {
        body.scriptData.angularVelocityProgram = std::make_shared<Program>(value->bodyStatements);
    }
    body.ApplyProperties(properties);
    return true;
}

bool SceneScriptRuntime::RefreshConnectionScript(Connection& connection)
{
    currentScope[L"connection"] = std::make_shared<Value>();
    if (!TryExecuteScript(connection.scriptText, L"Connection script"))
    {
        return false;
    }
    if (currentScope.find(L"connection") == currentScope.end())
    {
        return true;
    }
    ApplyConnectionConfig(connection, *currentScope[L"connection"]);

    connection.UpdatePosition();
    return true;
}

void SceneScriptRuntime::ApplyMotionPresets(Body& body, bool isDynamic,
                                          double const& simulationTime, double stepDeltaTime)
{
    if (!lastError.empty())
    {
        return;
    }

    MotionPresetExecutionScope executionScope(isExecutingMotionPreset);
    wchar_t const* presetName = L"Motion preset";
    try
    {
        if (isDynamic)
        {
            if (body.scriptData.hasAngularVelocityPreset)
            {
                presetName = L"Angular velocity preset";
                FollowPresetAngularVelocity(body, simulationTime);
            }
        }
        else
        {
            if (body.scriptData.hasPositionPreset)
            {
                presetName = L"Position preset";
                FollowPresetPosition(body, simulationTime, stepDeltaTime);
            }
            if (body.scriptData.hasAnglePreset)
            {
                presetName = L"Angle preset";
                FollowPresetAngle(body, simulationTime, stepDeltaTime);
            }
        }
    }
    catch (ScriptError const& error)
    {
        // 不让异常中断物理遍历；App 在本帧推进完成后暂停并展示错误。
        ReportError(presetName, error);
    }
}

void SceneScriptRuntime::FollowPresetPosition(Body& body, double const& simulationTime, double stepDeltaTime)
{
    if (!body.scriptData.positionProgram)
    {
        return;
    }
    Scope scope;
    bool hasReturned = false;
    scope[L"time"] = std::make_shared<Value>(simulationTime);
    auto positionOld = ToVector2(Execute(hasReturned, currentScope, scope, *body.scriptData.positionProgram)->elements);
    scope.clear();
    scope[L"time"] = std::make_shared<Value>(simulationTime + stepDeltaTime);
    hasReturned = false;
    auto positionNew = ToVector2(Execute(hasReturned, currentScope, scope, *body.scriptData.positionProgram)->elements);
    auto velocityNew = (positionNew - positionOld) / stepDeltaTime;

    // 其实我觉得应当选用时间较小的值。
    body.position = positionNew;
    body.velocity = velocityNew;
}

void SceneScriptRuntime::FollowPresetAngle(Body& body, double const& simulationTime, double stepDeltaTime)
{
    if (!body.scriptData.angleProgram)
    {
        return;
    }

    Scope scope;
    bool hasReturned = false;
    scope[L"time"] = std::make_shared<Value>(simulationTime);
    auto radianOld = Execute(hasReturned, currentScope, scope, *body.scriptData.angleProgram)->number;
    scope.clear();
    scope[L"time"] = std::make_shared<Value>(simulationTime + stepDeltaTime);
    hasReturned = false;
    auto radianNew = Execute(hasReturned, currentScope, scope, *body.scriptData.angleProgram)->number;
    auto velocityAngularNew = (radianNew - radianOld) / stepDeltaTime;

    body.radian = radianNew;
    body.velocityAngular = velocityAngularNew;
}

void SceneScriptRuntime::FollowPresetAngularVelocity(Body& body, double const& simulationTime)
{
    if (!body.scriptData.angularVelocityProgram)
    {
        return;
    }

    Scope scope;
    bool hasReturned = false;
    scope[L"time"] = std::make_shared<Value>(simulationTime);
    auto velocityAngularNew = Execute(hasReturned, currentScope, scope, *body.scriptData.angularVelocityProgram)->number;

    body.velocityAngular = velocityAngularNew;
}
