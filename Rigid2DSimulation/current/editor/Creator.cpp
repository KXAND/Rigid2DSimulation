#include "Creator.h"
#include "AppItem.h"
#include "Body.h"
#include "BodyConfig.h"
#include "Connection.h"
#include "ConnectionConfig.h"
#include "PhysicsWorld.h"
#include "Shape.h"
#include "Value.h"
#include "current/common/utils.h"

Creator::Creator()
{
    bodyDepth = 10;
    connectionDepth = -1;
    thickness = 10;
    radius = 8;
}
Creator::Creator(Value const& config) : Creator()
{
    if (auto modeValue = FindConfigValue(config, L"mode"))
    {
        if (modeValue->type == L"str")
        {
            auto modeName = modeValue->stringValue;
            if (modeName == L"box")
            {
                mode = kCreateBox;
            }
            else if (modeName == L"ball")
            {
                mode = kCreateBall;
            }
            else if (modeName == L"plate")
            {
                mode = kCreatePlate;
            }
            else if (modeName == L"connection")
            {
                mode = kCreateConnection;
            }
            else if (modeName == L"point")
            {
                mode = kCreatePoint;
            }
            else if (modeName == L"nail")
            {
                mode = kCreateNail;
            }
            else if (modeName == L"particle")
            {
                mode = kCreateParticle;
            }
        }
        else
        {
            mode = modeValue->number;
        }
    }
    TryReadConfigNumber(config, L"thickness", thickness);
    TryReadConfigNumber(config, L"radius", radius);
    if (auto value = FindConfigValue(config, L"bodyConfiguration"))
    {
        bodyConfiguration = std::make_shared<Value>(*value);
    }
    if (auto value = FindConfigValue(config, L"connectionConfiguration"))
    {
        connectionConfiguration = std::make_shared<Value>(*value);
    }
}

double Creator::Depth() const
{
    switch (mode)
    {
    case kCreateConnection:
    case kCreateNail:
        return connectionDepth;
    default:
        break;
    }
    return bodyDepth;
}

