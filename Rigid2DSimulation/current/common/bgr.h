#pragma once
#include "vector3.h"
#include "tile.h"

struct Cur;
/// <summary>
/// Background ��װ��һ��tile�����е��˶�����Ⱦ������ background ֮�Ͻ��е�
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
	/// @todo:���ú����ĵ��ô�update���ƶ�����cur�У�ִ�е�˳����ȻԶ������render����֮ǰ�������д���ȷ
	/// @todo:����ȥ�������ν�Ĵ���
	/// </summary>
	/// <param name="cur"></param>
	void Render(Cur& cur);
	/// <summary>
	/// ����owner���
	/// </summary>
	/// <param name="cur"></param>
	void PreUpdate(Cur& cur);
	/// <summary>
	/// ����Ƿ����˱���
	/// </summary>
	/// <param name="cur"></param>
	void Update(Cur& cur);
};
