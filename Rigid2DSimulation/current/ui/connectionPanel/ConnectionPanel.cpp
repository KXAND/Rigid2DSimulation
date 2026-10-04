#include "ConnectionPanel.h"

#include <memory>
#include <vector>

ConnectionPanel::ConnectionPanel(App& app) : LocalizedFloatPanel(app, 400, 600, L"connectionPanelTitle")
{
    vector<Control*> controls;
    editorControls = std::make_shared<ConnectionEditorControls>();
    detailsLabel = std::make_shared<ConnectionDetailsLabel>();
    controls = {&*editorControls, &*detailsLabel};
    this->controls = std::make_shared<VerticalControlList>(controls);
    contentControl = &*this->controls;
    Init(app);
}
