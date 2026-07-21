#pragma once
#include "Tile.h"

struct App;
struct Control;
struct VerticalScrollBar;
struct Panel {
	DVector2 tl;
	double depth = 0;
	Control* c = NULL;
	int w = 0, h = 0;
	double wheel_mtp = 0;
	DVector2 tl_show_rel;
	DRect vp_show_rel;
	Color c_frame, c_panel;

	bool use_sb = false;
	// 我也不清楚为啥要用指针。有点惯性的感觉了。
	std::shared_ptr<VerticalScrollBar> sb;

	int h_show = 0;
	bool full = false;
	bool hovered = false;
	bool wheeled = false;

	Panel();
	static Panel CreateMenuPanel(App& app);
	static Panel CreateBottomPanel(App& app);

	int GetMinY() const;
	DRect GetVisibleViewport(App& app) const;
	// 不用 delegate 而用专门的 SetControl 主要是希望能重置滚动。
	void SetControl(App& app, Control* _c);
	void InitializeControl(App& app);
	void InitializeScrollBar();
	void Render(App& app);

	void Init(App& app);
	virtual void Update(App& app);
	// 如果有可删除的 Panel 就用这个。
	virtual void Discard(App& app);
	virtual void PreUpdate(App& app);
};


