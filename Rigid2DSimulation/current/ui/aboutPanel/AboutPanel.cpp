#include "AboutPanel.h"

#include <memory>
#include <utility>

#include "App.h"

AboutPanel::AboutPanel(App& app) : LocalizedFloatPanel(app.screen.Rect(), 600, 540, L"aboutPanelTitle")
{
    auto layout = std::make_unique<VerticalStack>();
    layout->AddLocalizedRichLabel(560, L"aboutText");
    SetContent(std::move(layout));
}
