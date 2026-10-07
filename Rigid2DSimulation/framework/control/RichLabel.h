#pragma once
#include "Font.h"
#include "Control.h"

class RichLabel : public Control
{
public:
    DVector2 gap;
    double depth = 0;
    int width = 0, height = 0;
    bool fixedWidth = false;
    bool fixedHeight = false;

    Color textColor;
    wstring text;

    RichLabel(int width = 0);
    int GetWidth() const override;
    int GetHeight() const override;

    int GetLineWidth() const;
    void Render(App& app);
    virtual void Sync(App& app) {}
    void Update(App& app);
};
class LocalizedRichLabel : public RichLabel
{
public:
    wstring textId;
    LocalizedRichLabel() = default;
    LocalizedRichLabel(int width, wstring const& id);
    void Sync(App& app) override;
};
