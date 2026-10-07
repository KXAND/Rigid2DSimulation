#pragma once
#include <functional>

#include "DVector2.h"

struct Body;
struct Connection;
/// <summary>
/// Group 是一系列通过 connection 连接的图形
/// </summary>
struct Group
{
    vector<Body*> bodies;
    Group(vector<Body*> const& bodies) : bodies(bodies) {}
    /// <summary>
    /// @todo
    /// </summary>
    /// <param name="rectangle"></param>
    // 每次越界位移后同步通知，不保存回调。
    void Warp(DRect rectangle, Body const* warpExcludedBody,
              std::function<void(Body const&)> const& onBodyWarped);
};

void FindBodiesInGroup(vector<Body*>& bodies, Body* body);
/// <summary>
/// 将输入的 body 划分为 group
/// </summary>
/// <param name="bodies"></param>
/// <returns></returns>
vector<std::shared_ptr<Group>> FormGroups(vector<std::shared_ptr<Body>> const& bodies);
