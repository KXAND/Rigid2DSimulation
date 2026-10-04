#include "SceneScriptRuntime.h"
#include "Body.h"
#include "Connection.h"
#include "Creator.h"
#include "PhysicsWorld.h"
#include "SceneBuilders.h"
#include "SceneCanvas.h"
#include "SceneEditor.h"
#include "SceneParameter.h"
#include "SceneRenderer.h"
#include "ShapeBuiltins.h"
#include "current/common/utils.h"

void SceneScriptRuntime::ResetScope(SceneScriptBindings bindings)
{
    currentScope.clear();
    InitializeDefaultFunctions(bindings);
    parameters.clear();
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
    currentScope[L"current.frameRate.GetDeltaTime()"] = std::make_shared<Value>(deltaTime);
}

void SceneScriptRuntime::SetSimulationTime(double time)
{
    currentScope[L"time"] = std::make_shared<Value>(time);
}

void SceneScriptRuntime::ExecuteSceneScript()
{
    Execute(currentScope, Compile(scriptText));
}

void SceneScriptRuntime::ApplySceneScript()
{
    scriptText = temporaryScriptText;
    ExecuteSceneScript();
}

void SceneScriptRuntime::Update()
{
    // 保留原帧流程：两次 Update 之间同步参数，不能合并脚本调用。
    if (currentScope.find(L"current.debugString") != currentScope.end())
    {
        debugString = currentScope[L"current.debugString"]->stringValue;
    }
    if (currentScope.find(L"Update") != currentScope.end())
    {
        auto file = currentScope[L"Update"];
        Execute(currentScope, file->bodyStatements);
    }

    // 删除标记为删除的键
    parameters.erase(
        remove_if(parameters.begin(), parameters.end(),
                  [](std::shared_ptr<SceneParameter> parameter)
                  {
                      return parameter->isMarkedForDeletion;
                  }),
        parameters.end());

    //更新对应键的值
    for (auto& parameter : parameters)
    {
        currentScope[parameter->name] = std::make_shared<Value>(parameter->value);
    }

    if (currentScope.find(L"Update") != currentScope.end())
    {
        auto updateFunction = *currentScope[L"Update"];
        Execute(currentScope, updateFunction.bodyStatements);
    }
}

void SceneScriptRuntime::SetConfig(Value const& config, SceneScriptBindings bindings)
{
    TryReadConfigNumber(config, L"volume", bindings.volume);
    TryReadConfigNumber(config, L"energyMultiplier", bindings.sceneRenderer.energyMultiplier);
    TryReadConfigNumber(config, L"chargeMultiplier", bindings.sceneRenderer.chargeMultiplier);
    TryReadConfigNumber(config, L"parallelEpsilon", bindings.physicsWorld.parallelEpsilon);
    TryReadConfigNumber(config, L"maxRealDeltaTime", bindings.maxRealDeltaTime);
    TryReadConfigNumber(config, L"hasElectrostatic", bindings.physicsWorld.hasElectrostatic);
    TryReadConfigNumber(config, L"coulombConstant", bindings.physicsWorld.coulombConstant);
    TryReadConfigNumber(config, L"stepCount", bindings.physicsWorld.stepNum);
    bindings.physicsWorld.stepNum = max(1, bindings.physicsWorld.stepNum);
    TryReadConfigNumber(config, L"gridSize", bindings.physicsWorld.gridSize);
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
    if (auto value = FindConfigValue(config, L"sceneWidth"))
    {
        bindings.physicsWorld.rectScene.width = value->number;
    }
    if (auto value = FindConfigValue(config, L"sceneHeight"))
    {
        bindings.physicsWorld.rectScene.height = value->number;
    }
    if (auto value = FindConfigValue(config, L"gravity"))
    {
        bindings.physicsWorld.gravity = ToVector2(*value);
    }

    bindings.physicsWorld.isSceneChanged = true;
}

