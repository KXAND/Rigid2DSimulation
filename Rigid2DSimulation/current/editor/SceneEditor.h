#pragma once
#include <memory>

#include "DVector2.h"

struct AppInput;
struct Body;
struct Connection;
struct Creator;
struct InputOwners;
class PhysicsWorld;

int constexpr kDragMode = 0;
int constexpr kSelectMode = 1;
int constexpr kCreateMode = 2;
int constexpr kDeleteMode = 3;

class SceneEditor
{
public:
    int mode = 0;
    std::shared_ptr<Creator> creator;
    Body* bodySelecting = NULL;
    Connection* connectionSelecting = NULL;
    double velocityAngularDrag = 0;

    void ClearSelection();
    void ClearDeletedSelection();
    void ResetTools();
    void CycleMode(InputOwners& inputOwners);
    void RequestBodyDeletion(PhysicsWorld& world);
    void RequestConnectionDeletion(PhysicsWorld& world);
    void PreUpdate(PhysicsWorld const& world, AppInput const& input, InputOwners& inputOwners, DRect viewport);
    void UpdateBodies(PhysicsWorld& world, AppInput& input, InputOwners const& inputOwners,
                      double realDeltaTime, bool isTrackShown, bool isPaused);
    void ApplyDrag(Body& body, AppInput const& input, InputOwners const& inputOwners,
                   double realDeltaTime, double stepDeltaTime);

private:
    void UpdateBody(Body& body, AppInput& input, InputOwners const& inputOwners,
                    double realDeltaTime, bool isTrackShown, bool isPaused);
    void UpdateDragAngle(Body& body, AppInput const& input, InputOwners const& inputOwners, double realDeltaTime);
    void HandleDragWhole(Body& body, AppInput const& input, double realDeltaTime);
    void HandleDragPoint(Body& body, AppInput const& input, double realDeltaTime);
    void HandleDragForce(Body& body, AppInput const& input, double deltaTime);
};
