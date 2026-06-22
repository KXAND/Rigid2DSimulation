#pragma once
#include "tile.h"

struct App;
struct Control;
// �������ĸ߶ȳ����˷�Χ��Scrollbar �����Զ�������
// ��һȨ������ Panel �ȶ����ֱ���ϼ���
// Scrollbar ���ǿؼ������̳� Control �ࡣ
struct ScrollbarY {
	dVector2 tl;
	double dep = 0;
	Control* c = NULL;
	int w = 0, h = 0;
	int h_show = 0, top_show = 0;
	dColor c_bar, c_frame, c_normal, draggedColor, hoveredColor, c_invalid;

	int ny_rel = 0, nh = 0;
	bool full = false;
	bool dragged = false;
	bool enabled = true;
	bool hovered = false;
	bool hovered_node = false;

	ScrollbarY();
	// û�� set_c ���� Discard ֮ǰ�Ķ���
	void render(App& app);

	virtual void Update(App& app);
	virtual void Discard(App& app);
	virtual void PreUpdate(App& app);
};

