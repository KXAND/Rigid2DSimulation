#include "SimulationApp.h"
#include "Utils.h"
#include "UserInterface.h"
#include "SceneCanvas.h"
#include "SceneParameter.h"
#include "Shape.h"
#include "Body.h"
#include "ShapeBuiltins.h"
#include "SceneBuilders.h"
#include "AudioClip.h"
#include "Creator.h"
#include "Connection.h"
#include "DrawString.h"

SimulationApp::SimulationApp()
{
    width = 1800;
    height = 900;
    SetLocale();
    waveOutput.blockDuration = 4 * 5e-4;

    PrintConsole(L"正在加载字体...");
    App::Init();

    PrintConsole(L"字体加载完成.", true);

    PrintConsole(L"正在加载控件...");
    userInterface = std::make_shared<UserInterface>(*this);
    userInterface->Init(*this);
    sceneCanvas = std::make_shared<SceneCanvas>();
    Reset();

    PrintConsole(L"控件加载完成.", true);

    PrintConsole(L"正在加载音乐...");
    bool audioLoaded = false;
    firstMusicClip = std::make_shared<AudioClip>(L"sound/bgm0.clip", &audioLoaded);
    if (!audioLoaded)
    {
        firstMusicClip = NULL;
    }
    secondMusicClip = std::make_shared<AudioClip>(L"sound/bgm1.clip", &audioLoaded);
    if (!audioLoaded)
    {
        secondMusicClip = NULL;
    }
    PrintConsole(L"音乐加载完成.", true);

    volume = 1;
    currentMusicClip = GenerateRandomFloat(1) < 0.5 ? &*firstMusicClip : &*secondMusicClip;
}

void SimulationApp::Save(wstring const& name) const
{
    FILE* file = OpenFile(name, L"wb");
    if (!file)
    {
        return;
    }
    int size = 0;
    size = (scriptText).size();
    fwrite(&(size), sizeof(size), 1, file);
    fwrite(&*(scriptText).begin(), sizeof(*(&*(scriptText).begin())), size, file);
    SaveParameters(file);

    // 可能会保存一些即将被删除的元素，但是也无所谓。
    size = physicsWorld.bodies.size();
    fwrite(&(size), sizeof(size), 1, file);
    for (auto body : physicsWorld.bodies)
    {
        body->Save(file);
    }
    size = physicsWorld.connections.size();
    fwrite(&(size), sizeof(size), 1, file);
    for (auto connection : physicsWorld.connections)
    {
        connection->Save(*this, file);
    }

    fclose(file);
}
void SimulationApp::Load(wstring const& name)
{
    FILE* file = OpenFile(name, L"rb");
    if (!file)
    {
        return;
    }
    Reset();
    int size = 0;
    fread(&(size), sizeof(size), 1, file);
    (scriptText).resize(size);
    fread(&*(scriptText).begin(), sizeof(*(&*(scriptText).begin())), size, file);
    temporaryScriptText = scriptText;
    LoadParameters(file);

    fread(&(size), sizeof(size), 1, file);
    for (int i = (0), iEnd = int(size); i < iEnd; ++i)
    {
        physicsWorld.bodies.push_back(std::make_shared<Body>(*this, file));
    }
    fread(&(size), sizeof(size), 1, file);
    for (int i = (0), iEnd = int(size); i < iEnd; ++i)
    {
        physicsWorld.connections.push_back(std::make_shared<Connection>(*this, file));
    }
    physicsWorld.isSceneChanged = true;

    Execute(currentScope, Compile(scriptText));
    fclose(file);
}
void SimulationApp::Reset()
{
    currentScope.clear();
    InitializeDefaultFunctions();
    parameters.clear();

    physicsWorld.ClearObjects();
    debugString.clear();

    currentScope[L"backgroundLeft"] = std::make_shared<Value>(sceneCanvas->topLeft.x);
    currentScope[L"backgroundTop"] = std::make_shared<Value>(sceneCanvas->topLeft.y);
    currentScope[L"backgroundWidth"] = std::make_shared<Value>(sceneCanvas->width);
    currentScope[L"backgroundHeight"] = std::make_shared<Value>(sceneCanvas->height);

    editor.ClearSelection();
    physicsWorld.ResetParameters(DRect(sceneCanvas->topLeft, sceneCanvas->width, sceneCanvas->height));
    maxRealDeltaTime = 0.05;
    energyMultiplier = 4e-6;
    chargeMultiplier = 1;
    isTrackShown = false;

    editor.ResetTools();
}

