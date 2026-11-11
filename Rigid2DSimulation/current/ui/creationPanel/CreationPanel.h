#pragma once

#include <memory>
#include <string>

#include "ControlList.h"
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

    std::unique_ptr<VerticalControlList> layout;

    void BuildToolButtons();
    void AddSizeInput(SizeField field);
    static void ApplySize(App& app, SizeField field, std::wstring const& text);
    void AddToolButton(int mode, wchar_t const* textId);
    static std::wstring GetHelpText(App& app);
};
