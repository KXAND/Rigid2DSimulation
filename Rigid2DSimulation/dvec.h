#pragma once
#include "utils.h"

// 整型向量
struct dVector2 {
	int x = 0;
	int y = 0;
	dVector2() = default;
	dVector2(int x, int y) : x(x), y(y) {}
	explicit dVector2(wstring const& s);

	int lensqr() const { return x * x + y * y; }
	dVector2 const& operator+() const { return *this; }
	dVector2 operator-() const { return { -x, -y }; }
	dVector2& operator+=(dVector2 a) { x += a.x; y += a.y; return *this; }
	dVector2& operator-=(dVector2 a) { return *this += -a; }
	dVector2& operator*=(dVector2 a) { x *= a.x; y *= a.y; return *this; }
	dVector2& operator/=(dVector2 a) { x /= a.x; y /= a.y; return *this; }
	dVector2& operator*=(int a) { x *= a; y *= a; return *this; }
	dVector2& operator/=(int a) { x /= a; y /= a; return *this; }
};
inline dVector2 operator+(dVector2 a, dVector2 b) { return a += b; }
inline dVector2 operator-(dVector2 a, dVector2 b) { return a -= b; }
inline dVector2 operator*(dVector2 a, dVector2 b) { return a *= b; }
inline dVector2 operator/(dVector2 a, dVector2 b) { return a /= b; }
inline dVector2 operator*(dVector2 a, int b) { return a *= b; }
inline dVector2 operator/(dVector2 a, int b) { return a /= b; }
inline dVector2 operator*(int a, dVector2 b) { return b * a; }
inline bool operator==(dVector2 a, dVector2 b) {
	return memcmp(&a, &b, sizeof(dVector2)) == 0;
}
inline bool operator!=(dVector2 a, dVector2 b) { return !(a == b); }
wstring tw(dVector2 v);

struct dRect {
	dVector2 topLeftPosition;
	int w = 0, h = 0;
	dRect() = default;
	dRect(int w, int h) : dRect({}, w, h) {}
	dRect(dVector2 tl, int w, int h) : topLeftPosition(tl), w(w), h(h) {}

	int left() const { return topLeftPosition.x; }
	int top() const { return topLeftPosition.y; }
	int right() const { return left() + w; }
	int bottom() const { return top() + h; }
	dVector2 center() const { return topLeftPosition + dVector2(w, h) / 2; }
};
bool isInside(dVector2 v, dRect r);
// 这里可能会出现负的 dRect。
dRect getOverlapRect(dRect a, dRect b);
