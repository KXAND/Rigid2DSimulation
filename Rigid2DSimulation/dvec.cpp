#include "dvec.h"

dVector2::dVector2(wstring const& s) {
	int i = 0;
	x = fetch_num(i, s);
	y = fetch_num(i, s);
}
wstring tw(dVector2 v) {
	return L"(" + tw(v.x) + L"," + tw(v.y) + L")";
}

bool isInside(dVector2 v, dRect r) {
	return
		isBetween(v.x, r.left(), r.right()) &&
		isBetween(v.y, r.top(), r.bottom());
}
dRect getOverlapRect(dRect a, dRect b) {
	int left = max(a.left(), b.left());
	int right = min(a.right(), b.right());
	int top = max(a.top(), b.top());
	int bottom = min(a.bottom(), b.bottom());
	return dRect({ left, top }, right - left, bottom - top);
}
