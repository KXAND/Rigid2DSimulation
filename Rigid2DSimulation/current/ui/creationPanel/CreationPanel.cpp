#include "CreationPanel.h"

#include <cerrno>
#include <cmath>
#include <cwchar>
#include <cwctype>
#include <memory>

#include "Creator.h"
#include "SimulationApp.h"

CreationPanel::CreationPanel(App& app) : LocalizedFloatPanel(app, 400, 680, L"creationPanelTitle")
{
    BuildToolButtons();

    radiusTextBox = CreateSizeInput(SizeField::ParticleRadius);
    thicknessTextBox = CreateSizeInput(SizeField::PlateThickness);
    helpLabel = std::make_shared<RichLabel>(340);
    helpLabel->textProvider = GetHelpText;

    layout = std::make_shared<VerticalControlList>();
    for (auto const& button : toolButtons)
    {
        layout->controls.push_back(button.get());
    }
    layout->controls.push_back(radiusTextBox.get());
    layout->controls.push_back(thicknessTextBox.get());
    layout->controls.push_back(helpLabel.get());
    contentControl = layout.get();
    Init(app);
}

void CreationPanel::BuildToolButtons()
{
    AddToolButton(kCreateBox, L"creationBoxTool");
    AddToolButton(kCreateBall, L"creationBallTool");
    AddToolButton(kCreatePlate, L"creationPlateTool");
    AddToolButton(kCreateParticle, L"creationParticleTool");
    AddToolButton(kCreateConnection, L"creationConnectionTool");
    AddToolButton(kCreatePoint, L"creationPointTool");
    AddToolButton(kCreateNail, L"creationNailTool");
}

void CreationPanel::AddToolButton(int mode, wchar_t const* textId)
{
    auto button = std::make_shared<LocalizedButton>(340, textId);
    button->onClick = [mode](App& app)
    {
        auto& simulationApp = static_cast<SimulationApp&>(app);
        simulationApp.editor.SelectCreationTool(mode);
    };
    toolButtons.push_back(button);
}

std::shared_ptr<LocalizedTextBox> CreationPanel::CreateSizeInput(SizeField field)
{
    auto textBox = std::make_shared<LocalizedTextBox>(
        field == SizeField::ParticleRadius ? L"creationParticleRadius" : L"creationPlateThickness", 100);
    textBox->submitOnEnter = true;
    textBox->valueProvider = [field](App& app)
    {
        auto const& config = static_cast<SimulationApp&>(app).editor.GetRequestedCreatorConfig();
        return FormatFixed2(field == SizeField::ParticleRadius ? config.radius : config.thickness);
    };
    textBox->onEditFinished = [field](App& app, std::wstring const& text)
    {
        ApplySize(app, field, text);
    };
    return textBox;
}

void CreationPanel::ApplySize(App& app, SizeField field, std::wstring const& text)
{
    wchar_t* end = nullptr;
    errno = 0;
    double size = std::wcstod(text.c_str(), &end);
    if (end == text.c_str() || errno == ERANGE || !std::isfinite(size) || size <= 0)
    {
        return;
    }
    while (std::iswspace(*end))
    {
        ++end;
    }
    if (*end != L'\0')
    {
        return;
    }

    auto& simulationApp = static_cast<SimulationApp&>(app);
    auto config = simulationApp.editor.GetRequestedCreatorConfig();
    if (field == SizeField::ParticleRadius)
    {
        config.radius = size;
    }
    else
    {
        config.thickness = size;
    }
    simulationApp.editor.RequestCreatorConfig(config);
}

std::wstring CreationPanel::GetHelpText(App& app)
{
    auto& simulationApp = static_cast<SimulationApp&>(app);
    auto const& config = simulationApp.editor.GetRequestedCreatorConfig();
    wchar_t const* nameId = L"creationUnknownTool";
    wchar_t const* helpId = L"creationChooseTool";
    switch (config.mode)
    {
    case kCreateBox:
        nameId = L"creationBoxTool";
        helpId = L"creationBoxHelp";
        break;
    case kCreateBall:
        nameId = L"creationBallTool";
        helpId = L"creationBallHelp";
        break;
    case kCreatePlate:
        nameId = L"creationPlateTool";
        helpId = L"creationPlateHelp";
        break;
    case kCreateParticle:
        nameId = L"creationParticleTool";
        helpId = L"creationParticleHelp";
        break;
    case kCreateConnection:
        nameId = L"creationConnectionTool";
        helpId = L"creationConnectionHelp";
        break;
    case kCreatePoint:
        nameId = L"creationPointTool";
        helpId = L"creationPointHelp";
        break;
    case kCreateNail:
        nameId = L"creationNailTool";
        helpId = L"creationNailHelp";
        break;
    }
    std::wstring text = app.GetLocalizedString(L"creatorMode") + app.GetLocalizedString(nameId) + L"\n";
    if (simulationApp.editor.mode != kCreateMode)
    {
        text += app.GetLocalizedString(L"creationChooseTool") + L"\n";
    }
    text += app.GetLocalizedString(helpId) + L"\n\n" + app.GetLocalizedString(L"creationSizeHelp");
    return text;
}
