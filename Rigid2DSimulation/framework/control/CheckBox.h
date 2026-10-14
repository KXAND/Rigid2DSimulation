#pragma once
#include <functional>

#include "Tile.h"
#include "Control.h"

class CheckBox : public Control
{
public:
    int height = 0;
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
    int GetWidth() const override;
    int GetHeight() const override;

    DVector2 GetBoxTopLeft() const;
    void Render(App& app);
    std::function<bool(App&)> checkedProvider;
    std::function<void(App&, bool)> onCheckedChanged;

    virtual void Sync(App& app);
    virtual void Upload(App& app) const;

    void Update(App& app) override;
    void Discard(App& app) override;
    void PreUpdate(App& app) override;
};
class LocalizedCheckBox : public CheckBox
{
public:
    wstring textId;
    LocalizedCheckBox() = default;
    LocalizedCheckBox(wstring const& id, bool big = false);
    void Update(App& app) override;
};
