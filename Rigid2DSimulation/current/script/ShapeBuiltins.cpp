#include "ShapeBuiltins.h"

#include <memory>
#include <vector>

#include "Value.h"
#include "Vector2.h"

namespace
{
    std::shared_ptr<Value> MakeVertexValue(double x, double y)
    {
        std::vector<std::shared_ptr<Value>> coordinates;
        coordinates.push_back(std::make_shared<Value>(x));
        coordinates.push_back(std::make_shared<Value>(y));
        return std::make_shared<Value>(coordinates);
    }

    std::shared_ptr<Value> MakePolygonConfig(std::vector<std::shared_ptr<Value>> const& vertices)
    {
        std::vector<std::shared_ptr<Value>> shapeConfig;
        shapeConfig.push_back(std::make_shared<Value>(0));
        shapeConfig.push_back(std::make_shared<Value>(vertices));
        return std::make_shared<Value>(shapeConfig);
    }
}

shared_ptr<Value> Ball(Value const& radius)
{
    vector<shared_ptr<Value>> shapeConfigs;
    vector<shared_ptr<Value>> shapeConfig;
    shapeConfig.push_back(make_shared<Value>(1));
    shapeConfig.push_back(make_shared<Value>(radius));
    shapeConfigs.push_back(make_shared<Value>(shapeConfig));
    return make_shared<Value>(shapeConfigs);
}

shared_ptr<Value> Box(Value const& width, Value const& height)
{
    vector<std::shared_ptr<Value>> shapeConfigs;

    vector<std::shared_ptr<Value>> vertices;
    vertices.push_back(MakeVertexValue(+width.number / 2, +height.number / 2));
    vertices.push_back(MakeVertexValue(-width.number / 2, +height.number / 2));
    vertices.push_back(MakeVertexValue(-width.number / 2, -height.number / 2));
    vertices.push_back(MakeVertexValue(+width.number / 2, -height.number / 2));

    shapeConfigs.push_back(MakePolygonConfig(vertices));
    return std::make_shared<Value>(shapeConfigs);
}

std::shared_ptr<Value> RegularPolygon(Value const& sideCount, Value const& radius)
{
    vector<std::shared_ptr<Value>> shapeConfigs;

    vector<std::shared_ptr<Value>> vertices;
    auto polygonSideCount = max(3, (int)sideCount.number);
    for (int i = (0), iEnd = int(polygonSideCount); i < iEnd; ++i)
    {
        double phi = 2 * kPi * i / polygonSideCount;
        vertices.push_back(MakeVertexValue(radius.number * cos(phi), radius.number * sin(phi)));
    }

    shapeConfigs.push_back(MakePolygonConfig(vertices));
    return std::make_shared<Value>(shapeConfigs);
}

std::shared_ptr<Value> Parallelogram(Value const& width, Value const& height, Value const& shear)
{
    vector<std::shared_ptr<Value>> shapeConfigs;

    vector<std::shared_ptr<Value>> vertices;
    vertices.push_back(MakeVertexValue(+width.number / 2 + shear.number, +height.number / 2));
    vertices.push_back(MakeVertexValue(-width.number / 2 + shear.number, +height.number / 2));
    vertices.push_back(MakeVertexValue(-width.number / 2 - shear.number, -height.number / 2));
    vertices.push_back(MakeVertexValue(+width.number / 2 - shear.number, -height.number / 2));

    shapeConfigs.push_back(MakePolygonConfig(vertices));
    return std::make_shared<Value>(shapeConfigs);
}

std::shared_ptr<Value> Trapezoid(Value const& topWidth, Value const& bottomWidth, Value const& height)
{
    vector<std::shared_ptr<Value>> shapeConfigs;

    vector<std::shared_ptr<Value>> vertices;
    vertices.push_back(MakeVertexValue(+topWidth.number / 2, +height.number / 2));
    vertices.push_back(MakeVertexValue(-topWidth.number / 2, +height.number / 2));
    vertices.push_back(MakeVertexValue(-bottomWidth.number / 2, -height.number / 2));
    vertices.push_back(MakeVertexValue(+bottomWidth.number / 2, -height.number / 2));

    shapeConfigs.push_back(MakePolygonConfig(vertices));
    return std::make_shared<Value>(shapeConfigs);
}

std::shared_ptr<Value> Cross(Value const& size, Value const& thickness)
{
    vector<std::shared_ptr<Value>> shapeConfigs;

    vector<std::shared_ptr<Value>> vertices;
    vertices.push_back(MakeVertexValue(+size.number / 2, +thickness.number / 2));
    vertices.push_back(MakeVertexValue(-size.number / 2, +thickness.number / 2));
    vertices.push_back(MakeVertexValue(-size.number / 2, -thickness.number / 2));
    vertices.push_back(MakeVertexValue(+size.number / 2, -thickness.number / 2));

    shapeConfigs.push_back(MakePolygonConfig(vertices));

    vertices.clear();
    vertices.push_back(MakeVertexValue(+thickness.number / 2, +size.number / 2));
    vertices.push_back(MakeVertexValue(-thickness.number / 2, +size.number / 2));
    vertices.push_back(MakeVertexValue(-thickness.number / 2, -size.number / 2));
    vertices.push_back(MakeVertexValue(+thickness.number / 2, -size.number / 2));

    shapeConfigs.push_back(MakePolygonConfig(vertices));
    return std::make_shared<Value>(shapeConfigs);
}

