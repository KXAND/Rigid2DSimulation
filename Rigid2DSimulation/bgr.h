#pragma once
#include "vector3.h"
#include "tile.h"

struct Cur;
/// <summary>
/// Background 封装了一个tile，所有的运动和渲染都是在 background 之上进行的
/// </summary>
struct Background
{
	dVector2 topLeft;
	double depth = 0;
	tile backgroundTile;
	int width = 0, height = 0;

	bool hovered = false;
	bool wheeled = false;

	Background(Cur& cur);
	dRect viewPort() const { return { topLeft, width, height }; }
	
	/// <summary>
	/// @todo:将该函数的调用从update中移动到了cur中，执行的顺序依然远在其他render函数之前，理由有待明确
	/// @todo:考虑去掉这个无谓的传参
	/// </summary>
	/// <param name="cur"></param>
	void Render(Cur& cur);
	/// <summary>
	/// 更新深度
	/// </summary>
	/// <param name="cur"></param>
	void PreUpdate(Cur& cur);
	/// <summary>
	/// 检查是否点击了背景
	/// </summary>
	/// <param name="cur"></param>
	void Update(Cur& cur);
};