#pragma once
#include "vector2.h"

struct vector3;
inline vector3 operator/(vector3 a, double b);
inline bool operator==(vector3 a, vector3 b);
struct vector3 {
	double x = 0, y = 0, z = 0;
	vector3() = default;
	vector3(double x, double y, double z) : x(x), y(y), z(z) {}
	vector3(double a) : vector3(a, a, a) {}
	explicit vector3(wstring const& s);

	// @todo:for dColor,应该删除
	double& r() { return x; }
	double& g() { return y; }
	double& b() { return z; }
	double r() const { return x; }
	double g() const { return y; }
	double b() const { return z; }

	vector3 operator+() const { return *this; }
	vector3 operator-() const { return { -x, -y, -z }; }
	vector3& operator+=(vector3 a) { x += a.x; y += a.y; z += a.z; return *this; }
	vector3& operator-=(vector3 a) { return *this += -a; }
	vector3& operator*=(vector3 a) { x *= a.x; y *= a.y; z *= a.z; return *this; }
	vector3& operator/=(vector3 a) { x /= a.x; y /= a.y; z /= a.z; return *this; }
	vector3& operator*=(double a) { x *= a; y *= a; z *= a; return *this; }
	vector3& operator/=(double a) { return *this *= 1 / a; }

	bool zero() const { return x == 0 && y == 0 && z == 0; }
	bool near_zero(double eps) const {
		return abs(x) <= eps && abs(y) <= eps && abs(z) <= eps;
	}
	vector3 normalize() const { return zero() ? vector3{} : *this / len(); }
	double len() const { return sqrt(lensqr()); }
	double lensqr() const { return x * x + y * y + z * z; }
};
inline double dot(vector3 a, vector3 b) {
	return a.x * b.x + a.y * b.y + a.z * b.z;
}
inline vector3 cross(vector3 a, vector3 b) {
	return {
		a.y * b.z - a.z * b.y,
		a.z * b.x - a.x * b.z,
		a.x * b.y - a.y * b.x,
	};
}
inline vector3 operator+(vector3 a, vector3 b) { return a += b; }
inline vector3 operator-(vector3 a, vector3 b) { return a -= b; }
inline vector3 operator*(vector3 a, vector3 b) { return a *= b; }
inline vector3 operator/(vector3 a, vector3 b) { return a /= b; }
inline vector3 operator*(vector3 a, double b) { return a *= b; }
inline vector3 operator/(vector3 a, double b) { return a /= b; }
inline vector3 operator*(double a, vector3 b) { return b * a; }
inline bool operator==(vector3 a, vector3 b) {
	return memcmp(&a, &b, sizeof(vector3)) == 0;
}
inline bool operator!=(vector3 a, vector3 b) { return !(a == b); }
wstring tw2(vector3 v);

struct matrix3;
matrix3 operator+(matrix3 a, matrix3 const& b);
matrix3 operator-(matrix3 a, matrix3 const& b);
matrix3 operator*(double a, matrix3 b);
matrix3 operator/(matrix3 a, double b);
struct matrix3 {
	double
		a00 = 0, a01 = 0, a02 = 0,
		a10 = 0, a11 = 0, a12 = 0,
		a20 = 0, a21 = 0, a22 = 0;

	matrix3() = default;
	matrix3(
		double a00, double a01, double a02,
		double a10, double a11, double a12,
		double a20, double a21, double a22) :
		a00(a00), a01(a01), a02(a02),
		a10(a10), a11(a11), a12(a12),
		a20(a20), a21(a21), a22(a22) {}
	matrix3(vector3 v0, vector3 v1, vector3 v2) : matrix3(
		v0.x, v1.x, v2.x,
		v0.y, v1.y, v2.y,
		v0.z, v1.z, v2.z) {}

	static matrix3 I() {
		return {
			1, 0, 0,
			0, 1, 0,
			0, 0, 1,
		};
	}
	static matrix3 cross(vector3 v) {
		// 指的是 u = v × u。
		matrix3 m;
		m.a01 = -v.z;  m.a02 = v.y;
		m.a12 = -v.x;  m.a10 = v.z;
		m.a20 = -v.y;  m.a21 = v.x; return m;
	}

	// 张量 b^T * A
	// @todo: 参数换位置
	static matrix3 tensor(vector3 a, vector3 b) {
		return matrix3(b.x * a, b.y * a, b.z * a);
	}

	// 罗德里格斯旋转公式。
	static matrix3 rotate(vector3 axis, double theta) {
		// 这里假设了 axis 是单位的。
		matrix3 mz = tensor(axis, axis);
		matrix3 mx = matrix3::I() - mz;
		matrix3 my = cross(axis);
		return cos(theta) * mx + sin(theta) * my + mz;
	}

