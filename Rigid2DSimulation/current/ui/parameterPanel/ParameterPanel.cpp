#include "ParameterPanel.h"

#include <memory>
#include <vector>

ParameterPanel::ParameterPanel(App& app) : LocalizedFloatPanel(app, 400, 600, L"parameterPanelTitle")
{
    vector<Control*> controls;
    parameterList = std::make_shared<ParameterControlList>();
    addButton = std::make_shared<ParameterAdditionButton>();
    controls = {&*parameterList, &*addButton};
    this->controls = std::make_shared<VerticalControlList>(controls);
    contentControl = &*this->controls;
    Init(app);
}
