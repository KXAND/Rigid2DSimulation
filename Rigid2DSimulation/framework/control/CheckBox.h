#pragma once
#include "Tile.h"
#include "Control.h"

struct CheckBox : Control {
	int h = 0;
	DVector2 gap;
	double depth = 0;
	wstring text;
	int boxSize = 0, innerBoxSize = 0;
	Color textColor, frameColor, innerColor, normalColor, invalidColor, hoveredColor;

	int textWidth = 0;
	bool checked = false;
	bool enabled = true;
	bool hovered = false;

	CheckBox(bool big = false);
	int GetW() const override;
	int GetH() const override;

	DVector2 GetBoxTopLeft() const;
	void Render(App& app);
	virtual void Sync(App& app) {}
	virtual void Upload(App& app) const {}

	void Update(App& app) override;
	void Discard(App& app) override;
	void PreUpdate(App& app) override;
};
struct LocalizedCheckBox : CheckBox {
	wstring textId;
	LocalizedCheckBox() = default;
	LocalizedCheckBox(wstring const& id, bool big = false);
	void Update(App& app) override;
};


