#include "CreationPanel.h"

#include "Creator.h"
#include "SimulationApp.h"

CreationPanel::CreationPanel(App& app) : LocalizedFloatPanel(app, 400, 680, L"creationPanelTitle")
{
    auto addToolButton = [this](int mode, wchar_t const* textId)
    {
        auto button = std::make_shared<LocalizedButton>(340, textId);
        button->onClick = [mode](App& app)
        {
            auto& simulationApp = static_cast<SimulationApp&>(app);
            simulationApp.editor.SelectCreationTool(mode);
        };
        toolButtons.push_back(button);
    };
    addToolButton(kCreateBox, L"creationBoxTool");
    addToolButton(kCreateBall, L"creationBallTool");
    addToolButton(kCreatePlate, L"creationPlateTool");
    addToolButton(kCreateParticle, L"creationParticleTool");
    addToolButton(kCreateConnection, L"creationConnectionTool");
    addToolButton(kCreatePoint, L"creationPointTool");
    addToolButton(kCreateNail, L"creationNailTool");
    radiusTextBox = std::make_shared<CreationSizeTextBox>(CreationSizeField::ParticleRadius);
    thicknessTextBox = std::make_shared<CreationSizeTextBox>(CreationSizeField::PlateThickness);
    helpLabel = std::make_shared<CreationHelpLabel>();

    controls = std::make_shared<VerticalControlList>();
    for (auto const& button : toolButtons)
    {
        controls->controls.push_back(button.get());
    }
    controls->controls.push_back(radiusTextBox.get());
    controls->controls.push_back(thicknessTextBox.get());
    controls->controls.push_back(helpLabel.get());
    contentControl = controls.get();
    Init(app);
}