void SimulationApp::Update()
{
    ResolveInputOwnership();

    UpdateBasicState();

    if (currentScope.find(L"Update") != currentScope.end())
    {
        auto updateFunction = *currentScope[L"Update"];
        Execute(currentScope, updateFunction.bodyStatements);
    }

    UpdateMusic();

    // 检查暂停
    if (!inputOwners.keyboardOwner)
    {
        if (input.IsKeyPressedDown(L' '))
        {
            isPaused = !isPaused;
        }
    }

    RemoveDeletedObjects();

    if (sceneCanvas->IsEmptyAreaClicked(input, inputOwners))
    {
        editor.ClearSelection();
    }

    // 保持底图与调试文字在物体更新、绘制之前输出。
    sceneCanvas->Render(screen);
    DrawString(screen, depthBuffer, 999, debugString,
               Color(110), font, sceneCanvas->topLeft + DVector2(10, 10), sceneCanvas->width - 20, sceneCanvas->GetViewport());

    PrepareSimulation();

    editor.UpdateBodies(physicsWorld, input, inputOwners, realDeltaTime, isTrackShown, isPaused);

    AdvanceSimulation();

    RenderSceneObjects();

    if (editor.mode == kCreateMode)
    {
        editor.creator->Update(physicsWorld, input, inputOwners);
        editor.creator->RenderPreview(*this);
    }
    userInterface->Update(*this);
}

void SimulationApp::ResolveInputOwnership()
{
    sceneCanvas->PreUpdate(input, inputOwners);
    editor.PreUpdate(physicsWorld, input, inputOwners, sceneCanvas->GetViewport());
    userInterface->PreUpdate(*this);
}

void SimulationApp::UpdateMusic()
{
    // 音乐
    if (firstMusicClip && secondMusicClip)
    {
        if (currentMusicClip->sampleIndex >= currentMusicClip->GetSampleCount())
        {
            currentMusicClip->sampleIndex = 0;
            currentMusicClip = (currentMusicClip == &*firstMusicClip) ? &*secondMusicClip : &*firstMusicClip;
        }
        if (!isMuted)
        {
            currentMusicClip->Play(waveOutput.inputSamples);
        }
    }
}

void SimulationApp::RemoveDeletedObjects()
{
    physicsWorld.MarkDeletedConnections();
    editor.ClearDeletedSelection();
    physicsWorld.RemoveDeletedObjects();
}

void SimulationApp::PrepareSimulation()
{
    // 更新运行信息
    realDeltaTime = min(maxRealDeltaTime, frameRate.GetDeltaTime());
    currentScope[L"current.frameRate.GetDeltaTime()"] = std::make_shared<Value>(realDeltaTime);
    physicsWorld.Prepare();
}

void SimulationApp::AdvanceSimulation()
{
    if (!isPaused && realDeltaTime != 0)
    {
        physicsWorld.Advance(realDeltaTime,
            [this](double time)
            {
                currentScope[L"time"] = std::make_shared<Value>(time);
            },
            [this](Body& body, double stepDeltaTime)
            {
                body.Step(*this, stepDeltaTime);
                editor.ApplyDrag(body, input, inputOwners, realDeltaTime, stepDeltaTime);
                body.UpdateShapesPositionAndAABB();
            });
    }
}

void SimulationApp::RenderSceneObjects()
{
    // 渲染
    for (auto& body : physicsWorld.bodies)
    {
        body->UpdateShapesPositionAndAABB();
        body->Render(*this);
    }
    for (auto& connection : physicsWorld.connections)
    {
        connection->UpdatePosition();
        connection->Render(*this);
    }
}

