#include "SceneBuilders.h"
#include "Body.h"
#include "BodyConfig.h"
#include "Connection.h"
#include "ConnectionConfig.h"
#include "DVector2.h"
#include "PhysicsWorld.h"
#include "Shape.h"
#include "current/common/utils.h"

void CreateBody(PhysicsWorld& physicsWorld, Value const& shapeConfigurations, Value const& configuration)
{
    bool point = false;
    TryReadConfigNumber(configuration, L"point", point);
    std::shared_ptr<Body> body;

    vector<std::shared_ptr<Shape>> shapes;
    for (auto shapeConfig : shapeConfigurations.elements)
    {
        if (shapeConfig->elements.empty())
        {
            continue;
        }
        std::shared_ptr<Shape> shape;
        if (shapeConfig->elements[0]->number)
        {
            if (shapeConfig->elements.size() < 2)
            {
                continue;
            }
            Vector2 relativePosition;
            if (shapeConfig->elements.size() >= 3)
            {
                relativePosition = ToVector2(*shapeConfig->elements[2]);
            }
            shape = std::make_shared<Shape>(shapeConfig->elements[1]->number, relativePosition);
        }
        else
        {
            if (shapeConfig->elements.size() < 2)
            {
                continue;
            }
            vector<Vector2> vertices;
            for (auto vertexValue : shapeConfig->elements[1]->elements)
            {
                vertices.push_back(ToVector2(*vertexValue));
            }
            shape = std::make_shared<Shape>(vertices);
        }
        shapes.push_back(shape);
    }

    if (point)
    {
        body = std::make_shared<Body>(Vector2());
    }
    else
    {
        body = std::make_shared<Body>(shapes);
    }

    body->appearance.innerColor = Color(GenerateRandomInt(256), GenerateRandomInt(256), GenerateRandomInt(256));
    ApplyBodyConfig(*body, configuration);
    bool resetPosition = false;
    TryReadConfigNumber(configuration, L"resetPosition", resetPosition);
    body->Init(resetPosition);
    physicsWorld.bodies.push_back(body);
    physicsWorld.isSceneChanged = true;
}

void CreateConnection(PhysicsWorld& physicsWorld, Value const& configuration)
{
    auto& bodies = physicsWorld.bodies;
    int idx0 = GenerateRandomInt(bodies.size());
    int idx1 = GenerateRandomInt(bodies.size());
    TryReadConfigNumber(configuration, L"index0", idx0);
    TryReadConfigNumber(configuration, L"index1", idx1);
    idx0 = Clamp(idx0, 0, (int)bodies.size());
    idx1 = Clamp(idx1, 0, (int)bodies.size());

    auto connection = std::make_shared<Connection>();
    connection->body0 = &*bodies[idx0];
    connection->body1 = &*bodies[idx1];
    connection->p0Relative = connection->body0->GenerateRandomPointInside();
    connection->p1Relative = connection->body1->GenerateRandomPointInside();

    bool absolute = false;
    TryReadConfigNumber(configuration, L"absolute", absolute);
    if (auto value = FindConfigValue(configuration, L"point0"))
    {
        connection->p0Relative = ToVector2(*value);
        if (absolute)
        {
            connection->p0Relative -= connection->body0->position;
            connection->p0Relative = connection->body0->transform.Inverse() * connection->p0Relative;
        }
    }
    if (auto value = FindConfigValue(configuration, L"point1"))
    {
        connection->p1Relative = ToVector2(*value);
        if (absolute)
        {
            connection->p1Relative -= connection->body1->position;
            connection->p1Relative = connection->body1->transform.Inverse() * connection->p1Relative;
        }
    }

    connection->SignUpToBodies();
    connection->UpdatePosition();
    connection->length = (connection->p0 - connection->p1).GetLength();
    ApplyConnectionConfig(*connection, configuration);
    physicsWorld.connections.push_back(connection);
    physicsWorld.isSceneChanged = true;
}

