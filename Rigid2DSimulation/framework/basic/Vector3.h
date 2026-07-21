#pragma once
#include "Vector2.h"

struct Vector3;
inline Vector3 operator/(Vector3 a, double b);
inline bool operator==(Vector3 a, Vector3 b);
struct Vector3 {
	double x = 0, y = 0, z = 0;
	Vector3() = default;
	Vector3(double x, double y, double z) : x(x), y(y), z(z) {}
	Vector3(double a) : Vector3(a, a, a) {}
	explicit Vector3(wstring const& text);

	// @todo:for dColor,应该删除
	double& GetRed() { return x; }
	double& GetGreen() { return y; }
	double& GetBlue() { return z; }
	double GetRed() const { return x; }
	double GetGreen() const { return y; }
	double GetBlue() const { return z; }

	Vector3 operator+() const { return *this; }
	Vector3 operator-() const { return { -x, -y, -z }; }
	Vector3& operator+=(Vector3 a) { x += a.x; y += a.y; z += a.z; return *this; }
	Vector3& operator-=(Vector3 a) { return *this += -a; }
	Vector3& operator*=(Vector3 a) { x *= a.x; y *= a.y; z *= a.z; return *this; }
	Vector3& operator/=(Vector3 a) { x /= a.x; y /= a.y; z /= a.z; return *this; }
	Vector3& operator*=(double a) { x *= a; y *= a; z *= a; return *this; }
	Vector3& operator/=(double a) { return *this *= 1 / a; }

	bool IsZero() const { return x == 0 && y == 0 && z == 0; }
	bool IsNearZero(double eps) const {
		return abs(x) <= eps && abs(y) <= eps && abs(z) <= eps;
	}
	Vector3 GetNormalized() const { return IsZero() ? Vector3{} : *this / GetLength(); }
	double GetLength() const { return sqrt(GetLengthSquared()); }
	double GetLengthSquared() const { return x * x + y * y + z * z; }
};
inline double Dot(Vector3 a, Vector3 b) {
	return a.x * b.x + a.y * b.y + a.z * b.z;
}
inline Vector3 Cross(Vector3 a, Vector3 b) {
	return {
		a.y * b.z - a.z * b.y,
		a.z * b.x - a.x * b.z,
		a.x * b.y - a.y * b.x,
	};
}
inline Vector3 operator+(Vector3 a, Vector3 b) { return a += b; }
inline Vector3 operator-(Vector3 a, Vector3 b) { return a -= b; }
inline Vector3 operator*(Vector3 a, Vector3 b) { return a *= b; }
inline Vector3 operator/(Vector3 a, Vector3 b) { return a /= b; }
inline Vector3 operator*(Vector3 a, double b) { return a *= b; }
inline Vector3 operator/(Vector3 a, double b) { return a /= b; }
inline Vector3 operator*(double a, Vector3 b) { return b * a; }
inline bool operator==(Vector3 a, Vector3 b) {
	return memcmp(&a, &b, sizeof(Vector3)) == 0;
}
inline bool operator!=(Vector3 a, Vector3 b) { return !(a == b); }
wstring FormatFixed2(Vector3 vector);

struct Matrix3;
Matrix3 operator+(Matrix3 a, Matrix3 const& b);
Matrix3 operator-(Matrix3 a, Matrix3 const& b);
Matrix3 operator*(double a, Matrix3 b);
Matrix3 operator/(Matrix3 a, double b);
struct Matrix3 {
	double
		a00 = 0, a01 = 0, a02 = 0,
		a10 = 0, a11 = 0, a12 = 0,
		a20 = 0, a21 = 0, a22 = 0;

	Matrix3() = default;
	Matrix3(
		double a00, double a01, double a02,
		double a10, double a11, double a12,
		double a20, double a21, double a22) :
		a00(a00), a01(a01), a02(a02),
		a10(a10), a11(a11), a12(a12),
		a20(a20), a21(a21), a22(a22) {}
	Matrix3(Vector3 v0, Vector3 v1, Vector3 v2) : Matrix3(
		v0.x, v1.x, v2.x,
		v0.y, v1.y, v2.y,
		v0.z, v1.z, v2.z) {}

	static Matrix3 Identity() {
		return {
			1, 0, 0,
			0, 1, 0,
			0, 0, 1,
		};
	}
	static Matrix3 Cross(Vector3 v) {
		// 指的是 u = v × u。
		Matrix3 m;
		m.a01 = -v.z;  m.a02 = v.y;
		m.a12 = -v.x;  m.a10 = v.z;
		m.a20 = -v.y;  m.a21 = v.x; return m;
	}

