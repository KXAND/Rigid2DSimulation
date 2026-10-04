#pragma once

#include <memory>

#include "ControlList.h"
#include "FloatPanel.h"
#include "ParameterButtons.h"
#include "ParameterControls.h"

struct ParameterPanel : LocalizedFloatPanel
{
    std::shared_ptr<ParameterControlList> parameterList;
    std::shared_ptr<ParameterAdditionButton> addButton;
    std::shared_ptr<VerticalControlList> controls;
    ParameterPanel(App& app);
};