void Creator::UpdateBox(PhysicsWorld& world, AppInput& input)
{
    if (active)
    {
        double x0 = creationStartPoint.x, y0 = creationStartPoint.y;
        double x1 = input.mousePosition.x, y1 = input.mousePosition.y;
        double minimumSize = 10;
        if (x1 - x0 >= minimumSize && y1 - y0 >= minimumSize)
        {
            body = std::make_shared<Body>(std::make_shared<Shape>(GetRectVerticesByCenter(x1 - x0, y1 - y0)));
            body->position = Vector2(x0 + x1, y0 + y1) / 2;
            body->appearance.innerColor = Color(GenerateRandomInt(256), GenerateRandomInt(256), GenerateRandomInt(256));
            if (bodyConfiguration)
            {
                ApplyBodyConfig(*body, *bodyConfiguration);
            }
            body->Init();
        }

        if (!input.mouseButtonDown[0])
        {
            active = false;
            if (body)
            {
                world.bodies.push_back(body);
                body = NULL;
                world.isSceneChanged = true;
            }
        }
    }
    else
    {
        if (hovered && input.IsMouseClickedDown(0))
        {
            active = true;
            creationStartPoint = input.mousePosition;
        }
    }
}
void Creator::UpdateBall(PhysicsWorld& world, AppInput& input)
{
    if (active)
    {
        double dragRadius = sqrt((creationStartPoint - input.mousePosition).GetLengthSquared());
        double minimumRadius = 7.5;
        if (dragRadius > minimumRadius)
        {
            body = std::make_shared<Body>(std::make_shared<Shape>(dragRadius));
            body->position = Vector2(creationStartPoint);
            body->appearance.innerColor = Color(GenerateRandomInt(256), GenerateRandomInt(256), GenerateRandomInt(256));
            if (bodyConfiguration)
            {
                ApplyBodyConfig(*body, *bodyConfiguration);
            }
            body->Init();
        }

        if (!input.mouseButtonDown[0])
        {
            active = false;
            if (body)
            {
                world.bodies.push_back(body);
                body = NULL;
                world.isSceneChanged = true;
            }
        }
    }
    else
    {
        if (hovered && input.IsMouseClickedDown(0))
        {
            active = true;
            creationStartPoint = input.mousePosition;
        }
    }
}
void Creator::UpdatePlate(PhysicsWorld& world, AppInput& input)
{
    if (active)
    {
        double plateThickness = thickness;
        double minimumLength = 5;
        DVector2 dragVector = creationStartPoint - input.mousePosition;
        double dragLen = sqrt(dragVector.GetLengthSquared());
        if (dragLen > minimumLength)
        {
            body = std::make_shared<Body>(std::make_shared<Shape>(GetRectVerticesByCenter(dragLen + plateThickness, plateThickness)));
            body->position = Vector2(creationStartPoint + input.mousePosition) / 2;
            body->radian = atan2(dragVector.y, dragVector.x);
            body->appearance.innerColor = Color(GenerateRandomInt(256), GenerateRandomInt(256), GenerateRandomInt(256));
            if (bodyConfiguration)
            {
                ApplyBodyConfig(*body, *bodyConfiguration);
            }
            body->Init();
        }

        if (!input.mouseButtonDown[0])
        {
            active = false;
            if (body)
            {
                world.bodies.push_back(body);
                body = NULL;
                world.isSceneChanged = true;
            }
        }
    }
    else
    {
        if (hovered && input.IsMouseClickedDown(0))
        {
            active = true;
            creationStartPoint = input.mousePosition;
        }
    }
}
void Creator::UpdateConnection(PhysicsWorld& world, AppInput& input)
{
    if (active)
    {
        connection = std::make_shared<Connection>();
        if (connectionConfiguration)
        {
            ApplyConnectionConfig(*connection, *connectionConfiguration);
        }
        // 要求 creator 的更新在 body 后面对吧，是不是耦合太高了。
        // 要求 creator 的更新在 body 后面对吧，是不是耦合太高了。
        if (!input.mouseButtonDown[0])
        {
            if (body1)
            {
                connection->body0 = body0;
                connection->body1 = body1;
                connection->p0Relative = p0Relative;
                connection->p1Relative = p1Relative;

                connection->SignUpToBodies();
                connection->UpdatePosition();
                connection->length = (connection->p0 - connection->p1).GetLength();
                world.connections.push_back(connection);
                world.isSceneChanged = true;
            }
            connection = NULL;
            body0 = body1 = NULL;
            active = false;
        }
    }
    else
    {
        if (body0)
        {
            active = true;
        }
    }
}
void Creator::UpdatePoint(PhysicsWorld& world, AppInput& input)
{
    if (hovered && input.IsMouseClickedDown(0))
    {
        body = std::make_shared<Body>((Vector2)input.mousePosition);
        body->appearance.innerColor = Color(GenerateRandomInt(256), GenerateRandomInt(256), GenerateRandomInt(256));
        if (bodyConfiguration)
        {
            ApplyBodyConfig(*body, *bodyConfiguration);
        }
        body->Init();
        world.bodies.push_back(body);
        body = NULL;
        world.isSceneChanged = true;
    }
}
void Creator::UpdateNail(PhysicsWorld& world, AppInput& input)
{
    if (body0)
    {
        body = std::make_shared<Body>(body0->position + body0->transform * p0Relative);
        body->Init();
        world.bodies.push_back(body);
        connection = std::make_shared<Connection>();
        connection->type = kLinkConnection;
        connection->body0 = body0;
        connection->body1 = &*body;
        connection->p0Relative = p0Relative;
        connection->SignUpToBodies();
        connection->UpdatePosition();
        world.connections.push_back(connection);

        body0 = NULL;
        connection = NULL;
        body = NULL;
        world.isSceneChanged = true;
    }
}
void Creator::UpdateParticle(PhysicsWorld& world, AppInput& input)
{
    if (hovered && input.IsMouseClickedDown(0))
    {
        body = std::make_shared<Body>(std::make_shared<Shape>(radius));
        body->position = (Vector2)input.mousePosition;
        body->appearance.innerColor = Color(GenerateRandomInt(256), GenerateRandomInt(256), GenerateRandomInt(256));
        if (bodyConfiguration)
        {
            ApplyBodyConfig(*body, *bodyConfiguration);
        }
        body->Init();
        world.bodies.push_back(body);
        body = NULL;
        world.isSceneChanged = true;
    }
}
void Creator::Update(PhysicsWorld& world, AppInput& input, InputOwners const& inputOwners)
{
    hovered = (inputOwners.hovered == this);
    body = NULL;
    connection = NULL;

    switch (mode)
    {
    case kCreateBox:
        UpdateBox(world, input);
        break;
    case kCreateBall:
        UpdateBall(world, input);
        break;
    case kCreatePlate:
        UpdatePlate(world, input);
        break;
    case kCreateConnection:
        UpdateConnection(world, input);
        break;
    case kCreatePoint:
        UpdatePoint(world, input);
        break;
    case kCreateNail:
        UpdateNail(world, input);
        break;
    case kCreateParticle:
        UpdateParticle(world, input);
        break;
    }
}

void Creator::Discard(InputOwners& inputOwners)
{
    inputOwners.Remove(this);
    body0 = body1 = NULL;
    body = NULL;
    connection = NULL;
    hovered = active = false;
}
void Creator::PreUpdate(AppInput const& input, InputOwners& inputOwners, DRect viewport)
{
    if (inputOwners.hoveredDepth <= Depth() && IsInside(input.mousePosition, viewport))
    {
        inputOwners.hoveredDepth = Depth();
        inputOwners.hovered = this;
    }
}