	// 张量 b^T * A
	// @todo: 参数换位置
	static Matrix3 OuterProduct(Vector3 a, Vector3 b) {
		return Matrix3(b.x * a, b.y * a, b.z * a);
	}

	// 罗德里格斯旋转公式。
	static Matrix3 Rotation(Vector3 axis, double theta) {
		// 这里假设了 axis 是单位的。
		Matrix3 mz = OuterProduct(axis, axis);
		Matrix3 mx = Matrix3::Identity() - mz;
		Matrix3 my = Cross(axis);
		return cos(theta) * mx + sin(theta) * my + mz;
	}

	Vector3 GetColumn0() const { return { a00, a10, a20 }; }
	Vector3 GetColumn1() const { return { a01, a11, a21 }; }
	Vector3 GetColumn2() const { return { a02, a12, a22 }; }
	Matrix3 operator+() const { return *this; }
	Matrix3 operator-() const {
		return {
			-a00, -a01, -a02,
			-a10, -a11, -a12,
			-a20, -a21, -a22,
		};
	}
	Matrix3& operator+=(Matrix3 const& a) {
		a00 += a.a00; a01 += a.a01; a02 += a.a02;
		a10 += a.a10; a11 += a.a11; a12 += a.a12;
		a20 += a.a20; a21 += a.a21; a22 += a.a22; return *this;
	}
	Matrix3& operator-=(Matrix3 const& a) { return *this += -a; }
	Matrix3& operator*=(double a) {
		a00 *= a; a01 *= a; a02 *= a;
		a10 *= a; a11 *= a; a12 *= a;
		a20 *= a; a21 *= a; a22 *= a; return *this;
	}
	Matrix3& operator/=(double a) { return *this *= 1 / a; }

	// 矩阵转置
	Matrix3 Transpose() const
	{
		return {
			a00, a10, a20,
			a01, a11, a21,
			a02, a12, a22,
		};
	}

	// 逆矩阵
	Matrix3 Inverse() const
	{
		// m 是 a 的伴随矩阵的转置矩阵
		// 伴随矩阵：元素的代数余子式构成的矩阵 代数余子式：去掉元素该行该列后做行列式计算得到的值
		// 逆矩阵 = 伴随矩阵 / 行列式值
		Matrix3 m;
		m.a00 = a11 * a22 - a12 * a21;
		m.a10 = a12 * a20 - a10 * a22;
		m.a20 = a10 * a21 - a11 * a20;
		m.a01 = a02 * a21 - a01 * a22;
		m.a11 = a00 * a22 - a02 * a20;
		m.a21 = a01 * a20 - a00 * a21;
		m.a02 = a01 * a12 - a02 * a11;
		m.a12 = a02 * a10 - a00 * a12;
		m.a22 = a00 * a11 - a01 * a10;
		return m / Determinant();
	}

	// 行列式运算
	double Determinant() const {
		return
			a00 * (a11 * a22 - a12 * a21) +
			a01 * (a12 * a20 - a10 * a22) +
			a02 * (a10 * a21 - a11 * a20);
	}
};
inline Matrix3 operator+(Matrix3 a, Matrix3 const& b) { return a += b; }
inline Matrix3 operator-(Matrix3 a, Matrix3 const& b) { return a -= b; }
inline Matrix3 operator*(Matrix3 a, double b) { return a *= b; }
inline Matrix3 operator/(Matrix3 a, double b) { return a /= b; }
inline Matrix3 operator*(double a, Matrix3 b) { return b * a; }
inline Vector3 operator*(Matrix3 const& a, Vector3 b) {
	return {
		a.a00 * b.x + a.a01 * b.y + a.a02 * b.z,
		a.a10 * b.x + a.a11 * b.y + a.a12 * b.z,
		a.a20 * b.x + a.a21 * b.y + a.a22 * b.z,
	};
}
inline Matrix3 operator*(Matrix3 const& a, Matrix3 const& b) {
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

struct Camera {
	DRect viewport;
	Vector3 position;
	Vector3 look;
	double scale = 0;
	double radian = 0;
	// min_y 必须为正数，为了防止除以 0。
	double minimumDistance = 0, maximumDistance = 0;
	double theta = 0, phi = 0, psi = 0;

	DVector2 center;
	Matrix3 transformMatrix;

	explicit Camera(DRect viewport = {});
	void Save(FILE* file);
	explicit Camera(FILE* file);

	void Calculate();
	void CalculateFirstPerson();
};

