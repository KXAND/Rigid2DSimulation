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
    explicit DVector2(wstring const& text);

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
    DVector2& operator+=(DVector2 other)
    {
        x += other.x;
        y += other.y;
        return *this;
    }
    DVector2& operator-=(DVector2 other)
    {
        return *this += -other;
    }
    DVector2& operator*=(DVector2 other)
    {
        x *= other.x;
        y *= other.y;
        return *this;
    }
    DVector2& operator/=(DVector2 other)
    {
        x /= other.x;
        y /= other.y;
        return *this;
    }
    DVector2& operator*=(int scale)
    {
        x *= scale;
        y *= scale;
        return *this;
    }
    DVector2& operator/=(int scale)
    {
        x /= scale;
        y /= scale;
        return *this;
    }
};

inline DVector2 operator+(DVector2 left, DVector2 right)
{
    return left += right;
}

inline DVector2 operator-(DVector2 left, DVector2 right)
{
    return left -= right;
}

inline DVector2 operator*(DVector2 left, DVector2 right)
{
    return left *= right;
}

inline DVector2 operator/(DVector2 left, DVector2 right)
{
    return left /= right;
}

inline DVector2 operator*(DVector2 vector, int scale)
{
    return vector *= scale;
}

inline DVector2 operator/(DVector2 vector, int scale)
{
    return vector /= scale;
}

inline DVector2 operator*(int scale, DVector2 vector)
{
    return vector * scale;
}

inline bool operator==(DVector2 left, DVector2 right)
{
    return memcmp(&left, &right, sizeof(DVector2)) == 0;
}

inline bool operator!=(DVector2 left, DVector2 right)
{
    return !(left == right);
}

wstring ToWString(DVector2 vector);

struct DRect
{
    DVector2 topLeftPosition;
    int width = 0;
    int height = 0;

    DRect() = default;
    DRect(int width, int height) : DRect({}, width, height)
    {
    }
    DRect(DVector2 topLeft, int width, int height) : topLeftPosition(topLeft), width(width), height(height)
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
        return GetLeft() + width;
    }
    int GetBottom() const
    {
        return GetTop() + height;
    }
    DVector2 GetCenter() const
    {
        return topLeftPosition + DVector2(width, height) / 2;
    }
};

bool IsInside(DVector2 vector, DRect rectangle);
// 这里可能会出现负的 DRect。
DRect GetOverlapRect(DRect first, DRect second);
