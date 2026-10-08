#pragma once
#include "Vector2.h"

class Body;

enum class DragMethod
{
    Point,
    Whole,
    Force
};

// 当前阶段的非拥有数据；body 为空表示没有拖拽作用，不跨对象删除边界保存。
struct BodyDrag
{
    Body const* body = nullptr;
    Vector2 relativePoint;
    double radian = 0;
    double velocityAngular = 0;
    DragMethod method = DragMethod::Point;
    Vector2 targetPosition;
    Vector2 targetDelta;
};

// 保留现有每帧修正规则：普通/整体执行，力拖拽不执行。
void ApplyBodyDragFrame(Body& body, BodyDrag const& drag, double realDeltaTime);
// 普通/整体使用 realDeltaTime 推导速度，力拖拽使用 stepDeltaTime 计算冲量。
void ApplyBodyDragSubstep(Body& body, BodyDrag const& drag, double realDeltaTime, double stepDeltaTime);
