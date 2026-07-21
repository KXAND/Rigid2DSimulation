#include "vector3.h"

Vector3::Vector3(wstring const& s) {
	int i = 0;
	x = fetch_num(i, s);
	y = fetch_num(i, s);
	z = fetch_num(i, s);
}
wstring FormatFixed2(Vector3 v) {
	return L"(" + FormatFixed2(v.x) + L"," + FormatFixed2(v.y) + L"," + FormatFixed2(v.z) + L")";
}

Camera::Camera(DRect vp) : vp(vp) {
	r = 1000; scl = 500;
	max_d = 1e6; min_d = 1; calc();
}
void Camera::save(FILE* file) {
	fwrite(&(vp), sizeof(vp), 1, file);
	fwrite(&(p), sizeof(p), 1, file);
	fwrite(&(look), sizeof(look), 1, file);
	fwrite(&(scl), sizeof(scl), 1, file);
	fwrite(&(r), sizeof(r), 1, file);
	fwrite(&(min_d), sizeof(min_d), 1, file);
	fwrite(&(max_d), sizeof(max_d), 1, file);
	fwrite(&(theta), sizeof(theta), 1, file);
	fwrite(&(phi), sizeof(phi), 1, file);
	fwrite(&(psi), sizeof(psi), 1, file);
}
Camera::Camera(FILE* file) {
	fread(&(vp), sizeof(vp), 1, file);
	fread(&(p), sizeof(p), 1, file);
	fread(&(look), sizeof(look), 1, file);
	fread(&(scl), sizeof(scl), 1, file);
	fread(&(r), sizeof(r), 1, file);
	fread(&(min_d), sizeof(min_d), 1, file);
	fread(&(max_d), sizeof(max_d), 1, file);
	fread(&(theta), sizeof(theta), 1, file);
	fread(&(phi), sizeof(phi), 1, file);
	fread(&(psi), sizeof(psi), 1, file);
	calc();
}

void Camera::calc() {
	p.y = -cos(theta) * cos(phi);
	p.x = sin(theta) * cos(phi);
	p.z = sin(phi);
	p = p * r + look;

	// 下面可能会产生奇异的情况。
	Vector3 vy = (look - p).normalize();
	Vector3 vx = cross(vy, Vector3(0, 0, 1)).normalize();
	Vector3 vz = cross(vx, vy);
	mt = Matrix3(vx, vy, vz).transpose();
	mt = Matrix3::rotate(Vector3(0, 1, 0), psi) * mt;
	ct = vp.topLeftPosition + DVector2(vp.w, vp.h) / 2;
}
void Camera::calcFirstPerson() {
	// 第一人称模式，舍弃了 look, r。
	Vector3 vy;
	vy.z = sin(phi);
	vy.x = cos(phi) * sin(theta);
	vy.y = cos(phi) * cos(theta);
	Vector3 vx = cross(vy, Vector3(0, 0, 1)).normalize();
	Vector3 vz = cross(vx, vy);

	mt = Matrix3(vx, vy, vz).transpose();
	mt = Matrix3::rotate(Vector3(0, 1, 0), psi) * mt;
	ct = vp.topLeftPosition + DVector2(vp.w, vp.h) / 2;
}

