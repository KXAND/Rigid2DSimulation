#pragma once

#include <string>

#include "ControlContainer.h"
#include "FloatPanel.h"

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

    void BuildToolButtons(VerticalStack& layout);
    void AddSizeInput(VerticalStack& layout, SizeField field);
    static void ApplySize(App& app, SizeField field, std::wstring const& text);
    void AddToolButton(VerticalStack& layout, int mode, wchar_t const* textId);
    static std::wstring GetHelpText(App& app);
};
