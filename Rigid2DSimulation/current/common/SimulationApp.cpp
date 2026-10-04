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
#include "Group.h"
#include "Creator.h"
#include "Collision.h"
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
    size = bodies.size();
    fwrite(&(size), sizeof(size), 1, file);
    for (auto body : bodies)
    {
        body->Save(file);
    }
    size = connections.size();
    fwrite(&(size), sizeof(size), 1, file);
    for (auto connection : connections)
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
        bodies.push_back(std::make_shared<Body>(*this, file));
    }
    fread(&(size), sizeof(size), 1, file);
    for (int i = (0), iEnd = int(size); i < iEnd; ++i)
    {
        connections.push_back(std::make_shared<Connection>(*this, file));
    }
    isSceneChanged = true;

    Execute(currentScope, Compile(scriptText));
    fclose(file);
}
void SimulationApp::Reset()
{
    currentScope.clear();
    InitializeDefaultFunctions();
    parameters.clear();

    bodies.clear();
    groups.clear();
    collisions.clear();
    connections.clear();
    debugString.clear();

    currentScope[L"backgroundLeft"] = std::make_shared<Value>(sceneCanvas->topLeft.x);
    currentScope[L"backgroundTop"] = std::make_shared<Value>(sceneCanvas->topLeft.y);
    currentScope[L"backgroundWidth"] = std::make_shared<Value>(sceneCanvas->width);
    currentScope[L"backgroundHeight"] = std::make_shared<Value>(sceneCanvas->height);

    ClearSelection();
    isSceneChanged = false;
    gravity = Vector2();
    maxRealDeltaTime = 0.05;
    simulationTime = 0;
    stepNum = 60;
    rectScene.topLeftPosition = sceneCanvas->topLeft;
    rectScene.width = sceneCanvas->width;
    rectScene.height = sceneCanvas->height;
    gridSize = 50;
    energyMultiplier = 4e-6;
    chargeMultiplier = 1;
    isTrackShown = false;

    hasElectrostatic = false;
    coulombConstant = 1e5;

    parallelEpsilon = 1e-3;
    useEqualPositionCorrection = false;
    velocityAngularDrag = 2;

    creator = std::make_shared<Creator>();
}
void SimulationApp::ClearSelection()
{
    bodySelecting = NULL;
    connectionSelecting = NULL;
}

