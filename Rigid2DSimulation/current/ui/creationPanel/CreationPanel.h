#pragma once

#include <memory>
#include <string>
#include <vector>

#include "Button.h"
#include "ControlList.h"
#include "FloatPanel.h"
#include "RichLabel.h"
#include "TextBox.h"

class CreationPanel : public LocalizedFloatPanel
{
public:
    explicit CreationPanel(App& app);

private:
    enum class SizeField
    {
        ParticleRadius,
        PlateThickness
    };

    std::vector<std::shared_ptr<LocalizedButton>> toolButtons;
    std::shared_ptr<LocalizedTextBox> radiusTextBox;
    std::shared_ptr<LocalizedTextBox> thicknessTextBox;
    std::shared_ptr<RichLabel> helpLabel;
    std::shared_ptr<VerticalControlList> layout;

    void BuildToolButtons();
    std::shared_ptr<LocalizedTextBox> CreateSizeInput(SizeField field);
    static void ApplySize(App& app, SizeField field, std::wstring const& text);
    void AddToolButton(int mode, wchar_t const* textId);
    static std::wstring GetHelpText(App& app);
};
