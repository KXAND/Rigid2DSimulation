#include "Control.h"

void Delegate::SetControl(App& app, Control* newControl)
{
    if (control && control != newControl)
    {
        control->Discard(app);
    }
    control = newControl;
}
int Delegate::GetWidth() const
{
    return control ? control->GetWidth() : 0;
}
int Delegate::GetHeight() const
{
    return control ? control->GetHeight() : 0;
}

void Delegate::Update(App& app)
{
    if (control)
    {
        control->topLeft = topLeft;
        control->viewport = viewport;
        control->Update(app);
    }
}
void Delegate::Render(App& app)
{
    if (control)
    {
        control->Render(app);
    }
}
void Delegate::Discard(App& app)
{
    if (control)
    {
        control->Discard(app);
    }
}
void Delegate::PreUpdate(App& app)
{
    if (control)
    {
        control->PreUpdate(app);
    }
}
