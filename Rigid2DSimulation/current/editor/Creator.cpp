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

CreatorConfig ParseCreatorConfig(Value const& config)
{
    CreatorConfig result;
    if (auto modeValue = FindConfigValue(config, L"mode"))
    {
        if (modeValue->type == L"str")
        {
            auto modeName = modeValue->stringValue;
            if (modeName == L"box")
            {
                result.mode = kCreateBox;
            }
            else if (modeName == L"ball")
            {
                result.mode = kCreateBall;
            }
            else if (modeName == L"plate")
            {
                result.mode = kCreatePlate;
            }
            else if (modeName == L"connection")
            {
                result.mode = kCreateConnection;
            }
            else if (modeName == L"point")
            {
                result.mode = kCreatePoint;
            }
            else if (modeName == L"nail")
            {
                result.mode = kCreateNail;
            }
            else if (modeName == L"particle")
            {
                result.mode = kCreateParticle;
            }
        }
        else
        {
            result.mode = modeValue->number;
        }
    }
    TryReadConfigNumber(config, L"thickness", result.thickness);
    TryReadConfigNumber(config, L"radius", result.radius);
    if (auto value = FindConfigValue(config, L"bodyConfiguration"))
    {
        result.bodyConfiguration = std::make_shared<Value>(*value);
    }
    if (auto value = FindConfigValue(config, L"connectionConfiguration"))
    {
        result.connectionConfiguration = std::make_shared<Value>(*value);
    }
    return result;
}

void Creator::ApplyConfig(CreatorConfig const& newConfig, InputOwners& inputOwners)
{
    Discard(inputOwners);
    config = newConfig;
}

