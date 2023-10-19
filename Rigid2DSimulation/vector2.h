#pragma once
#include "dvec.h"

struct vector2;
struct vector2 {
	double x = 0, y = 0;
	vector2() = default;
	vector2(double x, double y) : x(x), y(y) {}
	explicit vector2(double a) : vector2(a, a) {}
	explicit vector2(wstring const& s);
	explicit vector2(dvec v) : x(v.x), y(v.y) {}
	explicit operator dvec() const { return dvec(int(x), int(y)); }

	vector2 operator+() const { return *this; }
	//vector2 operator-() const { return { -x, -y }; }

	// 不使用 operator+ 可以避免额外的临时变量构造
	// inline 无法解决返回值产生的拷贝构造
	// 定义在类外可以避免一次临时变量的构造，但是需要友元破坏类的封装性
	/* 运算符重载 */
	vector2 operator-() const { return vector2(-x, -y); }
	vector2 operator+(const vector2& other) const { return vector2(this->x + other.x, this->y + other.y); }
	vector2 operator-(const vector2& other) const { return vector2(this->x - other.x, this->y - other.y); }
	vector2 operator*(const vector2& other) const { return vector2(this->x * other.x, this->y * other.y); }
	vector2 operator/(const vector2& other) const { return vector2(this->x / other.x, this->y / other.y); }

	vector2& operator+=(const vector2& other) { x += other.x; y += other.y; return *this; }
	vector2& operator-=(const vector2& other) { x -= other.x; y -= other.y; return *this; }
	vector2& operator*=(const vector2& other) { x *= other.x; y *= other.y; return *this; }
	vector2& operator/=(const vector2& other) { x /= other.x; y /= other.y; return *this; }

	vector2 operator+(const double& value) const { return vector2(this->x * value, this->y * value); }
	vector2 operator-(const double& value) const { return vector2(this->x / value, this->y / value); }
	vector2 operator*(const double& scale) const { return vector2(this->x * scale, this->y * scale); }
	vector2 operator/(const double& scale) const { return vector2(this->x / scale, this->y / scale); }

	friend vector2 operator*(const double& scale, const vector2& vector);
	vector2& operator+=(const double& value) { x += value; y += value; return *this; }
	vector2& operator-=(const double& value) { x -= value; y -= value; return *this; }
	vector2& operator*=(const double& scale) { x *= scale; y *= scale; return *this; }
	vector2& operator/=(const double& scale) { x /= scale; y /= scale; return *this; }

	bool operator==(const vector2& other) const { return this->x == other.x && this->y == other.y; }
	bool operator!=(const vector2& other) const { return this->x != other.x || this->y != other.y; }

	bool zero() const { return x == 0 && y == 0; }
	bool near_zero(double eps) const {
		return abs(x) <= eps && abs(y) <= eps;
	}
	vector2 unit() const { return zero() ? vector2{} : *this / len(); }
	double len() const { return sqrt(lensqr()); }
	double lensqr() const { return x * x + y * y; }
};
inline double dot(vector2 a, vector2 b) { return a.x * b.x + a.y * b.y; }
//顺序问题
inline vector2 operator*(const double& scale, const vector2& vector) { return vector2(vector.x * scale, vector.y * scale); }

wstring tw2(vector2 v);

struct rect {
	vector2 tl;
	double w = 0, h = 0;

	rect() = default;
	rect(double w, double h) : rect({}, w, h) {}
	rect(vector2 tl, double w, double h) : tl(tl), w(w), h(h) {}

	double left() const { return tl.x; }
	double top() const { return tl.y; }
	double right() const { return left() + w; }
	double bottom() const { return top() + h; }
};
bool insd(vector2 v, rect r);

struct mat2;
inline mat2 operator/(mat2 a, double b);
struct mat2 {
	double a00 = 0, a01 = 0, a10 = 0, a11 = 0;
	mat2() = default;
	mat2(double a00, double a01, double a10, double a11) :
		a00(a00), a01(a01), a10(a10), a11(a11) {}
	mat2(vector2 v0, vector2 v1) : mat2(v0.x, v1.x, v0.y, v1.y) {}

	static mat2 rot(double rad) {
		double c = cos(rad), s = sin(rad);
		return { c, -s, s, c };
	}
	static mat2 id() { return { 1, 0, 0, 1 }; }

	mat2 operator+() const { return *this; }
	mat2 operator-() const { return { -a00, -a01, -a10, -a11 }; }
	mat2& operator+=(mat2 a) {
		a00 += a.a00; a01 += a.a01; a10 += a.a10; a11 += a.a11;
		return *this;
	}
	mat2& operator-=(mat2 a) { return *this += -a; }
	mat2& operator*=(double a) {
		a00 *= a; a01 *= a; a10 *= a; a11 *= a;
		return *this;
	}
	mat2& operator/=(double a) { return *this *= 1 / a; }

	mat2 tsp() const { return { a00, a10, a01, a11 }; }
	mat2 inv() const { return mat2(a11, -a01, -a10, a00) / det(); }
	mat2 abs() const {
		return mat2(::abs(a00), ::abs(a01), ::abs(a10), ::abs(a11));
	}
	double det() const { return a00 * a11 - a01 * a10; }
};
inline mat2 operator+(mat2 a, mat2 b) { return a += b; }
inline mat2 operator-(mat2 a, mat2 b) { return a -= b; }
inline mat2 operator*(mat2 a, double b) { return a *= b; }
inline mat2 operator/(mat2 a, double b) { return a /= b; }
inline mat2 operator*(double a, mat2 b) { return b * a; }
inline vector2 operator*(mat2 a, vector2 b) {
	return {
		a.a00 * b.x + a.a01 * b.y,
		a.a10 * b.x + a.a11 * b.y
	};
}
inline mat2 operator*(mat2 a, mat2 b) {
	return {
		a.a00 * b.a00 + a.a01 * b.a10,
		a.a00 * b.a01 + a.a01 * b.a11,
		a.a10 * b.a00 + a.a11 * b.a10,
		a.a10 * b.a01 + a.a11 * b.a11
	};
}

vector2 bezier(vector<vector2> ps, double t);
double dist_sqr(vector2 a, vector2 b, vector2 p);
