#pragma once

#include <memory>

#include "ControlList.h"
#include "FloatPanel.h"

class ParameterPanel : public LocalizedFloatPanel
{
public:
    ParameterPanel(App& app);

private:
    std::unique_ptr<VerticalControlList> layout;
    VerticalControlList* parameterList = nullptr;

    void CreateParameterRow(int id);
    void SyncParameterRows(App& app);
    void UpdateContent(App& app) override;
};
