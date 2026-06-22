#pragma once
#include "font.h"
#include "control.h"

typedef function<wstring(int, App&)> DpNm;
wstring dpnm_none(int id, App &app);

struct Droplist : Control {
	dVector2 gap;
	double dep = 0;
	wstring txt;
	int w = 0, h = 0;

	// ��ʹĳЩ����� picked ����ȡ -1��Ĭ��ֵ������� 0 �ɡ�
	// ���� picked ��ȡʲôֵ��û������ģ��ܲ���ȡ -1 ֮�࣬Ҫʵ���еõ���
	int picked = 0;
	int h_item = 0;
	int n_item = 0;
	DpNm nm = dpnm_none;

	double sign_r = 0;
	int sign_margin = 0;
	int x_txt_margin = 0;
	dColor c_txt, c_mark, c_edit, c_frame, c_normal, c_invalid, hoveredColor;

	// -1 ����û���������κ�ѡ�����档
	int item_hv = -1;
	int w_txt = 0;
	bool edit = false;
	bool enabled = true;
	bool hovered = false;

	Droplist(int w = 0, DpNm const &nm = dpnm_none);
	int GetW() const override;
	int GetH() const override;

	dVector2 tl_box() const;
	dVector2 tl_item() const;
	void render_main(App& app);
	void render_items(App& app);
	void render(App& app);
	virtual void Sync(App& app) {}
	// ����Ĭ�ϵ��� Upload ��У�����ı�ؼ�����ֵ��ֻ�ı�Ŀ��ֵ��
	virtual void Upload(App& app) const {}

	void Update(App& app) override;
	void Discard(App& app) override;
	void PreUpdate(App& app) override;
};
struct DpLan : Droplist {
	wstring id_txt;
	DpLan() = default;
	DpLan(wstring const& id, int w, DpNm const& nm = dpnm_none);
	void Update(App& app) override;
};

