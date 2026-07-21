#include "Control.h"

void Delegate::SetControl(App& app, Control* newControl) {
	if (control && control != newControl) { control->Discard(app); }
	control = newControl;
}
int Delegate::GetW() const { return control ? control->GetW() : 0; }
int Delegate::GetH() const { return control ? control->GetH() : 0; }

void Delegate::Update(App& app) {
	if (control) { control->tl = tl; control->vp = vp; control->Update(app); }
}
void Delegate::Discard(App& app) { if (control) { control->Discard(app); } }
void Delegate::PreUpdate(App& app) { if (control) { control->PreUpdate(app); } }

