#include "ShapeBuiltins.h"
#include "Value.h"
#include "Shape.h"

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
	vector<std::shared_ptr<Value>> shapeConfig;
	shapeConfig.push_back(std::make_shared<Value>(0));
	vector<std::shared_ptr<Value>> tmp;
	tmp.push_back(std::make_shared<Value>(vector<std::shared_ptr<Value>>{ std::make_shared<Value>(+width.number / 2), std::make_shared<Value>(+height.number / 2) }));
	tmp.push_back(std::make_shared<Value>(vector<std::shared_ptr<Value>>{ std::make_shared<Value>(-width.number / 2), std::make_shared<Value>(+height.number / 2) }));
	tmp.push_back(std::make_shared<Value>(vector<std::shared_ptr<Value>>{ std::make_shared<Value>(-width.number / 2), std::make_shared<Value>(-height.number / 2) }));
	tmp.push_back(std::make_shared<Value>(vector<std::shared_ptr<Value>>{ std::make_shared<Value>(+width.number / 2), std::make_shared<Value>(-height.number / 2) }));
	shapeConfig.push_back(std::make_shared<Value>(tmp));
	shapeConfigs.push_back(std::make_shared<Value>(shapeConfig));
	return std::make_shared<Value>(shapeConfigs);
}
std::shared_ptr<Value> RegularPolygon(Value const& sideCount, Value const& radius)
{
	vector<std::shared_ptr<Value>> shapeConfigs;
	vector<std::shared_ptr<Value>> shapeConfig;
	shapeConfig.push_back(std::make_shared<Value>(0));
	vector<std::shared_ptr<Value>> tmp;
	auto polygonSideCount = max(3, (int)sideCount.number);
	for (int i = (0), iEnd = int(polygonSideCount); i < iEnd; ++i)
	{
		double phi = 2 * kPi * i / polygonSideCount;
		tmp.push_back(std::make_shared<Value>(vector<std::shared_ptr<Value>>{ std::make_shared<Value>(radius.number * cos(phi)), std::make_shared<Value>(radius.number * sin(phi)) }));
	}
	shapeConfig.push_back(std::make_shared<Value>(tmp));
	shapeConfigs.push_back(std::make_shared<Value>(shapeConfig));
	return std::make_shared<Value>(shapeConfigs);
}
std::shared_ptr<Value> Parallelogram(Value const& width, Value const& height, Value const& shear)
{
	vector<std::shared_ptr<Value>> shapeConfigs;
	vector<std::shared_ptr<Value>> shapeConfig;
	shapeConfig.push_back(std::make_shared<Value>(0));
	vector<std::shared_ptr<Value>> tmp;
	tmp.push_back(std::make_shared<Value>(vector<std::shared_ptr<Value>>{ std::make_shared<Value>(+width.number / 2 + shear.number), std::make_shared<Value>(+height.number / 2) }));
	tmp.push_back(std::make_shared<Value>(vector<std::shared_ptr<Value>>{ std::make_shared<Value>(-width.number / 2 + shear.number), std::make_shared<Value>(+height.number / 2) }));
	tmp.push_back(std::make_shared<Value>(vector<std::shared_ptr<Value>>{ std::make_shared<Value>(-width.number / 2 - shear.number), std::make_shared<Value>(-height.number / 2) }));
	tmp.push_back(std::make_shared<Value>(vector<std::shared_ptr<Value>>{ std::make_shared<Value>(+width.number / 2 - shear.number), std::make_shared<Value>(-height.number / 2) }));
	shapeConfig.push_back(std::make_shared<Value>(tmp));
	shapeConfigs.push_back(std::make_shared<Value>(shapeConfig));
	return std::make_shared<Value>(shapeConfigs);
}
std::shared_ptr<Value> Trapezoid(Value const& topWidth, Value const& bottomWidth, Value const& height)
{
	vector<std::shared_ptr<Value>> shapeConfigs;
	vector<std::shared_ptr<Value>> shapeConfig;
	shapeConfig.push_back(std::make_shared<Value>(0));
	vector<std::shared_ptr<Value>> tmp;
	tmp.push_back(std::make_shared<Value>(vector<std::shared_ptr<Value>>{ std::make_shared<Value>(+topWidth.number / 2), std::make_shared<Value>(+height.number / 2) }));
	tmp.push_back(std::make_shared<Value>(vector<std::shared_ptr<Value>>{ std::make_shared<Value>(-topWidth.number / 2), std::make_shared<Value>(+height.number / 2) }));
	tmp.push_back(std::make_shared<Value>(vector<std::shared_ptr<Value>>{ std::make_shared<Value>(-bottomWidth.number / 2), std::make_shared<Value>(-height.number / 2) }));
	tmp.push_back(std::make_shared<Value>(vector<std::shared_ptr<Value>>{ std::make_shared<Value>(+bottomWidth.number / 2), std::make_shared<Value>(-height.number / 2) }));
	shapeConfig.push_back(std::make_shared<Value>(tmp));
	shapeConfigs.push_back(std::make_shared<Value>(shapeConfig));
	return std::make_shared<Value>(shapeConfigs);
}
std::shared_ptr<Value> Cross(Value const& size, Value const& thickness)
{
	vector<std::shared_ptr<Value>> shapeConfigs;
	vector<std::shared_ptr<Value>> shapeConfig;
	shapeConfig.push_back(std::make_shared<Value>(0));
	vector<std::shared_ptr<Value>> tmp;
	tmp.push_back(std::make_shared<Value>(vector<std::shared_ptr<Value>>{ std::make_shared<Value>(+size.number / 2), std::make_shared<Value>(+thickness.number / 2) }));
	tmp.push_back(std::make_shared<Value>(vector<std::shared_ptr<Value>>{ std::make_shared<Value>(-size.number / 2), std::make_shared<Value>(+thickness.number / 2) }));
	tmp.push_back(std::make_shared<Value>(vector<std::shared_ptr<Value>>{ std::make_shared<Value>(-size.number / 2), std::make_shared<Value>(-thickness.number / 2) }));
	tmp.push_back(std::make_shared<Value>(vector<std::shared_ptr<Value>>{ std::make_shared<Value>(+size.number / 2), std::make_shared<Value>(-thickness.number / 2) }));
	shapeConfig.push_back(std::make_shared<Value>(tmp));
	shapeConfigs.push_back(std::make_shared<Value>(shapeConfig));
	shapeConfig.clear();
	shapeConfig.push_back(std::make_shared<Value>(0));
	tmp.clear();
	tmp.push_back(std::make_shared<Value>(vector<std::shared_ptr<Value>>{ std::make_shared<Value>(+thickness.number / 2), std::make_shared<Value>(+size.number / 2) }));
	tmp.push_back(std::make_shared<Value>(vector<std::shared_ptr<Value>>{ std::make_shared<Value>(-thickness.number / 2), std::make_shared<Value>(+size.number / 2) }));
	tmp.push_back(std::make_shared<Value>(vector<std::shared_ptr<Value>>{ std::make_shared<Value>(-thickness.number / 2), std::make_shared<Value>(-size.number / 2) }));
	tmp.push_back(std::make_shared<Value>(vector<std::shared_ptr<Value>>{ std::make_shared<Value>(+thickness.number / 2), std::make_shared<Value>(-size.number / 2) }));
	shapeConfig.push_back(std::make_shared<Value>(tmp));
	shapeConfigs.push_back(std::make_shared<Value>(shapeConfig));
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
	vector<std::shared_ptr<Value>> shapeConfig;
	shapeConfig.push_back(std::make_shared<Value>(0));
	vector<std::shared_ptr<Value>> tmp;
	tmp.push_back(std::make_shared<Value>(vector<std::shared_ptr<Value>>{ std::make_shared<Value>(topOuterRight.x), std::make_shared<Value>(topOuterRight.y) }));
	tmp.push_back(std::make_shared<Value>(vector<std::shared_ptr<Value>>{ std::make_shared<Value>(topOuterLeft.x), std::make_shared<Value>(topOuterLeft.y) }));
	tmp.push_back(std::make_shared<Value>(vector<std::shared_ptr<Value>>{ std::make_shared<Value>(topInnerLeft.x), std::make_shared<Value>(topInnerLeft.y) }));
	tmp.push_back(std::make_shared<Value>(vector<std::shared_ptr<Value>>{ std::make_shared<Value>(topInnerRight.x), std::make_shared<Value>(topInnerRight.y) }));
	shapeConfig.push_back(std::make_shared<Value>(tmp));
	shapeConfigs.push_back(std::make_shared<Value>(shapeConfig));
	shapeConfig.clear();
	shapeConfig.push_back(std::make_shared<Value>(0));
	tmp.clear();
	tmp.push_back(std::make_shared<Value>(vector<std::shared_ptr<Value>>{ std::make_shared<Value>(bottomInnerRight.x), std::make_shared<Value>(bottomInnerRight.y) }));
	tmp.push_back(std::make_shared<Value>(vector<std::shared_ptr<Value>>{ std::make_shared<Value>(bottomInnerLeft.x), std::make_shared<Value>(bottomInnerLeft.y) }));
	tmp.push_back(std::make_shared<Value>(vector<std::shared_ptr<Value>>{ std::make_shared<Value>(bottomOuterLeft.x), std::make_shared<Value>(bottomOuterLeft.y) }));
	tmp.push_back(std::make_shared<Value>(vector<std::shared_ptr<Value>>{ std::make_shared<Value>(bottomOuterRight.x), std::make_shared<Value>(bottomOuterRight.y) }));
	shapeConfig.push_back(std::make_shared<Value>(tmp));
	shapeConfigs.push_back(std::make_shared<Value>(shapeConfig));
	shapeConfig.clear();
	shapeConfig.push_back(std::make_shared<Value>(0));
	tmp.clear();
	tmp.push_back(std::make_shared<Value>(vector<std::shared_ptr<Value>>{ std::make_shared<Value>(rightInnerTop.x), std::make_shared<Value>(rightInnerTop.y) }));
	tmp.push_back(std::make_shared<Value>(vector<std::shared_ptr<Value>>{ std::make_shared<Value>(rightInnerBottom.x), std::make_shared<Value>(rightInnerBottom.y) }));
	tmp.push_back(std::make_shared<Value>(vector<std::shared_ptr<Value>>{ std::make_shared<Value>(rightOuterBottom.x), std::make_shared<Value>(rightOuterBottom.y) }));
	tmp.push_back(std::make_shared<Value>(vector<std::shared_ptr<Value>>{ std::make_shared<Value>(rightOuterTop.x), std::make_shared<Value>(rightOuterTop.y) }));
	shapeConfig.push_back(std::make_shared<Value>(tmp));
	shapeConfigs.push_back(std::make_shared<Value>(shapeConfig));
	shapeConfig.clear();
	shapeConfig.push_back(std::make_shared<Value>(0));
	tmp.clear();
	tmp.push_back(std::make_shared<Value>(vector<std::shared_ptr<Value>>{ std::make_shared<Value>(leftOuterTop.x), std::make_shared<Value>(leftOuterTop.y) }));
	tmp.push_back(std::make_shared<Value>(vector<std::shared_ptr<Value>>{ std::make_shared<Value>(leftOuterBottom.x), std::make_shared<Value>(leftOuterBottom.y) }));
	tmp.push_back(std::make_shared<Value>(vector<std::shared_ptr<Value>>{ std::make_shared<Value>(leftInnerBottom.x), std::make_shared<Value>(leftInnerBottom.y) }));
	tmp.push_back(std::make_shared<Value>(vector<std::shared_ptr<Value>>{ std::make_shared<Value>(leftInnerTop.x), std::make_shared<Value>(leftInnerTop.y) }));
	shapeConfig.push_back(std::make_shared<Value>(tmp));
	shapeConfigs.push_back(std::make_shared<Value>(shapeConfig));
	return std::make_shared<Value>(shapeConfigs);
}