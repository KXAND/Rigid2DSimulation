#pragma once

#include <memory>
#include <vector>

#include "Button.h"
#include "ControlList.h"
#include "DragBar.h"
#include "FloatPanel.h"
#include "Separator.h"
#include "TextBox.h"

class ParameterPanel : public LocalizedFloatPanel
{
public:
    struct ParameterRow
    {
        std::shared_ptr<TextBox> nameTextBox;
        std::shared_ptr<LocalizedButton> deleteButton;
        std::shared_ptr<HorizontalControlList> nameRow;
        std::shared_ptr<HorizontalDragBar> valueDragBar;
        std::shared_ptr<TextBox> valueTextBox;
        std::shared_ptr<HorizontalControlList> valueRow;
        std::shared_ptr<HorizontalSeparator> separator;
        std::shared_ptr<VerticalControlList> layout;
    };

    std::vector<ParameterRow> parameterRows;
    std::shared_ptr<VerticalControlList> parameterList;
    std::shared_ptr<LocalizedButton> addButton;
    std::shared_ptr<VerticalControlList> layout;

    ParameterPanel(App& app);

private:
    static ParameterRow CreateParameterRow(int id);
    void SyncParameterRows(App& app);
    void UpdateContent(App& app) override;
};
