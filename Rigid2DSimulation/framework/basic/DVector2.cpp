#include "DVector2.h"

DVector2::DVector2(wstring const& s)
{
    int i = 0;
    x = FetchNumber(i, s);
    y = FetchNumber(i, s);
}

wstring ToWString(DVector2 v)
{
    return L"(" + std::to_wstring(v.x) + L"," + std::to_wstring(v.y) + L")";
}

bool IsInside(DVector2 v, DRect r)
{
    return
        IsBetween(v.x, r.GetLeft(), r.GetRight()) &&
        IsBetween(v.y, r.GetTop(), r.GetBottom());
}

DRect GetOverlapRect(DRect a, DRect b)
{
    int left = max(a.GetLeft(), b.GetLeft());
    int right = min(a.GetRight(), b.GetRight());
    int top = max(a.GetTop(), b.GetTop());
    int bottom = min(a.GetBottom(), b.GetBottom());
    return DRect({ left, top }, right - left, bottom - top);
}
