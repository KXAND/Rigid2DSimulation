#pragma once
#include "Tile.h"

struct App;
struct Control;
struct VerticalScrollBar;
struct FloatPanel {
	DVector2 tl;
	double depth = 0;
	Control* contentControl = NULL;
	wstring txt;
	int w = 0, h = 0;
	double wheelMultiplier = 0;
	DVector2 contentOffset;
	DRect contentViewport;

	bool useScrollBar = false;
	std::shared_ptr<VerticalScrollBar> scrollBar;

	int h_bar = 0;
	int s_close = 0;
	int half_s_mark = 0;
	int margin_close = 0;
	int margin_title = 0;
	Color c_txt, c_mark, frameColor, panelColor;
	Color c_bar_normal, c_bar_dragged, c_bar_hovered;
	Color c_close_normal, c_close_chosen, c_close_hovered, c_close_invalid;

	int visibleHeight = 0;
	double t_click = 0;
	bool hasOverflow = false;
	bool shown = false;
	bool hovered = false;
	bool wheeled = false;
	bool hovered_bar = false;
	bool dragged_bar = false;
	bool chosen_close = false;
	bool hovered_close = false;
	bool enabled_close = true;

	FloatPanel();
	FloatPanel(App& app, int _w, int _h);

	int GetMinY() const;
	DRect GetVisibleViewport(App& app) const;
	DVector2 GetCloseTopLeft() const;
	void SetControl(App& app, Control* _c);
	void SetTopLeft(App& app, DVector2 const& p);
	void InitializeControl(App& app);
	void InitializeScrollBar();

	void Hide(App& app);
	void Show(App& app);
	void HandleCaption(App& app);
	void HandleWheelAndClampContent(App& app);

	void RenderMain(App& app);
	void RenderClose(App& app);
	void Render(App& app);

	void Init(App& app);
	virtual void Update(App& app);
	virtual void Discard(App& app);
	virtual void PreUpdate(App& app);
};
struct LocalizedFloatPanel : FloatPanel {
	wstring id_txt;
	LocalizedFloatPanel() = default;
	LocalizedFloatPanel(App& app, int w, int h, wstring const& id);
	void Update(App& app) override;
};

bool CompareFloatPanels(FloatPanel* const fp0, FloatPanel* const fp1);


