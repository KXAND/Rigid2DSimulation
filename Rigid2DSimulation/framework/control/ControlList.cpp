#include "ControlList.h"

HorizontalControlList::HorizontalControlList(vector<Control*> const& controls) : controls(controls) {}
int HorizontalControlList::GetWidth() const {
	if (controls.empty()) { return 0; }
	int totalWidth = 0;
	for (auto control : controls) { totalWidth += control->GetWidth(); } return totalWidth;
}
int HorizontalControlList::GetHeight() const {
	int maxHeight = 0;
	for (auto control : controls)
	if (control->GetHeight() > maxHeight) { maxHeight = control->GetHeight(); } return maxHeight;
}

void HorizontalControlList::Update(App& app) {
	int currentX = topLeft.x;
	for (auto control : controls) {
		control->viewport = viewport;
		control->topLeft.x = currentX; control->topLeft.y = topLeft.y;
		// 成员是要先 Update 然后再求宽度的。
		control->Update(app); currentX += control->GetWidth();
	}
}
void HorizontalControlList::Discard(App& app) { for (auto control : controls) { control->Discard(app); } }
void HorizontalControlList::PreUpdate(App& app) { for (auto control : controls) { control->PreUpdate(app); } }

VerticalControlList::VerticalControlList(vector<Control*> const& controls) : controls(controls) {}
int VerticalControlList::GetWidth() const {
	int maxWidth = 0;
	for (auto control : controls)
	if (control->GetWidth() > maxWidth) { maxWidth = control->GetWidth(); } return maxWidth;
}
int VerticalControlList::GetHeight() const {
	if (controls.empty()) { return 0; }
	int totalHeight = 0;
	for (auto control : controls) { totalHeight += control->GetHeight(); } return totalHeight;
}

void VerticalControlList::Update(App& app) {
	int currentY = topLeft.y;
	for (auto control : controls) {
		control->viewport = viewport;
		control->topLeft.x = topLeft.x; control->topLeft.y = currentY;
		control->Update(app); currentY += control->GetHeight();
	}
}
void VerticalControlList::Discard(App& app) { for (auto control : controls) { control->Discard(app); } }
void VerticalControlList::PreUpdate(App& app) { for (auto control : controls) { control->PreUpdate(app); } }