void SimulationApp::SetConfig(Value const& config)
{
    TryReadConfigNumber(config, L"volume", volume);
    TryReadConfigNumber(config, L"energyMultiplier", energyMultiplier);
    TryReadConfigNumber(config, L"chargeMultiplier", chargeMultiplier);
    TryReadConfigNumber(config, L"parallelEpsilon", physicsWorld.parallelEpsilon);
    TryReadConfigNumber(config, L"maxRealDeltaTime", maxRealDeltaTime);
    TryReadConfigNumber(config, L"hasElectrostatic", physicsWorld.hasElectrostatic);
    TryReadConfigNumber(config, L"coulombConstant", physicsWorld.coulombConstant);
    TryReadConfigNumber(config, L"stepCount", physicsWorld.stepNum);
    physicsWorld.stepNum = max(1, physicsWorld.stepNum);
    TryReadConfigNumber(config, L"gridSize", physicsWorld.gridSize);
    TryReadConfigNumber(config, L"useEqualPositionCorrection", physicsWorld.useEqualPositionCorrection);
    TryReadConfigNumber(config, L"time", physicsWorld.simulationTime);
    TryReadConfigNumber(config, L"isTrackShown", isTrackShown);
    if (auto value = FindConfigValue(config, L"sceneLeft"))
    {
        physicsWorld.rectScene.topLeftPosition.x = value->number;
    }
    if (auto value = FindConfigValue(config, L"sceneTop"))
    {
        physicsWorld.rectScene.topLeftPosition.y = value->number;
    }
    if (auto value = FindConfigValue(config, L"sceneWidth"))
    {
        physicsWorld.rectScene.width = value->number;
    }
    if (auto value = FindConfigValue(config, L"sceneHeight"))
    {
        physicsWorld.rectScene.height = value->number;
    }
    if (auto value = FindConfigValue(config, L"gravity"))
    {
        physicsWorld.gravity = ToVector2(*value);
    }

    physicsWorld.isSceneChanged = true;
}

