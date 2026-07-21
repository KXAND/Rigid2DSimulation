#include "ControlList.h"

HorizontalControlList::HorizontalControlList(vector<Control*> const& cs) : cs(cs) {}
int HorizontalControlList::GetW() const {
	if (cs.empty()) { return 0; }
	int w = 0;
	for (auto c : cs) { w += c->GetW(); } return w;
}
int HorizontalControlList::GetH() const {
	int h = 0;
	for (auto c : cs) 
	if (c->GetH() > h) { h = c->GetH(); } return h;
}

void HorizontalControlList::Update(App& app) {
	int cur_x = tl.x;
	for (auto c : cs) {
		c->vp = vp;
		c->tl.x = cur_x; c->tl.y = tl.y;
		// 成员是要先 Update 然后再求宽度的。
		c->Update(app); cur_x += c->GetW();
	}
}
void HorizontalControlList::Discard(App& app) { for (auto c : cs) { c->Discard(app); } }
void HorizontalControlList::PreUpdate(App& app) { for (auto c : cs) { c->PreUpdate(app); } }

VerticalControlList::VerticalControlList(vector<Control*> const& cs) : cs(cs) {}
int VerticalControlList::GetW() const {
	int w = 0;
	for (auto c : cs)
	if (c->GetW() > w) { w = c->GetW(); } return w;
}
int VerticalControlList::GetH() const {
	if (cs.empty()) { return 0; }
	int h = 0;
	for (auto c : cs) { h += c->GetH(); } return h;
}

void VerticalControlList::Update(App& app) {
	int cur_y = tl.y;
	for (auto c : cs) {
		c->vp = vp;
		c->tl.x = tl.x; c->tl.y = cur_y;
		c->Update(app); cur_y += c->GetH();
	}
}
void VerticalControlList::Discard(App& app) { for (auto c : cs) { c->Discard(app); } }
void VerticalControlList::PreUpdate(App& app) { for (auto c : cs) { c->PreUpdate(app); } }

