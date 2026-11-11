#include "AboutPanel.h"

#include <memory>

AboutPanel::AboutPanel(App& app) : LocalizedFloatPanel(app, 600, 540, L"aboutPanelTitle")
{
    layout = std::make_unique<VerticalControlList>();
    layout->AddLocalizedRichLabel(560, L"aboutText");
    contentControl = layout.get();
    Init(app);
}
