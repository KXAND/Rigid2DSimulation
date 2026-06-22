#pragma once
#include "tile.h"

struct App;
struct Control;
struct ScrollbarY;
struct Panel {
	dVector2 tl;
	double dep = 0;
	Control* c = NULL;
	int w = 0, h = 0;
	double wheel_mtp = 0;
	dVector2 tl_show_rel;
	dRect vp_show_rel;
	dColor c_frame, c_panel;

	bool use_sb = false;
	// ��Ҳ�����ΪɶҪ��ָ�롣�е���Եĸо��ˡ�
	ptr<ScrollbarY> sb;

	int h_show = 0;
	bool full = false;
	bool hovered = false;
	bool wheeled = false;

	Panel();
	static Panel menu(App& app);
	static Panel bottom(App& app);

	int min_y() const;
	dRect vp_show(App& app) const;
	// ���� delegate ����ר�ŵ� set_c ��Ҫ��ϣ�������ù�����
	void set_c(App& app, Control* _c);
	void init_c(App& app);
	void init_sb();
	void render(App& app);

	void Init(App& app);
	virtual void Update(App& app);
	// ����п�ɾ���� Panel ���������
	virtual void Discard(App& app);
	virtual void PreUpdate(App& app);
};