void Boundary(PhysicsWorld& physicsWorld, DRect canvasRect, Value const& thickness, Value const& configuration)
{
    double canvasWidth = canvasRect.width, canvasHeight = canvasRect.height;
    double left = canvasRect.topLeftPosition.x, right = left + canvasWidth;
    double top = canvasRect.topLeftPosition.y, bottom = top + canvasHeight;
    Color color = Color(GenerateRandomInt(256), GenerateRandomInt(256), GenerateRandomInt(256));

    auto addBoundaryBody = [&physicsWorld, &configuration, color](double width, double height, Vector2 position)
    {
        auto shape = std::make_shared<Shape>(GetRectVerticesByCenter(width, height));
        auto body = std::make_shared<Body>(shape);
        body->position = position;
        body->fixed = true;
        body->appearance.innerColor = color;
        ApplyBodyConfig(*body, configuration);
        body->Init();
        physicsWorld.bodies.push_back(body);
    };

    addBoundaryBody(thickness.number, canvasHeight, Vector2(left, (top + bottom) / 2));
    addBoundaryBody(thickness.number, canvasHeight, Vector2(right, (top + bottom) / 2));
    addBoundaryBody(canvasWidth, thickness.number, Vector2((left + right) / 2, top));
    addBoundaryBody(canvasWidth, thickness.number, Vector2((left + right) / 2, bottom));
    physicsWorld.isSceneChanged = true;
}

void Gear(PhysicsWorld& physicsWorld, Value const& radius, Value const& toothCount, Value const& toothHeight, Value const& configuration)
{
    vector<std::shared_ptr<Shape>> shapes;
    shapes.push_back(std::make_shared<Shape>(radius.number));
    for (int i = (0), iEnd = int(toothCount.number); i < iEnd; ++i)
    {
        double phi0 = 2 * kPi * i / (int)toothCount.number;
        double phi1 = 2 * kPi * (i + 1) / (int)toothCount.number;
        double phi = (phi0 + phi1) / 2;
        Vector2 toothTip = Vector2(cos(phi), sin(phi)) * (radius.number + toothHeight.number);
        Vector2 v0 = Vector2(cos(phi0), sin(phi0)) * radius.number;
        Vector2 v1 = Vector2(cos(phi1), sin(phi1)) * radius.number;
        vector<Vector2> toothVertices{v0, toothTip, v1};
        auto toothShape = std::make_shared<Shape>(toothVertices);
        shapes.push_back(toothShape);
    }

    auto wheel = std::make_shared<Body>(shapes);
    wheel->appearance.innerColor = Color(GenerateRandomInt(256), GenerateRandomInt(256), GenerateRandomInt(256));
    ApplyBodyConfig(*wheel, configuration);
    wheel->Init();
    physicsWorld.bodies.push_back(wheel);

    auto root = std::make_shared<Body>(wheel->position);
    root->appearance.innerColor = wheel->appearance.innerColor;
    root->Init();
    physicsWorld.bodies.push_back(root);

    auto connection = std::make_shared<Connection>();
    connection->type = kLinkConnection;
    connection->body0 = &*root;
    connection->body1 = &*wheel;
    connection->SignUpToBodies();
    connection->UpdatePosition();
    physicsWorld.connections.push_back(connection);
    physicsWorld.isSceneChanged = true;
}

void Strand(PhysicsWorld& physicsWorld, Value const& point0Value, Value const& point1Value,
            Value const& countValue, Value const& ratio, Value const& bodyConfiguration, Value const& connectionConfiguration)
{
    Vector2 p0 = ToVector2(point0Value), p1 = ToVector2(point1Value);
    int bodyCount = countValue.number;
    std::shared_ptr<Body> previousBody, currentBody;
    std::shared_ptr<Connection> connection;
    double segmentLength = (p1 - p0).GetLength() / bodyCount;
    double bodyRadius = 0.5 * ratio.number * segmentLength;
    double gap = max(0.0, segmentLength - 2 * bodyRadius);
    Color bodyColor = Color(GenerateRandomInt(256), GenerateRandomInt(256), GenerateRandomInt(256));

    auto firstShape = std::make_shared<Shape>(bodyRadius);
    previousBody = std::make_shared<Body>(firstShape);
    previousBody->appearance.innerColor = bodyColor;
    previousBody->position = p0;
    ApplyBodyConfig(*previousBody, bodyConfiguration);
    previousBody->Init();
    physicsWorld.bodies.push_back(previousBody);

    for (int i = (1), iEnd = int(bodyCount + 1); i < iEnd; ++i)
    {
        auto shape = std::make_shared<Shape>(bodyRadius);
        currentBody = std::make_shared<Body>(shape);
        currentBody->appearance.innerColor = bodyColor;
        currentBody->position = (p0 * (bodyCount - i) + p1 * i) / bodyCount;
        ApplyBodyConfig(*currentBody, bodyConfiguration);
        currentBody->Init();
        physicsWorld.bodies.push_back(currentBody);

        connection = std::make_shared<Connection>();
        connection->body0 = &*previousBody;
        connection->body1 = &*currentBody;
        connection->length = gap;
        connection->p0Relative = +bodyRadius * (p1 - p0).GetNormalized();
        connection->p1Relative = -bodyRadius * (p1 - p0).GetNormalized();
        ApplyConnectionConfig(*connection, connectionConfiguration);
        connection->SignUpToBodies();
        connection->UpdatePosition();
        physicsWorld.connections.push_back(connection);
        previousBody = currentBody;
    }
    physicsWorld.isSceneChanged = true;
}

