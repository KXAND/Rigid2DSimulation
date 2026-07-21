#pragma once
#include "Control.h"

struct HorizontalControlList : Control {
	vector<Control*> cs;
	HorizontalControlList(vector<Control*> const& cs = {});
	int GetW() const override;
	int GetH() const override;

	void Update(App& app) override;
	void Discard(App& app) override;
	void PreUpdate(App& app) override;
};

struct VerticalControlList : Control {
	vector<Control*> cs;
	VerticalControlList(vector<Control*> const& cs = {});
	int GetW() const override;
	int GetH() const override;

	void Update(App& app) override;
	void Discard(App& app) override;
	void PreUpdate(App& app) override;
};

