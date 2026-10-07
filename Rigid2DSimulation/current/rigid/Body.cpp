#include "Body.h"
#include "framework/basic/Utils.h"
#include "Shape.h"
#include "Connection.h"

Body::Body()
{
    AssignProperties(BodyProperties{});
}

Body::Body(std::shared_ptr<Shape> shape, BodyProperties const& properties)
    : Body(vector<std::shared_ptr<Shape>>{shape}, properties)
{
}

Body::Body(vector<std::shared_ptr<Shape>> const& shapes, BodyProperties const& properties)
    : shapes(shapes)
{
    ApplyProperties(properties);
}

std::shared_ptr<Body> Body::CreateAtShapeCenter(vector<std::shared_ptr<Shape>> const& shapes,
                                              BodyProperties const& properties)
{
    auto body = std::shared_ptr<Body>(new Body());
    body->shapes = shapes;
    body->AssignProperties(properties);
    body->position = body->Recalculate();
    body->UpdateShapesPositionAndAABB();
    return body;
}

std::shared_ptr<Body> Body::CreatePoint(Vector2 const& position)
{
    BodyProperties properties;
    properties.position = position;
    return CreatePoint(properties);
}

std::shared_ptr<Body> Body::CreatePoint(BodyProperties const& properties)
{
    auto body = std::shared_ptr<Body>(new Body());
    body->point = true;
    body->AssignProperties(properties);
    return body;
}

BodyProperties Body::GetProperties() const
{
    BodyProperties properties;
    properties.position = position;
    properties.velocity = velocity;
    properties.radian = radian;
    properties.velocityAngular = velocityAngular;
    properties.fixed = fixed;
    properties.density = density;
    properties.elasticity = elasticity;
    properties.frictionStatic = frictionStatic;
    properties.frictionDynamic = frictionDynamic;
    properties.dampCoeff = dampCoeff;
    properties.dampCoeffAngular = dampCoeffAngular;
    properties.chargeDensity = chargeDensity;
    properties.innerColor = appearance.innerColor;
    return properties;
}

void Body::AssignProperties(BodyProperties const& properties)
{
    position = properties.position;
    velocity = properties.velocity;
    radian = properties.radian;
    velocityAngular = properties.velocityAngular;
    fixed = properties.fixed;
    density = properties.density;
    elasticity = properties.elasticity;
    frictionStatic = properties.frictionStatic;
    frictionDynamic = properties.frictionDynamic;
    dampCoeff = properties.dampCoeff;
    dampCoeffAngular = properties.dampCoeffAngular;
    chargeDensity = properties.chargeDensity;
    appearance.innerColor = properties.innerColor;
}

void Body::ApplyProperties(BodyProperties const& properties)
{
    AssignProperties(properties);
    if (point)
    {
        return;
    }

    Recalculate();
    UpdateShapesPositionAndAABB();
}

double Body::GetDepth() const
{
    return point ? appearance.depthPoint : appearance.depthShape;
}
bool Body::IsInside(Vector2 p) const
{
    if (point)
    {
        return (p - position).GetLengthSquared() < appearance.pointRadius * appearance.pointRadius;
    }

    for (auto shape : shapes)
    {
        if (shape->IsInside(p))
            return true;
    }
    return false;
}


Vector2 Body::GenerateRandomPointInside() const
{
    if (point)
    {
        return {};
    }

    double randomArea = GenerateRandomFloat(area);
    for (const auto& shape : shapes)
    {
        randomArea -= shape->area;
        if (randomArea < 0)
        {
            return shape->GenerateRandomPointInside();
        }
    }
    return shapes.back()->GenerateRandomPointInside();
}

void Body::UpdateShapesPositionAndAABB()
{
    transform = Matrix2::Rotation(radian);
    for (auto& shape : shapes)
    {
        shape->position = position + transform * shape->relativePosition;
        shape->radian = radian;
        for (int i = (0), iEnd = int(shape->vertices.size()); i < iEnd; ++i)
        {
            shape->vertices[i] = position + transform * shape->verticesRelative[i];
        }
    }
    UpdateAABB();
}

void Body::UpdateAABB()
{
    // @todo：考虑设计一个expand函数
    box = Aabb();
    for (const auto& shape : shapes)
    {
        box = Aabb(box, shape->GetBoundingBox());
    }
}

// 这个方法已经过时了。
void Body::Warp(DRect rectangle, bool isWarpBlocked)
{
    if (invMass == 0 || isWarpBlocked)
    {
        return;
    }

    if (position.x < rectangle.GetLeft())
    {
        position.x += rectangle.width;
    }
    else if (position.x > rectangle.GetRight())
    {
        position.x -= rectangle.width;
    }
    if (position.y < rectangle.GetTop())
    {
        position.y += rectangle.height;
    }
    else if (position.y > rectangle.GetBottom())
    {
        position.y -= rectangle.height;
    }
}

Vector2 Body::Recalculate()
{
    // 刷新仍完整执行旧计算，包括重复调用时的局部几何处理。
    area = 0;
    Vector2 center;
    inertia = 0;
    for (auto shape : shapes)
    {
        shape->area = 0;
        Vector2 shapeCenter;
        if (shape->isCircle)
        {
            shape->area = kPi * shape->radius * shape->radius;
            shapeCenter = shape->area * shape->relativePosition;
            inertia += shape->area * (shape->radius * shape->radius) / 2;
        }
        else
        {
            for (int i = (0), iEnd = int(shape->verticesRelative.size()); i < iEnd; ++i)
            {
                int j = (i + 1) % shape->verticesRelative.size();
                auto p = shape->verticesRelative[i], q = shape->verticesRelative[j];
                double triangleArea = Matrix2(p, q).Determinant() / 2;
                double triangleInertia = Dot(p, p) + Dot(p, q) + Dot(q, q);
                triangleInertia *= triangleArea / 6;

                shapeCenter += triangleArea * (p + q) / 3;
                shape->area += triangleArea;
                inertia += triangleInertia;
            }
        }

        area += shape->area;
        center += shapeCenter;
        shape->relativePosition = shapeCenter / shape->area;
    }

    center /= area;
    charge = chargeDensity * area;
    invMass = fixed ? 0 : 1 / (density * area);
    invInertia = fixed ? 0 : 1 / (density * inertia);
    for (auto shape : shapes)
    {
        shape->relativePosition -= center;
        for (auto& v : shape->verticesRelative)
        {
            v -= center;
        }
    }
    return center;
}

void Body::Step(Vector2 const& gravity, double stepDeltaTime, std::function<void(bool)> const& applyPresets)
{
    // 预设可能修改物体属性，但本次推进仍沿用调用前选定的运动分支。
    if (invMass)
    {
        velocity -= dampCoeff * velocity * stepDeltaTime;
        velocity += gravity * stepDeltaTime;
        velocityAngular -= dampCoeffAngular * velocityAngular * stepDeltaTime;
        applyPresets(true);
        position += velocity * stepDeltaTime;
        radian += velocityAngular * stepDeltaTime;
    }
    else
    {
        velocity = {};
        velocityAngular = 0;
        applyPresets(false);
    }
}






void Electrostatic(Body& b0, Body& b1, double sdt, double coulombConstant)
{
    double dsqr = (b1.position - b0.position).GetLengthSquared();
    Vector2 d = (b1.position - b0.position).GetNormalized();
    Vector2 je = -b0.charge * b1.charge * d / dsqr * sdt * coulombConstant;
    b0.velocity += je * b0.invMass;
    b1.velocity -= je * b1.invMass;
}
