#pragma once

#include <memory>

#include "BodyControls.h"
#include "ControlList.h"
#include "FloatPanel.h"

class BodyPanel : public LocalizedFloatPanel
{
public:
    std::shared_ptr<BodyEditorControls> editorControls;
    std::shared_ptr<BodyDetailsLabel> detailsLabel;
    std::shared_ptr<VerticalControlList> controls;
    BodyPanel(App& app);
};