void SceneScriptRuntime::InitializeDefaultFunctions(SceneScriptBindings bindings)
{
    auto setCreatorFunction = [bindings](vector<std::shared_ptr<Value>> const& arguments)
    {
        if (arguments.size() >= 1)
        {
            bindings.editor.creator = std::make_shared<Creator>(*arguments[0]);
        }
        return std::make_shared<Value>();
    };
    currentScope[L"SetCreator"] = std::make_shared<Value>(setCreatorFunction);

    auto createBodyFunction = [bindings](vector<std::shared_ptr<Value>> const& arguments)
    {
        if (arguments.size() >= 2)
        {
            CreateBody(bindings.physicsWorld, *arguments[0], *arguments[1]);
        }
        return std::make_shared<Value>();
    };
    currentScope[L"CreateBody"] = std::make_shared<Value>(createBodyFunction);

    auto boxFunction = [](vector<std::shared_ptr<Value>> const& arguments)
    {
        if (arguments.size() >= 2)
        {
            return Box(*arguments[0], *arguments[1]);
        }
        return std::make_shared<Value>();
    };
    currentScope[L"Box"] = std::make_shared<Value>(boxFunction);

    auto ballFunction = [](vector<std::shared_ptr<Value>> const& arguments)
    {
        if (arguments.size() >= 1)
        {
            return Ball(*arguments[0]);
        }
        return std::make_shared<Value>();
    };
    currentScope[L"Ball"] = std::make_shared<Value>(ballFunction);

    auto parallelogramFunction = [](vector<std::shared_ptr<Value>> const& arguments)
    {
        if (arguments.size() >= 2)
        {
            return Parallelogram(*arguments[0], *arguments[1], *arguments[2]);
        }
        return std::make_shared<Value>();
    };
    currentScope[L"Parallelogram"] = std::make_shared<Value>(parallelogramFunction);

    auto trapezoidFunction = [](vector<std::shared_ptr<Value>> const& arguments)
    {
        if (arguments.size() >= 3)
        {
            return Trapezoid(*arguments[0], *arguments[1], *arguments[2]);
        }
        return std::make_shared<Value>();
    };
    currentScope[L"Trapezoid"] = std::make_shared<Value>(trapezoidFunction);

    auto regularPolygonFunction = [](vector<std::shared_ptr<Value>> const& arguments)
    {
        if (arguments.size() >= 2)
        {
            return RegularPolygon(*arguments[0], *arguments[1]);
        }
        return std::make_shared<Value>();
    };
    currentScope[L"RegularPolygon"] = std::make_shared<Value>(regularPolygonFunction);

    auto crossFunction = [](vector<std::shared_ptr<Value>> const& arguments)
    {
        if (arguments.size() >= 2)
        {
            return Cross(*arguments[0], *arguments[1]);
        }
        return std::make_shared<Value>();
    };
    currentScope[L"Cross"] = std::make_shared<Value>(crossFunction);

    auto getBodyCountFunction = [bindings](vector<std::shared_ptr<Value>> const& arguments)
    {
        return std::make_shared<Value>(bindings.physicsWorld.bodies.size());
    };
    currentScope[L"GetBodyCount"] = std::make_shared<Value>(getBodyCountFunction);

    auto createConnectionFunction = [bindings](vector<std::shared_ptr<Value>> const& arguments)
    {
        if (arguments.size() >= 1)
        {
            CreateConnection(bindings.physicsWorld, *arguments[0]);
        }
        return std::make_shared<Value>();
    };
    currentScope[L"CreateConnection"] = std::make_shared<Value>(createConnectionFunction);

    auto setConfigFunction = [this, bindings](vector<std::shared_ptr<Value>> const& arguments)
    {
        if (arguments.size() >= 1)
        {
            SetConfig(*arguments[0], bindings);
        }
        return std::make_shared<Value>();
    };
    currentScope[L"SetConfig"] = std::make_shared<Value>(setConfigFunction);

    auto boundaryFunction = [bindings](vector<std::shared_ptr<Value>> const& arguments)
    {
        if (arguments.size() >= 2)
        {
            Boundary(bindings.physicsWorld, bindings.sceneCanvas.GetViewport(), *arguments[0], *arguments[1]);
        }
        return std::make_shared<Value>();
    };
    currentScope[L"Boundary"] = std::make_shared<Value>(boundaryFunction);

    auto framedBoxFunction = [](vector<std::shared_ptr<Value>> const& arguments)
    {
        if (arguments.size() >= 3)
        {
            return FramedBox(*arguments[0], *arguments[1], *arguments[2]);
        }
        return std::make_shared<Value>();
    };
    currentScope[L"FramedBox"] = std::make_shared<Value>(framedBoxFunction);

    auto gearFunction = [bindings](vector<std::shared_ptr<Value>> const& arguments)
    {
        if (arguments.size() >= 4)
        {
            Gear(bindings.physicsWorld, *arguments[0], *arguments[1], *arguments[2], *arguments[3]);
        }
        return std::make_shared<Value>();
    };
    currentScope[L"Gear"] = std::make_shared<Value>(gearFunction);

    auto strandFunction = [bindings](vector<std::shared_ptr<Value>> const& arguments)
    {
        if (arguments.size() >= 6)
        {
            Strand(bindings.physicsWorld, *arguments[0], *arguments[1], *arguments[2], *arguments[3], *arguments[4], *arguments[5]);
        }
        return std::make_shared<Value>();
    };
    currentScope[L"Strand"] = std::make_shared<Value>(strandFunction);

    auto necklaceFunction = [bindings](vector<std::shared_ptr<Value>> const& arguments)
    {
        if (arguments.size() >= 6)
        {
            Necklace(bindings.physicsWorld, *arguments[0], *arguments[1], *arguments[2], *arguments[3], *arguments[4], *arguments[5]);
        }
        return std::make_shared<Value>();
    };
    currentScope[L"Necklace"] = std::make_shared<Value>(necklaceFunction);
}

