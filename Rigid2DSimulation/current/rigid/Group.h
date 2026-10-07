#pragma once
#include <functional>

#include "DVector2.h"

struct Body;

/// <summary>
/// Group 是一系列通过 connection 连接的图形
/// </summary>
struct Group
{
    vector<Body*> bodies;
    Group(vector<Body*> const& bodies) : bodies(bodies) {}

    // 每次越界位移后同步通知，不保存回调。
    void Warp(DRect rectangle, Body const* warpExcludedBody,
              std::function<void(Body const&)> const& onBodyWarped);
};

/// <summary>
/// 将输入的 body 划分为 group
/// </summary>
vector<std::shared_ptr<Group>> FormGroups(vector<std::shared_ptr<Body>> const& bodies);