	vector3 col0() const { return { a00, a10, a20 }; }
	vector3 col1() const { return { a01, a11, a21 }; }
	vector3 col2() const { return { a02, a12, a22 }; }
	matrix3 operator+() const { return *this; }
	matrix3 operator-() const {
		return {
			-a00, -a01, -a02,
			-a10, -a11, -a12,
			-a20, -a21, -a22,
		};
	}
	matrix3& operator+=(matrix3 const& a) {
		a00 += a.a00; a01 += a.a01; a02 += a.a02;
		a10 += a.a10; a11 += a.a11; a12 += a.a12;
		a20 += a.a20; a21 += a.a21; a22 += a.a22; return *this;
	}
	matrix3& operator-=(matrix3 const& a) { return *this += -a; }
	matrix3& operator*=(double a) {
		a00 *= a; a01 *= a; a02 *= a;
		a10 *= a; a11 *= a; a12 *= a;
		a20 *= a; a21 *= a; a22 *= a; return *this;
	}
	matrix3& operator/=(double a) { return *this *= 1 / a; }

	// 矩阵转置
	matrix3 transpose() const
	{
		return {
			a00, a10, a20,
			a01, a11, a21,
			a02, a12, a22,
		};
	}

	// 逆矩阵
	matrix3 inverse() const
	{
		// m 是 a 的伴随矩阵的转置矩阵
		// 伴随矩阵：元素的代数余子式构成的矩阵 代数余子式：去掉元素该行该列后做行列式计算得到的值
		// 逆矩阵 = 伴随矩阵 / 行列式值
		matrix3 m;
		m.a00 = a11 * a22 - a12 * a21;
		m.a10 = a12 * a20 - a10 * a22;
		m.a20 = a10 * a21 - a11 * a20;
		m.a01 = a02 * a21 - a01 * a22;
		m.a11 = a00 * a22 - a02 * a20;
		m.a21 = a01 * a20 - a00 * a21;
		m.a02 = a01 * a12 - a02 * a11;
		m.a12 = a02 * a10 - a00 * a12;
		m.a22 = a00 * a11 - a01 * a10;
		return m / det();
	}

	// 行列式运算
	double det() const {
		return
			a00 * (a11 * a22 - a12 * a21) +
			a01 * (a12 * a20 - a10 * a22) +
			a02 * (a10 * a21 - a11 * a20);
	}
};
inline matrix3 operator+(matrix3 a, matrix3 const& b) { return a += b; }
inline matrix3 operator-(matrix3 a, matrix3 const& b) { return a -= b; }
inline matrix3 operator*(matrix3 a, double b) { return a *= b; }
inline matrix3 operator/(matrix3 a, double b) { return a /= b; }
inline matrix3 operator*(double a, matrix3 b) { return b * a; }
inline vector3 operator*(matrix3 const& a, vector3 b) {
	return {
		a.a00 * b.x + a.a01 * b.y + a.a02 * b.z,
		a.a10 * b.x + a.a11 * b.y + a.a12 * b.z,
		a.a20 * b.x + a.a21 * b.y + a.a22 * b.z,
	};
}
inline matrix3 operator*(matrix3 const& a, matrix3 const& b) {
	return {
		a.a00 * b.a00 + a.a01 * b.a10 + a.a02 * b.a20,
		a.a00 * b.a01 + a.a01 * b.a11 + a.a02 * b.a21,
		a.a00 * b.a02 + a.a01 * b.a12 + a.a02 * b.a22,
		a.a10 * b.a00 + a.a11 * b.a10 + a.a12 * b.a20,
		a.a10 * b.a01 + a.a11 * b.a11 + a.a12 * b.a21,
		a.a10 * b.a02 + a.a11 * b.a12 + a.a12 * b.a22,
		a.a20 * b.a00 + a.a21 * b.a10 + a.a22 * b.a20,
		a.a20 * b.a01 + a.a21 * b.a11 + a.a22 * b.a21,
		a.a20 * b.a02 + a.a21 * b.a12 + a.a22 * b.a22,
	};
}

struct cam {
	dRect vp;
	vector3 p;
	vector3 look;
	double scl = 0;
	double r = 0;
	// min_y 必须为正数，为了防止除以 0。
	double min_d = 0, max_d = 0;
	double theta = 0, phi = 0, psi = 0;

	dVector2 ct;
	matrix3 mt;

	explicit cam(dRect vp = {});
	void save(FILE* f);
	explicit cam(FILE* f);

	void calc();
	void calcFirstPerson();
};
