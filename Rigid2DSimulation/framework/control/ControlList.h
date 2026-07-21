#pragma once
#include "Control.h"

struct HorizontalControlList : Control {
	vector<Control*> controls;
	HorizontalControlList(vector<Control*> const& controls = {});
	int GetW() const override;
	int GetH() const override;

	void Update(App& app) override;
	void Discard(App& app) override;
	void PreUpdate(App& app) override;
};

struct VerticalControlList : Control {
	vector<Control*> controls;
	VerticalControlList(vector<Control*> const& controls = {});
	int GetW() const override;
	int GetH() const override;

	void Update(App& app) override;
	void Discard(App& app) override;
	void PreUpdate(App& app) override;
};

