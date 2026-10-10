#pragma once

#include <memory>
#include <vector>

#include "ControlList.h"
#include "CreationButtons.h"
#include "CreationControls.h"
#include "FloatPanel.h"

class CreationPanel : public LocalizedFloatPanel
{
public:
    explicit CreationPanel(App& app);

private:
    std::vector<std::shared_ptr<CreationToolButton>> toolButtons;
    std::shared_ptr<CreationSizeTextBox> radiusTextBox;
    std::shared_ptr<CreationSizeTextBox> thicknessTextBox;
    std::shared_ptr<CreationHelpLabel> helpLabel;
    std::shared_ptr<VerticalControlList> controls;
};
