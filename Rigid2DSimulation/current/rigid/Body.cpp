#include "Body.h"
#include "current/common/utils.h"
#include "Shape.h"
#include "Connection.h"

// 这些构造函数真的很混乱。
Body::Body()
{
    density = 1;
    dampCoeff = 0.1;
    dampCoeffAngular = 0.1;
    elasticity = 0.7;
    frictionStatic = 0.5;
    frictionDynamic = 0.3;

    depthShape = 0;
    depthPoint = 1;
    pointRadius = 8;
    borderColor = {255, 255, 255};
    hoveredColor = {0, 0, 255};
    draggedColor = {255, 0, 0};
    selectedColor = {255, 0, 255};
    selectedConnectionColor = {255, 255, 0};
}
Body::Body(Vector2 const& position) : Body()
{
    point = true;
    this->position = position;
}
Body::Body(std::shared_ptr<Shape> shape) : Body()
{
    shapes = {shape};
}
Body::Body(vector<std::shared_ptr<Shape>> const& shapes) : Body()
{
    this->shapes = shapes;
}
void Body::Save(FILE* file) const
{
    int size = 0;
    fwrite(&(point), sizeof(point), 1, file);
    fwrite(&(fixed), sizeof(fixed), 1, file);
    size = shapes.size();
    fwrite(&(size), sizeof(size), 1, file);
    for (auto shape : shapes)
    {
        shape->Save(file);
    }
    fwrite(&(position), sizeof(position), 1, file);
    fwrite(&(velocity), sizeof(velocity), 1, file);
    size = (scriptText).size();
    fwrite(&(size), sizeof(size), 1, file);
    fwrite(&*(scriptText).begin(), sizeof(*(&*(scriptText).begin())), size, file);
    fwrite(&(radian), sizeof(radian), 1, file);
    fwrite(&(velocityAngular), sizeof(velocityAngular), 1, file);
    fwrite(&(density), sizeof(density), 1, file);
    fwrite(&(elasticity), sizeof(elasticity), 1, file);
    fwrite(&(frictionStatic), sizeof(frictionStatic), 1, file);
    fwrite(&(frictionDynamic), sizeof(frictionDynamic), 1, file);
    fwrite(&(dampCoeff), sizeof(dampCoeff), 1, file);
    fwrite(&(dampCoeffAngular), sizeof(dampCoeffAngular), 1, file);
    fwrite(&(chargeDensity), sizeof(chargeDensity), 1, file);
    fwrite(&(innerColor), sizeof(innerColor), 1, file);
}
Body::Body(FILE* file) : Body()
{
    int size = 0;
    fread(&(point), sizeof(point), 1, file);
    fread(&(fixed), sizeof(fixed), 1, file);
    fread(&(size), sizeof(size), 1, file);
    for (int i = (0), iEnd = int(size); i < iEnd; ++i)
    {
        shapes.push_back(std::make_shared<Shape>(file));
    }
    fread(&(position), sizeof(position), 1, file);
    fread(&(velocity), sizeof(velocity), 1, file);
    fread(&(size), sizeof(size), 1, file);
    (scriptText).resize(size);
    fread(&*(scriptText).begin(), sizeof(*(&*(scriptText).begin())), size, file);
    temporaryScriptText = scriptText;
    fread(&(radian), sizeof(radian), 1, file);
    fread(&(velocityAngular), sizeof(velocityAngular), 1, file);
    fread(&(density), sizeof(density), 1, file);
    fread(&(elasticity), sizeof(elasticity), 1, file);
    fread(&(frictionStatic), sizeof(frictionStatic), 1, file);
    fread(&(frictionDynamic), sizeof(frictionDynamic), 1, file);
    fread(&(dampCoeff), sizeof(dampCoeff), 1, file);
    fread(&(dampCoeffAngular), sizeof(dampCoeffAngular), 1, file);
    fread(&(chargeDensity), sizeof(chargeDensity), 1, file);
    fread(&(innerColor), sizeof(innerColor), 1, file);

}

double Body::GetDepth() const
{
    return point ? depthPoint : depthShape;
}
bool Body::IsInside(Vector2 p) const
{
    if (point)
    {
        return (p - position).GetLengthSquared() < pointRadius * pointRadius;
    }

    for (auto shape : shapes)
    {
        if (shape->IsInside(p))
            return true;
    }
    return false;
}


void Body::ReadConfig(Value const& config)
{
    if (auto value = FindConfigValue(config, L"position"))
    {
        position = ToVector2(*value);
    }
    if (auto value = FindConfigValue(config, L"velocity"))
    {
        velocity = ToVector2(*value);
    }
    if (auto value = FindConfigValue(config, L"innerColor"))
    {
        innerColor = (Color)ToVector3(*value);
    }
    TryReadConfigNumber(config, L"radian", radian);
    TryReadConfigNumber(config, L"velocityAngular", velocityAngular);
    TryReadConfigNumber(config, L"elasticity", elasticity);
    TryReadConfigNumber(config, L"frictionStatic", frictionStatic);
    TryReadConfigNumber(config, L"frictionDynamic", frictionDynamic);
    TryReadConfigNumber(config, L"dampCoeff", dampCoeff);
    TryReadConfigNumber(config, L"dampCoeffAngular", dampCoeffAngular);
    TryReadConfigNumber(config, L"fixed", fixed);
    TryReadConfigNumber(config, L"density", density);
    TryReadConfigNumber(config, L"chargeDensity", chargeDensity);
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
void Body::Warp(DRect rectangle)
{
    if (invMass == 0 || dragged)
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

void Body::Init(bool resetPosition)
{
    // 这里假设是之前没有乱改成员。
    if (point)
    {
        return;
    }

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
    if (resetPosition)
    {
        position = center;
    }
    for (auto shape : shapes)
    {
        shape->relativePosition -= center;
        for (auto& v : shape->verticesRelative)
        {
            v -= center;
        }
    }
    UpdateShapesPositionAndAABB();
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
