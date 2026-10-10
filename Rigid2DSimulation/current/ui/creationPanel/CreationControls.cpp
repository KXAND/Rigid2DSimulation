#include "CreationControls.h"

#include <cerrno>
#include <cmath>
#include <cwchar>
#include <cwctype>

#include "Creator.h"
#include "SimulationApp.h"

CreationSizeTextBox::CreationSizeTextBox(CreationSizeField field)
    : LocalizedTextBox(field == CreationSizeField::ParticleRadius ? L"creationParticleRadius" : L"creationPlateThickness", 100),
      field(field)
{
}

void CreationSizeTextBox::Update(App& app)
{
    LocalizedTextBox::Update(app);
    if (edit && app.input.IsKeyPressedDown(VK_RETURN))
    {
        OnDone(app);
        Discard(app);
    }
}

void CreationSizeTextBox::Sync(App& app)
{
    auto& simulationApp = static_cast<SimulationApp&>(app);
    auto const& config = simulationApp.editor.GetRequestedCreatorConfig();
    value = FormatFixed2(field == CreationSizeField::ParticleRadius ? config.radius : config.thickness);
}

void CreationSizeTextBox::OnDone(App& app) const
{
    wchar_t* end = nullptr;
    errno = 0;
    double size = std::wcstod(value.c_str(), &end);
    if (end == value.c_str() || errno == ERANGE || !std::isfinite(size) || size <= 0)
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
    if (field == CreationSizeField::ParticleRadius)
    {
        config.radius = size;
    }
    else
    {
        config.thickness = size;
    }
    simulationApp.editor.RequestCreatorConfig(config);
}

CreationHelpLabel::CreationHelpLabel() : RichLabel(340)
{
}

void CreationHelpLabel::Sync(App& app)
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
    text = app.GetLocalizedString(L"creatorMode") + app.GetLocalizedString(nameId) + L"\n";
    if (simulationApp.editor.mode != kCreateMode)
    {
        text += app.GetLocalizedString(L"creationChooseTool") + L"\n";
    }
    text += app.GetLocalizedString(helpId) + L"\n\n" + app.GetLocalizedString(L"creationSizeHelp");
}
