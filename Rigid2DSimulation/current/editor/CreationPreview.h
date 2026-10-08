#pragma once
#include "Vector2.h"

class Body;
class Connection;

// 当帧借用；工具更新、取消或场景删除后必须重新获取。
struct CreationPreview
{
    Body const* body = nullptr;
    Connection const* connection = nullptr;
    Vector2 relativeAnchor;
    Vector2 targetPosition;
};
