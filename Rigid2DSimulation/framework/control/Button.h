#pragma once
#include "Font.h"
#include "Control.h"

struct Button : Control {
	DVector2 gap;
	double depth = 0;
	wstring txt;
	int w = 0, h = 0;
	Color c_txt, c_frame, c_normal, c_chosen, hoveredColor, c_invalid;

	bool chosen = false;
	bool enabled = true;
	bool hovered = false;

	Button(int w = 0);
	int GetW() const override;
	int GetH() const override;

	void render(App& app);
	virtual void OnClick(App& app) {}

	void Update(App& app) override;
	void Discard(App& app) override;
	void PreUpdate(App& app) override;
};
struct LocalizedButton : Button {
	wstring id_txt;
	LocalizedButton() = default;
	LocalizedButton(int w, wstring const& id);
	void Update(App& app) override;
};

struct FloatPanel;
struct ShowFloatPanelButton : Button {
	FloatPanel* fpn = NULL;
	ShowFloatPanelButton() = default;
	ShowFloatPanelButton(FloatPanel* fpn);
	void OnClick(App& app) override;
	void Update(App& app) override;
};


