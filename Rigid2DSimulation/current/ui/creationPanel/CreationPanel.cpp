#include "CreationPanel.h"

#include "Creator.h"

CreationPanel::CreationPanel(App& app) : LocalizedFloatPanel(app, 400, 680, L"creationPanelTitle")
{
    toolButtons = {
        std::make_shared<CreationToolButton>(kCreateBox, L"creationBoxTool"),
        std::make_shared<CreationToolButton>(kCreateBall, L"creationBallTool"),
        std::make_shared<CreationToolButton>(kCreatePlate, L"creationPlateTool"),
        std::make_shared<CreationToolButton>(kCreateParticle, L"creationParticleTool"),
        std::make_shared<CreationToolButton>(kCreateConnection, L"creationConnectionTool"),
        std::make_shared<CreationToolButton>(kCreatePoint, L"creationPointTool"),
        std::make_shared<CreationToolButton>(kCreateNail, L"creationNailTool")
    };
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
