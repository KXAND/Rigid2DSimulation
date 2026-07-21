#pragma once
#include "Utils.h"

// 整型向量
struct DVector2
{
    int x = 0;
    int y = 0;

    DVector2() = default;
    DVector2(int x, int y) : x(x), y(y)
    {
    }
    explicit DVector2(wstring const& s);

    int GetLengthSquared() const
    {
        return x * x + y * y;
    }
    DVector2 const& operator+() const
    {
        return *this;
    }
    DVector2 operator-() const
    {
        return { -x, -y };
    }
    DVector2& operator+=(DVector2 a)
    {
        x += a.x;
        y += a.y;
        return *this;
    }
    DVector2& operator-=(DVector2 a)
    {
        return *this += -a;
    }
    DVector2& operator*=(DVector2 a)
    {
        x *= a.x;
        y *= a.y;
        return *this;
    }
    DVector2& operator/=(DVector2 a)
    {
        x /= a.x;
        y /= a.y;
        return *this;
    }
    DVector2& operator*=(int a)
    {
        x *= a;
        y *= a;
        return *this;
    }
    DVector2& operator/=(int a)
    {
        x /= a;
        y /= a;
        return *this;
    }
};

inline DVector2 operator+(DVector2 a, DVector2 b)
{
    return a += b;
}

inline DVector2 operator-(DVector2 a, DVector2 b)
{
    return a -= b;
}

inline DVector2 operator*(DVector2 a, DVector2 b)
{
    return a *= b;
}

inline DVector2 operator/(DVector2 a, DVector2 b)
{
    return a /= b;
}

inline DVector2 operator*(DVector2 a, int b)
{
    return a *= b;
}

inline DVector2 operator/(DVector2 a, int b)
{
    return a /= b;
}

inline DVector2 operator*(int a, DVector2 b)
{
    return b * a;
}

inline bool operator==(DVector2 a, DVector2 b)
{
    return memcmp(&a, &b, sizeof(DVector2)) == 0;
}

inline bool operator!=(DVector2 a, DVector2 b)
{
    return !(a == b);
}

wstring ToWString(DVector2 v);

struct DRect
{
    DVector2 topLeftPosition;
    int w = 0;
    int h = 0;

    DRect() = default;
    DRect(int w, int h) : DRect({}, w, h)
    {
    }
    DRect(DVector2 tl, int w, int h) : topLeftPosition(tl), w(w), h(h)
    {
    }

    int GetLeft() const
    {
        return topLeftPosition.x;
    }
    int GetTop() const
    {
        return topLeftPosition.y;
    }
    int GetRight() const
    {
        return GetLeft() + w;
    }
    int GetBottom() const
    {
        return GetTop() + h;
    }
    DVector2 GetCenter() const
    {
        return topLeftPosition + DVector2(w, h) / 2;
    }
};

bool IsInside(DVector2 v, DRect r);
// 这里可能会出现负的 DRect。
DRect GetOverlapRect(DRect a, DRect b);
