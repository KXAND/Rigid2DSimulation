#include "vector3.h"

vector3::vector3(wstring const& s) {
	int i = 0;
	x = fetch_num(i, s);
	y = fetch_num(i, s);
	z = fetch_num(i, s);
}
wstring tw2(vector3 v) {
	return L"(" + tw2(v.x) + L"," + tw2(v.y) + L"," + tw2(v.z) + L")";
}

cam::cam(dRect vp) : vp(vp) {
	r = 1000; scl = 500;
	max_d = 1e6; min_d = 1; calc();
}
void cam::save(FILE* f) {
	fwt(vp); fwt(p); fwt(look);
	fwt(scl); fwt(r);
	fwt(min_d); fwt(max_d);
	fwt(theta); fwt(phi); fwt(psi);
}
cam::cam(FILE* f) {
	frd(vp); frd(p); frd(look);
	frd(scl); frd(r);
	frd(min_d); frd(max_d);
	frd(theta); frd(phi); frd(psi); calc();
}

void cam::calc() {
	p.y = -cos(theta) * cos(phi);
	p.x = sin(theta) * cos(phi);
	p.z = sin(phi);
	p = p * r + look;

	// 下面可能会产生奇异的情况。
	vector3 vy = (look - p).normalize();
	vector3 vx = cross(vy, vector3(0, 0, 1)).normalize();
	vector3 vz = cross(vx, vy);
	mt = matrix3(vx, vy, vz).transpose();
	mt = matrix3::rotate(vector3(0, 1, 0), psi) * mt;
	ct = vp.topLeftPosition + dVector2(vp.w, vp.h) / 2;
}
void cam::calcFirstPerson() {
	// 第一人称模式，舍弃了 look, r。
	vector3 vy;
	vy.z = sin(phi);
	vy.x = cos(phi) * sin(theta);
	vy.y = cos(phi) * cos(theta);
	vector3 vx = cross(vy, vector3(0, 0, 1)).normalize();
	vector3 vz = cross(vx, vy);

	mt = matrix3(vx, vy, vz).transpose();
	mt = matrix3::rotate(vector3(0, 1, 0), psi) * mt;
	ct = vp.topLeftPosition + dVector2(vp.w, vp.h) / 2;
}