void SimulationApp::Update()
{
    // pre update
    sceneCanvas->PreUpdate(input, inputOwners);
    if (mode == kCreateMode)
    {
        creator->PreUpdate(*this);
    }
    for (auto& body : bodies)
    {
        body->PreUpdate(*this);
    }
    userInterface->PreUpdate(*this);
    UpdateBasicState();

    if (currentScope.find(L"Update") != currentScope.end())
    {
        auto updateFunction = *currentScope[L"Update"];
        Execute(currentScope, updateFunction.bodyStatements);
    }

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

    // 检查暂停
    if (!inputOwners.keyboardOwner)
    {
        if (input.IsKeyPressedDown(L' '))
        {
            isPaused = !isPaused;
        }
    }

    // 删除被标记为删除的图形和其上的连接
    for (auto body : bodies)
        if (body->isMarkedForDeletion)
            for (auto connection : body->connections)
            {
                connection->isMarkedForDeletion = true;
            }
    if (bodySelecting && bodySelecting->isMarkedForDeletion)
    {
        bodySelecting = NULL;
    }
    if (connectionSelecting && connectionSelecting->isMarkedForDeletion)
    {
        connectionSelecting = NULL;
    }
    for (auto& connection : connections)
        if (connection->isMarkedForDeletion)
        {
            //将该连接从其两端的物体的连接列表中移除。
            auto& firstBodyConnections = connection->body0->connections;
            firstBodyConnections.erase(remove(firstBodyConnections.begin(), firstBodyConnections.end(), &*connection), firstBodyConnections.end());
            auto& secondBodyConnections = connection->body1->connections;
            secondBodyConnections.erase(remove(secondBodyConnections.begin(), secondBodyConnections.end(), &*connection), secondBodyConnections.end());
        }
    connections.erase(
        remove_if(connections.begin(), connections.end(),
                  [](std::shared_ptr<Connection> connection)
                  {
                      return connection->isMarkedForDeletion;
                  }),
        connections.end());
    bodies.erase(
        remove_if(bodies.begin(), bodies.end(),
                  [](std::shared_ptr<Body> body)
                  {
                      return body->isMarkedForDeletion;
                  }),
        bodies.end());

    if (sceneCanvas->IsEmptyAreaClicked(input, inputOwners))
    {
        ClearSelection();
    }

    // 保持底图与调试文字在物体更新、绘制之前输出。
    sceneCanvas->Render(screen);
    DrawString(screen, depthBuffer, 999, debugString,
               Color(110), font, sceneCanvas->topLeft + DVector2(10, 10), sceneCanvas->width - 20, sceneCanvas->GetViewport());

    // 更新运行信息
    realDeltaTime = min(maxRealDeltaTime, frameRate.GetDeltaTime());
    currentScope[L"current.frameRate.GetDeltaTime()"] = std::make_shared<Value>(realDeltaTime);
    gridNumX = ceil(rectScene.width / gridSize);
    gridNumY = ceil(rectScene.height / gridSize);
    if (isSceneChanged)
    {
        isSceneChanged = false;
        groups = FormGroups(bodies);
    }
    for (const auto& body : bodies)
    {
        body->Update(*this);
    }

    // 物理模拟
    if (!isPaused && realDeltaTime != 0)
    {
        // 标准时长
        double stepDeltaTime = realDeltaTime / stepNum;

        for (int i = (0), iEnd = int(stepNum); i < iEnd; ++i)
        {
            currentScope[L"time"] = std::make_shared<Value>(simulationTime);
            for (const auto& body : bodies)
            {
                body->Step(*this, stepDeltaTime);
            }
            // for (auto b : bodies) { b->warp(rectScene); }
            for (auto& group : groups)
            {
                group->Warp(rectScene);
            }

            //计算静电
            if (hasElectrostatic)
            {
                for (int i = (0), iEnd = int(bodies.size() - 1); i < iEnd; ++i)
                    for (int j = (i + 1), jEnd = int(bodies.size()); j < jEnd; ++j)
                    {
                        Electrostatic(*bodies[i], *bodies[j], stepDeltaTime, coulombConstant);
                    }
            }

            // 重新检查物体在grid中的情况
            multiGridBodies.clear();
            grid.clear();
            grid.resize(gridNumX * gridNumY);
            for (auto& body : bodies)
            {
                body->RegisterGrid(*this);
            }

            // 碰撞
            collisions.clear();
            DetectCollisions();
            for (auto& collision : collisions)
            {
                collision->Simulate(useEqualPositionCorrection);
            }
            // for (auto collision : collisions) { collision->Render(*this); }

            // 连接
            for (auto& connection : connections)
            {
                connection->Simulate(stepDeltaTime, useEqualPositionCorrection);
            }

            simulationTime += stepDeltaTime;
        }
    }

    // 渲染
    for (auto& body : bodies)
    {
        body->UpdateShapesPositionAndAABB();
        body->Render(*this);
    }
    for (auto& connection : connections)
    {
        connection->UpdatePosition();
        connection->Render(*this);
    }

    if (mode == kCreateMode)
    {
        creator->Update(*this);
    }
    userInterface->Update(*this);
}

