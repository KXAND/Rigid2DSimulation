#pragma once
#include "Font.h"
#include "Control.h"

struct Button : Control {
	DVector2 gap;
	double depth = 0;
	wstring text;
	int w = 0, h = 0;
	Color textColor, frameColor, normalColor, chosenColor, hoveredColor, invalidColor;

	bool chosen = false;
	bool enabled = true;
	bool hovered = false;

	Button(int w = 0);
	int GetW() const override;
	int GetH() const override;

	void Render(App& app);
	virtual void OnClick(App& app) {}

	void Update(App& app) override;
	void Discard(App& app) override;
	void PreUpdate(App& app) override;
};
struct LocalizedButton : Button {
	wstring textId;
	LocalizedButton() = default;
	LocalizedButton(int w, wstring const& id);
	void Update(App& app) override;
};

struct FloatPanel;
struct ShowFloatPanelButton : Button {
	FloatPanel* floatPanel = NULL;
	ShowFloatPanelButton() = default;
	ShowFloatPanelButton(FloatPanel* floatPanel);
	void OnClick(App& app) override;
	void Update(App& app) override;
};


