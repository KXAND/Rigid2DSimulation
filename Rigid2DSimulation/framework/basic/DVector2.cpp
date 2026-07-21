#include "DVector2.h"

DVector2::DVector2(wstring const& text)
{
    int index = 0;
    x = FetchNumber(index, text);
    y = FetchNumber(index, text);
}

wstring ToWString(DVector2 vector)
{
    return L"(" + std::to_wstring(vector.x) + L"," + std::to_wstring(vector.y) + L")";
}

bool IsInside(DVector2 vector, DRect rectangle)
{
    return
        IsBetween(vector.x, rectangle.GetLeft(), rectangle.GetRight()) &&
        IsBetween(vector.y, rectangle.GetTop(), rectangle.GetBottom());
}

DRect GetOverlapRect(DRect first, DRect second)
{
    int left = max(first.GetLeft(), second.GetLeft());
    int right = min(first.GetRight(), second.GetRight());
    int top = max(first.GetTop(), second.GetTop());
    int bottom = min(first.GetBottom(), second.GetBottom());
    return DRect({ left, top }, right - left, bottom - top);
}
