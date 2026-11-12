#pragma once

#include "ControlContainer.h"
#include "FloatPanel.h"

class ParameterPanel : public LocalizedFloatPanel
{
public:
    ParameterPanel(App& app);

private:
    VerticalStack* parameterList = nullptr;

    void CreateParameterRow(int id);
    void SyncParameterRows(App& app);
    void UpdateContent(App& app) override;
};
