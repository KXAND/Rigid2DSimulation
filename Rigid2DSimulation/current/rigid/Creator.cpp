#include "Creator.h"
#include "current/common/utils.h"
#include "SimulationApp.h"
#include "Background.h"
#include "Value.h"
#include "Body.h"
#include "Shape.h"
#include "Connection.h"

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

void Creator::UpdateBox(SimulationApp& simulationApp)
{
    if (active)
    {
        double x0 = creationStartPoint.x, y0 = creationStartPoint.y;
        double x1 = simulationApp.input.mousePosition.x, y1 = simulationApp.input.mousePosition.y;
        double minimumSize = 10;
        if (x1 - x0 >= minimumSize && y1 - y0 >= minimumSize)
        {
            body = std::make_shared<Body>(std::make_shared<Shape>(GetRectVerticesByCenter(x1 - x0, y1 - y0)));
            body->position = Vector2(x0 + x1, y0 + y1) / 2;
            body->innerColor = Color(GenerateRandomInt(256), GenerateRandomInt(256), GenerateRandomInt(256));
            if (bodyConfiguration)
            {
                body->ReadConfig(*bodyConfiguration);
            }
            body->Init();
        }

        if (!simulationApp.input.mouseButtonDown[0])
        {
            active = false;
            if (body)
            {
                simulationApp.bodies.push_back(body);
                body = NULL;
                simulationApp.isSceneChanged = true;
            }
        }
    }
    else
    {
        if (hovered && simulationApp.input.IsMouseClickedDown(0))
        {
            active = true;
            creationStartPoint = simulationApp.input.mousePosition;
        }
    }
}
void Creator::UpdateBall(SimulationApp& simulationApp)
{
    if (active)
    {
        double dragRadius = sqrt((creationStartPoint - simulationApp.input.mousePosition).GetLengthSquared());
        double minimumRadius = 7.5;
        if (dragRadius > minimumRadius)
        {
            body = std::make_shared<Body>(std::make_shared<Shape>(dragRadius));
            body->position = Vector2(creationStartPoint);
            body->innerColor = Color(GenerateRandomInt(256), GenerateRandomInt(256), GenerateRandomInt(256));
            if (bodyConfiguration)
            {
                body->ReadConfig(*bodyConfiguration);
            }
            body->Init();
        }

        if (!simulationApp.input.mouseButtonDown[0])
        {
            active = false;
            if (body)
            {
                simulationApp.bodies.push_back(body);
                body = NULL;
                simulationApp.isSceneChanged = true;
            }
        }
    }
    else
    {
        if (hovered && simulationApp.input.IsMouseClickedDown(0))
        {
            active = true;
            creationStartPoint = simulationApp.input.mousePosition;
        }
    }
}
void Creator::UpdatePlate(SimulationApp& simulationApp)
{
    if (active)
    {
        double plateThickness = thickness;
        double minimumLength = 5;
        DVector2 dragVector = creationStartPoint - simulationApp.input.mousePosition;
        double dragLen = sqrt(dragVector.GetLengthSquared());
        if (dragLen > minimumLength)
        {
            body = std::make_shared<Body>(std::make_shared<Shape>(GetRectVerticesByCenter(dragLen + plateThickness, plateThickness)));
            body->position = Vector2(creationStartPoint + simulationApp.input.mousePosition) / 2;
            body->radian = atan2(dragVector.y, dragVector.x);
            body->innerColor = Color(GenerateRandomInt(256), GenerateRandomInt(256), GenerateRandomInt(256));
            if (bodyConfiguration)
            {
                body->ReadConfig(*bodyConfiguration);
            }
            body->Init();
        }

        if (!simulationApp.input.mouseButtonDown[0])
        {
            active = false;
            if (body)
            {
                simulationApp.bodies.push_back(body);
                body = NULL;
                simulationApp.isSceneChanged = true;
            }
        }
    }
    else
    {
        if (hovered && simulationApp.input.IsMouseClickedDown(0))
        {
            active = true;
            creationStartPoint = simulationApp.input.mousePosition;
        }
    }
}
void Creator::UpdateConnection(SimulationApp& simulationApp)
{
    if (active)
    {
        connection = std::make_shared<Connection>();
        if (connectionConfiguration)
        {
            connection->ReadConfig(*connectionConfiguration);
        }
        // 要求 creator 的更新在 body 后面对吧，是不是耦合太高了。
        // 要求 creator 的更新在 body 后面对吧，是不是耦合太高了。
        if (!simulationApp.input.mouseButtonDown[0])
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
                simulationApp.connections.push_back(connection);
                simulationApp.isSceneChanged = true;
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
void Creator::UpdatePoint(SimulationApp& simulationApp)
{
    if (hovered && simulationApp.input.IsMouseClickedDown(0))
    {
        body = std::make_shared<Body>((Vector2)simulationApp.input.mousePosition);
        body->innerColor = Color(GenerateRandomInt(256), GenerateRandomInt(256), GenerateRandomInt(256));
        if (bodyConfiguration)
        {
            body->ReadConfig(*bodyConfiguration);
        }
        body->Init();
        simulationApp.bodies.push_back(body);
        body = NULL;
        simulationApp.isSceneChanged = true;
    }
}
void Creator::UpdateNail(SimulationApp& simulationApp)
{
    if (body0)
    {
        body = std::make_shared<Body>(body0->position + body0->transform * p0Relative);
        body->Init();
        simulationApp.bodies.push_back(body);
        connection = std::make_shared<Connection>();
        connection->type = kLinkConnection;
        connection->body0 = body0;
        connection->body1 = &*body;
        connection->p0Relative = p0Relative;
        connection->SignUpToBodies();
        connection->UpdatePosition();
        simulationApp.connections.push_back(connection);

        body0 = NULL;
        connection = NULL;
        body = NULL;
        simulationApp.isSceneChanged = true;
    }
}
void Creator::UpdateParticle(SimulationApp& simulationApp)
{
    if (hovered && simulationApp.input.IsMouseClickedDown(0))
    {
        body = std::make_shared<Body>(std::make_shared<Shape>(radius));
        body->position = (Vector2)simulationApp.input.mousePosition;
        body->innerColor = Color(GenerateRandomInt(256), GenerateRandomInt(256), GenerateRandomInt(256));
        if (bodyConfiguration)
        {
            body->ReadConfig(*bodyConfiguration);
        }
        body->Init();
        simulationApp.bodies.push_back(body);
        body = NULL;
        simulationApp.isSceneChanged = true;
    }
}
void Creator::Update(SimulationApp& simulationApp)
{
    hovered = (simulationApp.inputOwners.hovered == this);
    body = NULL;
    connection = NULL;

    switch (mode)
    {
    case kCreateBox:
        UpdateBox(simulationApp);
        break;
    case kCreateBall:
        UpdateBall(simulationApp);
        break;
    case kCreatePlate:
        UpdatePlate(simulationApp);
        break;
    case kCreateConnection:
        UpdateConnection(simulationApp);
        break;
    case kCreatePoint:
        UpdatePoint(simulationApp);
        break;
    case kCreateNail:
        UpdateNail(simulationApp);
        break;
    case kCreateParticle:
        UpdateParticle(simulationApp);
        break;
    }

    if (body)
    {
        body->Render(simulationApp);
    }
    if (connection && body0)
    {
        connection->p0 = body0->position + body0->transform * p0Relative;
        connection->p1 = (Vector2)simulationApp.input.mousePosition;
        connection->Render(simulationApp);
    }
}
void Creator::Discard(SimulationApp& simulationApp)
{
    simulationApp.inputOwners.Remove(this);
    body0 = body1 = NULL;
    body = NULL;
    connection = NULL;
    hovered = active = false;
}
void Creator::PreUpdate(SimulationApp& simulationApp)
{
    if (simulationApp.inputOwners.hoveredDepth <= Depth() && IsInside(simulationApp.input.mousePosition, simulationApp.background->GetViewport()))
    {
        simulationApp.inputOwners.hoveredDepth = Depth();
        simulationApp.inputOwners.hovered = this;
    }
}