void SceneScriptRuntime::RefreshBodyScript(Body& body)
{
    currentScope[L"body"] = std::make_shared<Value>();
    Execute(currentScope, Compile(body.scriptText));
    if (currentScope.find(L"body") == currentScope.end())
    {
        return;
    }
    auto const& bodyConfig = *currentScope[L"body"];
    body.ReadConfig(bodyConfig);

    // 这些单独放是因为它们无法只能通过 scriptText 获得，无法保存。
    TryReadConfigNumber(bodyConfig, L"hasPositionPreset", body.hasPositionPreset);
    TryReadConfigNumber(bodyConfig, L"hasAnglePreset", body.hasAnglePreset);
    TryReadConfigNumber(bodyConfig, L"hasAngularVelocityPreset", body.hasAngularVelocityPreset);
    if (auto value = FindConfigValue(bodyConfig, L"positionProgram"))
    {
        body.positionProgram = std::make_shared<Program>(value->bodyStatements);
    }
    if (auto value = FindConfigValue(bodyConfig, L"angleProgram"))
    {
        body.angleProgram = std::make_shared<Program>(value->bodyStatements);
    }
    if (auto value = FindConfigValue(bodyConfig, L"angularVelocityProgram"))
    {
        body.angularVelocityProgram = std::make_shared<Program>(value->bodyStatements);
    }
    body.Init();
}

void SceneScriptRuntime::RefreshConnectionScript(Connection& connection)
{
    currentScope[L"connection"] = std::make_shared<Value>();
    Execute(currentScope, Compile(connection.scriptText));
    if (currentScope.find(L"connection") == currentScope.end())
    {
        return;
    }
    connection.ReadConfig(*currentScope[L"connection"]);

    connection.UpdatePosition();
}

void SceneScriptRuntime::ApplyMotionPresets(Body& body, bool isDynamic,
                                          double const& simulationTime, double stepDeltaTime)
{
    if (isDynamic)
    {
        if (body.hasAngularVelocityPreset)
        {
            FollowPresetAngularVelocity(body, simulationTime);
        }
    }
    else
    {
        if (body.hasPositionPreset)
        {
            FollowPresetPosition(body, simulationTime, stepDeltaTime);
        }
        if (body.hasAnglePreset)
        {
            FollowPresetAngle(body, simulationTime, stepDeltaTime);
        }
    }
}

void SceneScriptRuntime::FollowPresetPosition(Body& body, double const& simulationTime, double stepDeltaTime)
{
    if (!body.positionProgram)
    {
        return;
    }
    Scope scope;
    bool hasReturned = false;
    scope[L"time"] = std::make_shared<Value>(simulationTime);
    auto positionOld = ToVector2(Execute(hasReturned, currentScope, scope, *body.positionProgram)->elements);
    scope.clear();
    scope[L"time"] = std::make_shared<Value>(simulationTime + stepDeltaTime);
    hasReturned = false;
    auto positionNew = ToVector2(Execute(hasReturned, currentScope, scope, *body.positionProgram)->elements);
    auto velocityNew = (positionNew - positionOld) / stepDeltaTime;

    // 其实我觉得应当选用时间较小的值。
    body.position = positionNew;
    body.velocity = velocityNew;
}

void SceneScriptRuntime::FollowPresetAngle(Body& body, double const& simulationTime, double stepDeltaTime)
{
    if (!body.angleProgram)
    {
        return;
    }

    Scope scope;
    bool hasReturned = false;
    scope[L"time"] = std::make_shared<Value>(simulationTime);
    auto radianOld = Execute(hasReturned, currentScope, scope, *body.angleProgram)->number;
    scope.clear();
    scope[L"time"] = std::make_shared<Value>(simulationTime + stepDeltaTime);
    hasReturned = false;
    auto radianNew = Execute(hasReturned, currentScope, scope, *body.angleProgram)->number;
    auto velocityAngularNew = (radianNew - radianOld) / stepDeltaTime;

    body.radian = radianNew;
    body.velocityAngular = velocityAngularNew;
}

void SceneScriptRuntime::FollowPresetAngularVelocity(Body& body, double const& simulationTime)
{
    if (!body.angularVelocityProgram)
    {
        return;
    }

    Scope scope;
    bool hasReturned = false;
    scope[L"time"] = std::make_shared<Value>(simulationTime);
    auto velocityAngularNew = Execute(hasReturned, currentScope, scope, *body.angularVelocityProgram)->number;

    body.velocityAngular = velocityAngularNew;
}
