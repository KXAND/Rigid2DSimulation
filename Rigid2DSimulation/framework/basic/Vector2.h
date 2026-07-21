#pragma once
#include "DVector2.h"

struct Vector2
{
	double x = 0, y = 0;
	Vector2() = default;
	Vector2(double x, double y) : x(x), y(y) { }
	explicit Vector2(double a) : Vector2(a, a) { }
	explicit Vector2(wstring const& s);
	explicit Vector2(DVector2 v) : x(v.x), y(v.y) { }
	explicit operator DVector2() const { return DVector2(int(x), int(y)); }


	// 不使用 operator+ 可以避免额外的临时变量构造
	// inline 无法解决返回值产生的拷贝构造
	// 定义在类外可以避免一次临时变量的构造，但是需要友元破坏类的封装性
	/* 运算符重载 */
	Vector2 operator+() const { return *this; }
	Vector2 operator-() const { return Vector2(-x, -y); }
	Vector2 operator+(const Vector2& other) const { return Vector2(this->x + other.x, this->y + other.y); }
	Vector2 operator-(const Vector2& other) const { return Vector2(this->x - other.x, this->y - other.y); }
	Vector2 operator*(const Vector2& other) const { return Vector2(this->x * other.x, this->y * other.y); }
	Vector2 operator/(const Vector2& other) const { return Vector2(this->x / other.x, this->y / other.y); }

	Vector2& operator+=(const Vector2& other) { x += other.x; y += other.y; return *this; }
	Vector2& operator-=(const Vector2& other) { x -= other.x; y -= other.y; return *this; }
	Vector2& operator*=(const Vector2& other) { x *= other.x; y *= other.y; return *this; }
	Vector2& operator/=(const Vector2& other) { x /= other.x; y /= other.y; return *this; }

	Vector2 operator+(const double& value) const { return Vector2(this->x * value, this->y * value); }
	Vector2 operator-(const double& value) const { return Vector2(this->x / value, this->y / value); }
	Vector2 operator*(const double& scale) const { return Vector2(this->x * scale, this->y * scale); }
	Vector2 operator/(const double& scale) const { return Vector2(this->x / scale, this->y / scale); }

	friend Vector2 operator*(const double& scale, const Vector2& vector);
	Vector2& operator+=(const double& value) { x += value; y += value; return *this; }
	Vector2& operator-=(const double& value) { x -= value; y -= value; return *this; }
	Vector2& operator*=(const double& scale) { x *= scale; y *= scale; return *this; }
	Vector2& operator/=(const double& scale) { x /= scale; y /= scale; return *this; }

	bool operator==(const Vector2& other) const { return this->x == other.x && this->y == other.y; }
	bool operator!=(const Vector2& other) const { return this->x != other.x || this->y != other.y; }

	bool IsZero() const { return x == 0 && y == 0; }
	bool IsNearZero(double eps) const
	{
		return abs(x) <= eps && abs(y) <= eps;
	}
	Vector2 GetNormalized() const { return IsZero() ? Vector2{} : *this / GetLength(); }
	double GetLength() const { return sqrt(GetLengthSquared()); }
	double GetLengthSquared() const { return x * x + y * y; }
};
inline double Dot(const Vector2& a, const Vector2& b) { return a.x * b.x + a.y * b.y; }
//顺序问题
inline Vector2 operator*(const double& scale, const Vector2& vector) { return Vector2(vector.x * scale, vector.y * scale); }

wstring ToWString(Vector2 v);

struct Rect
{
	Vector2 tl;
	double w = 0, h = 0;

	Rect() = default;
	Rect(double w, double h) : Rect({}, w, h) { }
	Rect(Vector2 tl, double w, double h) : tl(tl), w(w), h(h) { }

	double GetLeft() const { return tl.x; }
	double GetTop() const { return tl.y; }
	double GetRight() const { return GetLeft() + w; }
	double GetBottom() const { return GetTop() + h; }
};
bool IsInside(Vector2 v, Rect r);

struct Matrix2;
inline Matrix2 operator/(Matrix2 a, double b);
struct Matrix2
{
	double a00 = 0, a01 = 0, a10 = 0, a11 = 0;
	Matrix2() = default;
	Matrix2(double a00, double a01, double a10, double a11) :
		a00(a00), a01(a01), a10(a10), a11(a11)
	{
	}
	Matrix2(Vector2 v0, Vector2 v1) : Matrix2(v0.x, v1.x, v0.y, v1.y) { }

	static Matrix2 rotate(double rad)
	{
		double c = cos(rad), s = sin(rad);
		return { c, -s, s, c };
	}
	static Matrix2 I() { return { 1, 0, 0, 1 }; }

	Matrix2 operator+() const { return *this; }
	Matrix2 operator-() const { return { -a00, -a01, -a10, -a11 }; }
	Matrix2& operator+=(Matrix2 a)
	{
		a00 += a.a00; a01 += a.a01; a10 += a.a10; a11 += a.a11;
		return *this;
	}
	Matrix2& operator-=(Matrix2 a) { return *this += -a; }
	Matrix2& operator*=(double a)
	{
		a00 *= a; a01 *= a; a10 *= a; a11 *= a;
		return *this;
	}
	Matrix2& operator/=(double a) { return *this *= 1 / a; }

	Matrix2 transpose() const { return { a00, a10, a01, a11 }; }
	Matrix2 inverse() const { return Matrix2(a11, -a01, -a10, a00) / det(); }
	Matrix2 abs() const
	{
		return Matrix2(::abs(a00), ::abs(a01), ::abs(a10), ::abs(a11));
	}
	// 行列式运算
	double det() const { return a00 * a11 - a01 * a10; }
};
inline Matrix2 operator+(Matrix2 a, Matrix2 b) { return a += b; }
inline Matrix2 operator-(Matrix2 a, Matrix2 b) { return a -= b; }
inline Matrix2 operator*(Matrix2 a, double b) { return a *= b; }
inline Matrix2 operator/(Matrix2 a, double b) { return a /= b; }
inline Matrix2 operator*(double a, Matrix2 b) { return b * a; }
inline Vector2 operator*(Matrix2 a, Vector2 b)
{
	return {
		a.a00 * b.x + a.a01 * b.y,
		a.a10 * b.x + a.a11 * b.y
	};
}
inline Matrix2 operator*(Matrix2 a, Matrix2 b)
{
	return {
		a.a00 * b.a00 + a.a01 * b.a10,
		a.a00 * b.a01 + a.a01 * b.a11,
		a.a10 * b.a00 + a.a11 * b.a10,
		a.a10 * b.a01 + a.a11 * b.a11
	};
}

Vector2 GetPositionOnBezier(vector<Vector2> ps, double t);// @todo：没有引用
double GetDistanceSquare(Vector2 a, Vector2 b, Vector2 p);

