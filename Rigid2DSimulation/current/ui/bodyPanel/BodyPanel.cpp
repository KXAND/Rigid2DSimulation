#include "BodyPanel.h"

#include <memory>
#include <vector>

BodyPanel ::BodyPanel(App& app) : LocalizedFloatPanel(app, 400, 600, L"bodyPanelTitle")
{
    vector<Control*> controls;
    editorControls = std::make_shared<BodyEditorControls>();
    detailsLabel = std::make_shared<BodyDetailsLabel>();
    controls = {&*editorControls, &*detailsLabel};
    this->controls = std::make_shared<VerticalControlList>(controls);
    contentControl = &*this->controls;
    Init(app);
}
