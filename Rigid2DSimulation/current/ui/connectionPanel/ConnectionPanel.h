#pragma once

#include <memory>

#include "ConnectionControls.h"
#include "ControlList.h"
#include "FloatPanel.h"

struct ConnectionPanel : LocalizedFloatPanel
{
    std::shared_ptr<ConnectionEditorControls> editorControls;
    std::shared_ptr<ConnectionDetailsLabel> detailsLabel;
    std::shared_ptr<VerticalControlList> controls;
    ConnectionPanel(App& app);
};
