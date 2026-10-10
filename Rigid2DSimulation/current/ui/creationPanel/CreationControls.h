#pragma once

#include "RichLabel.h"
#include "TextBox.h"

enum class CreationSizeField
{
    ParticleRadius,
    PlateThickness
};

class CreationSizeTextBox : public LocalizedTextBox
{
public:
    explicit CreationSizeTextBox(CreationSizeField field);
    void Update(App& app) override;
    void Sync(App& app) override;
    void OnDone(App& app) const override;

private:
    CreationSizeField field;
};

class CreationHelpLabel : public RichLabel
{
public:
    CreationHelpLabel();
    void Sync(App& app) override;
};