void Necklace(PhysicsWorld& physicsWorld, Value const& centerValue, Value const& radius,
              Value const& countValue, Value const& ratio, Value const& bodyConfiguration, Value const& connectionConfiguration)
{
    std::shared_ptr<Body> firstBody, previousBody, currentBody;
    int bodyCount = countValue.number;
    Vector2 center = ToVector2(centerValue);
    std::shared_ptr<Connection> connection;
    double segmentLength = 2 * kPi * radius.number / bodyCount;
    double ballRadius = 0.5 * ratio.number * segmentLength;
    double gap = max(0.0, segmentLength - 2 * ballRadius);
    Color bodyColor = Color(GenerateRandomInt(256), GenerateRandomInt(256), GenerateRandomInt(256));

    auto firstShape = std::make_shared<Shape>(ballRadius);
    previousBody = std::make_shared<Body>(firstShape);
    previousBody->appearance.innerColor = bodyColor;
    previousBody->position = center + Vector2(radius.number, 0);
    ApplyBodyConfig(*previousBody, bodyConfiguration);
    previousBody->Init();
    physicsWorld.bodies.push_back(previousBody);
    firstBody = previousBody;

    for (int i = (1), iEnd = int(bodyCount); i < iEnd; ++i)
    {
        double phi = 2 * kPi * i / bodyCount;
        auto shape = std::make_shared<Shape>(ballRadius);
        currentBody = std::make_shared<Body>(shape);
        currentBody->appearance.innerColor = bodyColor;
        currentBody->position = center + Vector2(cos(phi), sin(phi)) * radius.number;
        ApplyBodyConfig(*currentBody, bodyConfiguration);
        currentBody->Init();
        physicsWorld.bodies.push_back(currentBody);

        connection = std::make_shared<Connection>();
        connection->body0 = &*previousBody;
        connection->body1 = &*currentBody;
        connection->length = gap;
        connection->p1Relative = ballRadius * Vector2(+sin(phi), -cos(phi));
        phi = 2 * kPi * (i - 1) / bodyCount;
        connection->p0Relative = ballRadius * Vector2(-sin(phi), +cos(phi));
        ApplyConnectionConfig(*connection, connectionConfiguration);
        connection->SignUpToBodies();
        connection->UpdatePosition();
        physicsWorld.connections.push_back(connection);
        previousBody = currentBody;
    }

    double phi = 2 * kPi * bodyCount / bodyCount;
    currentBody = firstBody;
    connection = std::make_shared<Connection>();
    connection->body0 = &*previousBody;
    connection->body1 = &*currentBody;
    connection->length = gap;
    connection->p1Relative = ballRadius * Vector2(+sin(phi), -cos(phi));
    phi = 2 * kPi * (bodyCount - 1) / bodyCount;
    connection->p0Relative = ballRadius * Vector2(-sin(phi), +cos(phi));
    ApplyConnectionConfig(*connection, connectionConfiguration);
    connection->SignUpToBodies();
    connection->UpdatePosition();
    physicsWorld.connections.push_back(connection);
    physicsWorld.isSceneChanged = true;
}
