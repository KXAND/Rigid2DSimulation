#include "DVector2.h"

DVector2::DVector2(wstring const& s) {
	int i = 0;
	x = fetch_num(i, s);
	y = fetch_num(i, s);
}
wstring ToWString(DVector2 v) {
	return L"(" + std::to_wstring(v.x) + L"," + std::to_wstring(v.y) + L")";
}

bool isInside(DVector2 v, DRect r) {
	return
		isBetween(v.x, r.left(), r.right()) &&
		isBetween(v.y, r.top(), r.bottom());
}
DRect getOverlapRect(DRect a, DRect b) {
	int left = max(a.left(), b.left());
	int right = min(a.right(), b.right());
	int top = max(a.top(), b.top());
	int bottom = min(a.bottom(), b.bottom());
	return DRect({ left, top }, right - left, bottom - top);
}