void SimulationApp::SaveParameters(FILE* file) const
{
    int size = parameters.size();
    fwrite(&(size), sizeof(size), 1, file);
    for (auto parameter : parameters)
    {
        parameter->Save(file);
    }
}
void SimulationApp::LoadParameters(FILE* file)
{
    int size = 0;
    fread(&(size), sizeof(size), 1, file);
    for (int i = (0), iEnd = int(size); i < iEnd; ++i)
    {
        parameters.push_back(std::make_shared<SceneParameter>(file));
    }
}
void SimulationApp::InitializeDefaultFunctions()
{
    auto setCreatorFunction = [this](vector<std::shared_ptr<Value>> const& arguments)
    {
        if (arguments.size() >= 1)
        {
            editor.creator = std::make_shared<Creator>(*arguments[0]);
        }
        return std::make_shared<Value>();
    };
    currentScope[L"SetCreator"] = std::make_shared<Value>(setCreatorFunction);

    auto createBodyFunction = [this](vector<std::shared_ptr<Value>> const& arguments)
    {
        if (arguments.size() >= 2)
        {
            CreateBody(*this, *arguments[0], *arguments[1]);
        }
        return std::make_shared<Value>();
    };
    currentScope[L"CreateBody"] = std::make_shared<Value>(createBodyFunction);

    auto boxFunction = [this](vector<std::shared_ptr<Value>> const& arguments)
    {
        if (arguments.size() >= 2)
        {
            return Box(*arguments[0], *arguments[1]);
        }
        return std::make_shared<Value>();
    };
    currentScope[L"Box"] = std::make_shared<Value>(boxFunction);

    auto ballFunction = [this](vector<std::shared_ptr<Value>> const& arguments)
    {
        if (arguments.size() >= 1)
        {
            return Ball(*arguments[0]);
        }
        return std::make_shared<Value>();
    };
    currentScope[L"Ball"] = std::make_shared<Value>(ballFunction);

    auto parallelogramFunction = [this](vector<std::shared_ptr<Value>> const& arguments)
    {
        if (arguments.size() >= 2)
        {
            return Parallelogram(*arguments[0], *arguments[1], *arguments[2]);
        }
        return std::make_shared<Value>();
    };
    currentScope[L"Parallelogram"] = std::make_shared<Value>(parallelogramFunction);

    auto trapezoidFunction = [this](vector<std::shared_ptr<Value>> const& arguments)
    {
        if (arguments.size() >= 3)
        {
            return Trapezoid(*arguments[0], *arguments[1], *arguments[2]);
        }
        return std::make_shared<Value>();
    };
    currentScope[L"Trapezoid"] = std::make_shared<Value>(trapezoidFunction);

    auto regularPolygonFunction = [this](vector<std::shared_ptr<Value>> const& arguments)
    {
        if (arguments.size() >= 2)
        {
            return RegularPolygon(*arguments[0], *arguments[1]);
        }
        return std::make_shared<Value>();
    };
    currentScope[L"RegularPolygon"] = std::make_shared<Value>(regularPolygonFunction);

    auto crossFunction = [this](vector<std::shared_ptr<Value>> const& arguments)
    {
        if (arguments.size() >= 2)
        {
            return Cross(*arguments[0], *arguments[1]);
        }
        return std::make_shared<Value>();
    };
    currentScope[L"Cross"] = std::make_shared<Value>(crossFunction);

    auto getBodyCountFunction = [this](vector<std::shared_ptr<Value>> const& arguments)
    {
        return std::make_shared<Value>(physicsWorld.bodies.size());
    };
    currentScope[L"GetBodyCount"] = std::make_shared<Value>(getBodyCountFunction);

    auto createConnectionFunction = [this](vector<std::shared_ptr<Value>> const& arguments)
    {
        if (arguments.size() >= 1)
        {
            CreateConnection(*this, *arguments[0]);
        }
        return std::make_shared<Value>();
    };
    currentScope[L"CreateConnection"] = std::make_shared<Value>(createConnectionFunction);

    auto setConfigFunction = [this](vector<std::shared_ptr<Value>> const& arguments)
    {
        if (arguments.size() >= 1)
        {
            SetConfig(*arguments[0]);
        }
        return std::make_shared<Value>();
    };
    currentScope[L"SetConfig"] = std::make_shared<Value>(setConfigFunction);

    auto boundaryFunction = [this](vector<std::shared_ptr<Value>> const& arguments)
    {
        if (arguments.size() >= 2)
        {
            Boundary(*this, *arguments[0], *arguments[1]);
        }
        return std::make_shared<Value>();
    };
    currentScope[L"Boundary"] = std::make_shared<Value>(boundaryFunction);

    auto framedBoxFunction = [this](vector<std::shared_ptr<Value>> const& arguments)
    {
        if (arguments.size() >= 3)
        {
            return FramedBox(*arguments[0], *arguments[1], *arguments[2]);
        }
        return std::make_shared<Value>();
    };
    currentScope[L"FramedBox"] = std::make_shared<Value>(framedBoxFunction);

    auto gearFunction = [this](vector<std::shared_ptr<Value>> const& arguments)
    {
        if (arguments.size() >= 4)
        {
            Gear(*this, *arguments[0], *arguments[1], *arguments[2], *arguments[3]);
        }
        return std::make_shared<Value>();
    };
    currentScope[L"Gear"] = std::make_shared<Value>(gearFunction);

    auto strandFunction = [this](vector<std::shared_ptr<Value>> const& arguments)
    {
        if (arguments.size() >= 6)
        {
            Strand(*this, *arguments[0], *arguments[1], *arguments[2], *arguments[3], *arguments[4], *arguments[5]);
        }
        return std::make_shared<Value>();
    };
    currentScope[L"Strand"] = std::make_shared<Value>(strandFunction);

    auto necklaceFunction = [this](vector<std::shared_ptr<Value>> const& arguments)
    {
        if (arguments.size() >= 6)
        {
            Necklace(*this, *arguments[0], *arguments[1], *arguments[2], *arguments[3], *arguments[4], *arguments[5]);
        }
        return std::make_shared<Value>();
    };
    currentScope[L"Necklace"] = std::make_shared<Value>(necklaceFunction);
}
void SimulationApp::UpdateBasicState()
{
    title = GetLocalizedString(L"title");
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
}
