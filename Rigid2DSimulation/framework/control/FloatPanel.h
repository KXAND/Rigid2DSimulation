#pragma once
#include "Tile.h"

struct App;
struct Control;
struct VerticalScrollBar;
struct FloatPanel
{
    DVector2 topLeft;
    double depth = 0;
    Control* contentControl = NULL;
    wstring title;
    int width = 0, height = 0;
    double wheelMultiplier = 0;
    DVector2 contentOffset;
    DRect contentViewport;

    bool useScrollBar = false;
    std::shared_ptr<VerticalScrollBar> scrollBar;

    int titleBarHeight = 0;
    int closeButtonSize = 0;
    int closeMarkHalfSize = 0;
    int closeButtonMargin = 0;
    int titleMargin = 0;
    Color titleColor, closeMarkColor, frameColor, panelColor;
    Color titleBarNormalColor, titleBarDraggedColor, titleBarHoveredColor;
    Color closeButtonNormalColor, closeButtonChosenColor, closeButtonHoveredColor, closeButtonInvalidColor;

    int visibleHeight = 0;
    double lastClickTime = 0;
    bool hasOverflow = false;
    bool shown = false;
    bool hovered = false;
    bool wheeled = false;
    bool titleBarHovered = false;
    bool titleBarDragged = false;
    bool closeButtonChosen = false;
    bool closeButtonHovered = false;
    bool closeButtonEnabled = true;

    FloatPanel();
    FloatPanel(App& app, int width, int height);

    int GetMinY() const;
    DRect GetVisibleViewport(App& app) const;
    DVector2 GetCloseTopLeft() const;
    void SetControl(App& app, Control* newContentControl);
    void SetTopLeft(App& app, DVector2 const& topLeft);
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
struct LocalizedFloatPanel : FloatPanel
{
    wstring titleLocalizationId;
    LocalizedFloatPanel() = default;
    LocalizedFloatPanel(App& app, int width, int height, wstring const& id);
    void Update(App& app) override;
};

bool CompareFloatPanels(FloatPanel* const firstPanel, FloatPanel* const secondPanel);
