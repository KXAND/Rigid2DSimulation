#pragma once
#include "Control.h"

class HorizontalControlList : public Control
{
public:
    vector<Control*> controls;
    HorizontalControlList(vector<Control*> const& controls = {});
    int GetWidth() const override;
    int GetHeight() const override;

    void Update(App& app) override;
    void Discard(App& app) override;
    void PreUpdate(App& app) override;
};

class VerticalControlList : public Control
{
public:
    vector<Control*> controls;
    VerticalControlList(vector<Control*> const& controls = {});
    int GetWidth() const override;
    int GetHeight() const override;

    void Update(App& app) override;
    void Discard(App& app) override;
    void PreUpdate(App& app) override;
};
