#include "AboutPanel.h"

#include <memory>

AboutPanel::AboutPanel(App& app) : LocalizedFloatPanel(app, 600, 540, L"aboutPanelTitle")
{
    aboutText = std::make_shared<LocalizedRichLabel>(560, L"aboutText");
    contentControl = aboutText.get();
    Init(app);
}
