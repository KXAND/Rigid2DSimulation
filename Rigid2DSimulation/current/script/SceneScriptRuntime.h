#pragma once
#include <memory>
#include <string>
#include <vector>

#include "Value.h"

class Body;
class Connection;
class PhysicsWorld;
class SceneCanvas;
class SceneEditor;
struct SceneParameter;
class SceneRenderer;

struct SceneScriptBindings
{
    PhysicsWorld& physicsWorld;
    SceneEditor& editor;
    SceneRenderer& sceneRenderer;
    SceneCanvas const& sceneCanvas;
    double& volume;
    double& maxRealDeltaTime;
    bool& isTrackShown;
};

class SceneScriptRuntime
{
public:
    Scope currentScope;
    std::wstring scriptText, temporaryScriptText, debugString;
    std::vector<std::shared_ptr<SceneParameter>> parameters;

    void ResetScope(SceneScriptBindings bindings);
    void ResetSceneVariables(SceneCanvas const& canvas);
    void Update();
    // App 在每帧 Update 前设置；Update 同步面板参数后再发布到 DSL。
    void SetFrameDeltaTime(double deltaTime);
    void SetSimulationTime(double time);
    void ExecuteSceneScript();
    void ApplySceneScript();
    void RefreshBodyScript(Body& body);
    void RefreshConnectionScript(Connection& connection);
    void ApplyMotionPresets(Body& body, bool isDynamic, double const& simulationTime, double stepDeltaTime);
    std::wstring const& GetError() const;
    // 只消费待记录消息；错误状态保留至重新应用脚本或重置。
    std::wstring ConsumeError();

private:
    double frameDeltaTime = 0;
    bool isExecutingMotionPreset = false;
    std::wstring lastError;
    bool hasPendingError = false;

    void ClearError();
    void RequireSceneMutationAllowed(wchar_t const* functionName) const;
    void InitializeDefaultFunctions(SceneScriptBindings bindings);
    void SetConfig(Value const& config, SceneScriptBindings bindings);
    void FollowPresetPosition(Body& body, double const& simulationTime, double stepDeltaTime);
    void FollowPresetAngle(Body& body, double const& simulationTime, double stepDeltaTime);
    void FollowPresetAngularVelocity(Body& body, double const& simulationTime);
};