void SimulationApp::SetConfig(Value const& config)
{
    TryReadConfigNumber(config, L"volume", volume);
    TryReadConfigNumber(config, L"energyMultiplier", energyMultiplier);
    TryReadConfigNumber(config, L"chargeMultiplier", chargeMultiplier);
    TryReadConfigNumber(config, L"parallelEpsilon", parallelEpsilon);
    TryReadConfigNumber(config, L"maxRealDeltaTime", maxRealDeltaTime);
    TryReadConfigNumber(config, L"hasElectrostatic", hasElectrostatic);
    TryReadConfigNumber(config, L"coulombConstant", coulombConstant);
    TryReadConfigNumber(config, L"stepCount", stepNum);
    stepNum = max(1, stepNum);
    TryReadConfigNumber(config, L"gridSize", gridSize);
    TryReadConfigNumber(config, L"useEqualPositionCorrection", useEqualPositionCorrection);
    TryReadConfigNumber(config, L"time", simulationTime);
    TryReadConfigNumber(config, L"isTrackShown", isTrackShown);
    if (auto value = FindConfigValue(config, L"sceneLeft"))
    {
        rectScene.topLeftPosition.x = value->number;
    }
    if (auto value = FindConfigValue(config, L"sceneTop"))
    {
        rectScene.topLeftPosition.y = value->number;
    }
    if (auto value = FindConfigValue(config, L"sceneWidth"))
    {
        rectScene.width = value->number;
    }
    if (auto value = FindConfigValue(config, L"sceneHeight"))
    {
        rectScene.height = value->number;
    }
    if (auto value = FindConfigValue(config, L"gravity"))
    {
        gravity = ToVector2(*value);
    }

    isSceneChanged = true;
}

void SimulationApp::DetectCollisions()
{
    for (int i = (0), iEnd = int(gridNumX); i < iEnd; ++i)
        for (int j = (0), jEnd = int(gridNumY); j < jEnd; ++j)
        {
            auto& bodies = grid[j * gridNumX + i];
            for (int k = (0), kEnd = int(bodies.size()); k < kEnd; ++k)
            {
                auto& body = *bodies[k];

                // 检查同grid和跨grid的情况，输出到collisions中
                for (int m = (k + 1), mEnd = int(bodies.size()); m < mEnd; ++m)
                {
                    Collide(body, *bodies[m], collisions, parallelEpsilon);
                }
                for (auto otherBody : multiGridBodies)
                {
                    Collide(body, *otherBody, collisions, parallelEpsilon);
                }

                // 检查右邻
                if (i + 1 < gridNumX)
                {
                    int a = i + 1, b = j;
                    for (auto otherBody : grid[b * gridNumX + a])
                    {
                        Collide(body, *otherBody, collisions, parallelEpsilon);
                    }
                }
                if (j + 1 >= gridNumY)
                {
                    continue;
                } // 检查下邻，没有下邻就无法进行下列操作了

                //检查左下
                if (i - 1 >= 0)
                {
                    int a = i - 1, b = j + 1;
                    for (auto otherBody : grid[b * gridNumX + a])
                    {
                        Collide(body, *otherBody, collisions, parallelEpsilon);
                    }
                }
                // 检查右下
                if (i + 1 < gridNumX)
                {
                    int a = i + 1, b = j + 1;
                    for (auto otherBody : grid[b * gridNumX + a])
                    {
                        Collide(body, *otherBody, collisions, parallelEpsilon);
                    }
                }
                // 检查正下
                int a = i, b = j + 1;
                for (auto otherBody : grid[b * gridNumX + a])
                {
                    Collide(body, *otherBody, collisions, parallelEpsilon);
                }
            }
        }

    // 检查多gird和多gird本身的碰撞
    for (int i = (0), iEnd = int(multiGridBodies.size() - 1); i < iEnd; ++i)
        for (int j = (i + 1), jEnd = int(multiGridBodies.size()); j < jEnd; ++j)
        {
            Collide(*multiGridBodies[i], *multiGridBodies[j], collisions, parallelEpsilon);
        }
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
            creator = std::make_shared<Creator>(*arguments[0]);
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
        return std::make_shared<Value>(bodies.size());
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
