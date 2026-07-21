#include "ControlList.h"

HorizontalControlList::HorizontalControlList(vector<Control*> const& controls) : controls(controls) {}
int HorizontalControlList::GetW() const {
	if (controls.empty()) { return 0; }
	int w = 0;
	for (auto control : controls) { w += control->GetW(); } return w;
}
int HorizontalControlList::GetH() const {
	int h = 0;
	for (auto control : controls)
	if (control->GetH() > h) { h = control->GetH(); } return h;
}

void HorizontalControlList::Update(App& app) {
	int currentX = tl.x;
	for (auto control : controls) {
		control->vp = vp;
		control->tl.x = currentX; control->tl.y = tl.y;
		// 成员是要先 Update 然后再求宽度的。
		control->Update(app); currentX += control->GetW();
	}
}
void HorizontalControlList::Discard(App& app) { for (auto control : controls) { control->Discard(app); } }
void HorizontalControlList::PreUpdate(App& app) { for (auto control : controls) { control->PreUpdate(app); } }

VerticalControlList::VerticalControlList(vector<Control*> const& controls) : controls(controls) {}
int VerticalControlList::GetW() const {
	int w = 0;
	for (auto control : controls)
	if (control->GetW() > w) { w = control->GetW(); } return w;
}
int VerticalControlList::GetH() const {
	if (controls.empty()) { return 0; }
	int h = 0;
	for (auto control : controls) { h += control->GetH(); } return h;
}

void VerticalControlList::Update(App& app) {
	int currentY = tl.y;
	for (auto control : controls) {
		control->vp = vp;
		control->tl.x = tl.x; control->tl.y = currentY;
		control->Update(app); currentY += control->GetH();
	}
}
void VerticalControlList::Discard(App& app) { for (auto control : controls) { control->Discard(app); } }
void VerticalControlList::PreUpdate(App& app) { for (auto control : controls) { control->PreUpdate(app); } }