double Creator::Depth() const
{
    switch (config.mode)
    {
    case kCreateConnection:
    case kCreateNail:
        return config.connectionDepth;
    default:
        break;
    }
    return config.bodyDepth;
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
            auto shape = std::make_shared<Shape>(GetRectVerticesByCenter(x1 - x0, y1 - y0));
            BodyProperties properties;
            properties.position = Vector2(x0 + x1, y0 + y1) / 2;
            properties.innerColor = Color(GenerateRandomInt(256), GenerateRandomInt(256), GenerateRandomInt(256));
            if (config.bodyConfiguration)
            {
                properties = ParseBodyProperties(*config.bodyConfiguration, properties);
            }
            body = std::make_shared<Body>(shape, properties);
        }

        if (!input.mouseButtonDown[0])
        {
            active = false;
            if (body)
            {
                world.AddBody(body);
                body = NULL;
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
            auto shape = std::make_shared<Shape>(dragRadius);
            BodyProperties properties;
            properties.position = Vector2(creationStartPoint);
            properties.innerColor = Color(GenerateRandomInt(256), GenerateRandomInt(256), GenerateRandomInt(256));
            if (config.bodyConfiguration)
            {
                properties = ParseBodyProperties(*config.bodyConfiguration, properties);
            }
            body = std::make_shared<Body>(shape, properties);
        }

        if (!input.mouseButtonDown[0])
        {
            active = false;
            if (body)
            {
                world.AddBody(body);
                body = NULL;
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
        double plateThickness = config.thickness;
        double minimumLength = 5;
        DVector2 dragVector = creationStartPoint - input.mousePosition;
        double dragLen = sqrt(dragVector.GetLengthSquared());
        if (dragLen > minimumLength)
        {
            auto shape = std::make_shared<Shape>(GetRectVerticesByCenter(dragLen + plateThickness, plateThickness));
            BodyProperties properties;
            properties.position = Vector2(creationStartPoint + input.mousePosition) / 2;
            properties.radian = atan2(dragVector.y, dragVector.x);
            properties.innerColor = Color(GenerateRandomInt(256), GenerateRandomInt(256), GenerateRandomInt(256));
            if (config.bodyConfiguration)
            {
                properties = ParseBodyProperties(*config.bodyConfiguration, properties);
            }
            body = std::make_shared<Body>(shape, properties);
        }

        if (!input.mouseButtonDown[0])
        {
            active = false;
            if (body)
            {
                world.AddBody(body);
                body = NULL;
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
        connection = std::make_shared<Connection>(body0, body1);
        if (config.connectionConfiguration)
        {
            ApplyConnectionConfig(*connection, *config.connectionConfiguration);
        }
        // 使用输入阶段确认时的姿态确定连接长度，新连接随后参与本帧模拟。
        if (!input.mouseButtonDown[0])
        {
            if (body1)
            {
                connection->p0Relative = p0Relative;
                connection->p1Relative = p1Relative;
                connection->UpdatePosition();
                connection->length = (connection->p0 - connection->p1).GetLength();
                world.AddConnection(connection);
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
        BodyProperties properties;
        properties.position = (Vector2)input.mousePosition;
        properties.innerColor = Color(GenerateRandomInt(256), GenerateRandomInt(256), GenerateRandomInt(256));
        if (config.bodyConfiguration)
        {
            properties = ParseBodyProperties(*config.bodyConfiguration, properties);
        }
        body = Body::CreatePoint(properties);
        world.AddBody(body);
        body = NULL;
    }
}
void Creator::UpdateNail(PhysicsWorld& world, AppInput& input)
{
    if (body0)
    {
        body = Body::CreatePoint(body0->position + body0->transform * p0Relative);
        world.AddBody(body);
        connection = std::make_shared<Connection>(body0, body.get());
        connection->type = kLinkConnection;
        connection->p0Relative = p0Relative;
        connection->UpdatePosition();
        world.AddConnection(connection);

        body0 = NULL;
        connection = NULL;
        body = NULL;
    }
}
void Creator::UpdateParticle(PhysicsWorld& world, AppInput& input)
{
    if (hovered && input.IsMouseClickedDown(0))
    {
        auto shape = std::make_shared<Shape>(config.radius);
        BodyProperties properties;
        properties.position = (Vector2)input.mousePosition;
        properties.innerColor = Color(GenerateRandomInt(256), GenerateRandomInt(256), GenerateRandomInt(256));
        if (config.bodyConfiguration)
        {
            properties = ParseBodyProperties(*config.bodyConfiguration, properties);
        }
        body = std::make_shared<Body>(shape, properties);
        world.AddBody(body);
        body = NULL;
    }
}

void Creator::OnBodyPressed(Body& hitBody, Vector2 const& mousePosition)
{
    if (hitBody.IsMarkedForDeletion())
    {
        return;
    }
    if ((config.mode == kCreateConnection || config.mode == kCreateNail) && !active)
    {
        body0 = &hitBody;
        p0Relative = hitBody.transform.Inverse() * (mousePosition - hitBody.position);
    }
}

void Creator::OnBodyReleased(Body& hitBody, Vector2 const& mousePosition)
{
    if (hitBody.IsMarkedForDeletion())
    {
        return;
    }
    if ((config.mode == kCreateConnection || config.mode == kCreateNail) && active)
    {
        body1 = &hitBody;
        p1Relative = hitBody.transform.Inverse() * (mousePosition - hitBody.position);
    }
}

void Creator::Update(PhysicsWorld& world, AppInput& input, InputOwners const& inputOwners)
{
    hovered = (inputOwners.hovered == this);
    previewTarget = (Vector2)input.mousePosition;
    body = NULL;
    connection = NULL;

    switch (config.mode)
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

CreationPreview Creator::GetPreview() const
{
    return {body.get(), body0 ? connection.get() : nullptr, p0Relative, previewTarget};
}

void Creator::PrepareForDeletion(InputOwners& inputOwners)
{
    if ((body0 && body0->IsMarkedForDeletion()) || (body1 && body1->IsMarkedForDeletion()))
    {
        Discard(inputOwners);
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
