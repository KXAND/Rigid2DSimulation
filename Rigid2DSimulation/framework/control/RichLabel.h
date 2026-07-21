#pragma once
#include "Font.h"
#include "Control.h"

struct RichLabel : Control {
	DVector2 gap;
	double depth = 0;
	int w = 0, h = 0;
	bool fixedWidth = false;
	bool fixedHeight = false;

	Color textColor;
	wstring text;

	RichLabel(int w = 0);
	int GetW() const override;
	int GetH() const override;

	int GetLineWidth() const;
	void Render(App& app);
	virtual void Sync(App& app) {}
	void Update(App& app);
};
struct LocalizedRichLabel : RichLabel {
	wstring textId;
	LocalizedRichLabel() = default;
	LocalizedRichLabel(int w, wstring const& id);
	void Sync(App& app) override;
};