std::shared_ptr<Value> FramedBox(Value const& width, Value const& height, Value const& thickness)
{
    Vector2 topOuterRight(+width.number / 2 + thickness.number / 2, +height.number / 2 + thickness.number / 2);
    Vector2 topOuterLeft(-width.number / 2 - thickness.number / 2, +height.number / 2 + thickness.number / 2);
    Vector2 topInnerLeft(-width.number / 2 - thickness.number / 2, +height.number / 2 - thickness.number / 2);
    Vector2 topInnerRight(+width.number / 2 + thickness.number / 2, +height.number / 2 - thickness.number / 2);
    Vector2 bottomInnerRight(+width.number / 2 + thickness.number / 2, -height.number / 2 + thickness.number / 2);
    Vector2 bottomInnerLeft(-width.number / 2 - thickness.number / 2, -height.number / 2 + thickness.number / 2);
    Vector2 bottomOuterLeft(-width.number / 2 - thickness.number / 2, -height.number / 2 - thickness.number / 2);
    Vector2 bottomOuterRight(+width.number / 2 + thickness.number / 2, -height.number / 2 - thickness.number / 2);
    Vector2 rightOuterTop(+width.number / 2 + thickness.number / 2, +height.number / 2 + thickness.number / 2);
    Vector2 rightOuterBottom(+width.number / 2 + thickness.number / 2, -height.number / 2 - thickness.number / 2);
    Vector2 rightInnerBottom(+width.number / 2 - thickness.number / 2, -height.number / 2 - thickness.number / 2);
    Vector2 rightInnerTop(+width.number / 2 - thickness.number / 2, +height.number / 2 + thickness.number / 2);
    Vector2 leftInnerTop(-width.number / 2 + thickness.number / 2, +height.number / 2 + thickness.number / 2);
    Vector2 leftInnerBottom(-width.number / 2 + thickness.number / 2, -height.number / 2 - thickness.number / 2);
    Vector2 leftOuterBottom(-width.number / 2 - thickness.number / 2, -height.number / 2 - thickness.number / 2);
    Vector2 leftOuterTop(-width.number / 2 - thickness.number / 2, +height.number / 2 + thickness.number / 2);

    vector<std::shared_ptr<Value>> shapeConfigs;

    vector<std::shared_ptr<Value>> vertices;
    vertices.push_back(MakeVertexValue(topOuterRight.x, topOuterRight.y));
    vertices.push_back(MakeVertexValue(topOuterLeft.x, topOuterLeft.y));
    vertices.push_back(MakeVertexValue(topInnerLeft.x, topInnerLeft.y));
    vertices.push_back(MakeVertexValue(topInnerRight.x, topInnerRight.y));

    shapeConfigs.push_back(MakePolygonConfig(vertices));

    vertices.clear();
    vertices.push_back(MakeVertexValue(bottomInnerRight.x, bottomInnerRight.y));
    vertices.push_back(MakeVertexValue(bottomInnerLeft.x, bottomInnerLeft.y));
    vertices.push_back(MakeVertexValue(bottomOuterLeft.x, bottomOuterLeft.y));
    vertices.push_back(MakeVertexValue(bottomOuterRight.x, bottomOuterRight.y));

    shapeConfigs.push_back(MakePolygonConfig(vertices));

    vertices.clear();
    vertices.push_back(MakeVertexValue(rightInnerTop.x, rightInnerTop.y));
    vertices.push_back(MakeVertexValue(rightInnerBottom.x, rightInnerBottom.y));
    vertices.push_back(MakeVertexValue(rightOuterBottom.x, rightOuterBottom.y));
    vertices.push_back(MakeVertexValue(rightOuterTop.x, rightOuterTop.y));

    shapeConfigs.push_back(MakePolygonConfig(vertices));

    vertices.clear();
    vertices.push_back(MakeVertexValue(leftOuterTop.x, leftOuterTop.y));
    vertices.push_back(MakeVertexValue(leftOuterBottom.x, leftOuterBottom.y));
    vertices.push_back(MakeVertexValue(leftInnerBottom.x, leftInnerBottom.y));
    vertices.push_back(MakeVertexValue(leftInnerTop.x, leftInnerTop.y));

    shapeConfigs.push_back(MakePolygonConfig(vertices));
    return std::make_shared<Value>(shapeConfigs);
}
