#include "Vector3.h"

Vector3::Vector3(wstring const& text)
{
    int index = 0;
    x = FetchNumber(index, text);
    y = FetchNumber(index, text);
    z = FetchNumber(index, text);
}

wstring FormatFixed2(Vector3 vector)
{
    return L"(" + FormatFixed2(vector.x) + L"," + FormatFixed2(vector.y) + L"," + FormatFixed2(vector.z) + L")";
}

Camera::Camera(DRect vp) : vp(vp)
{
    r = 1000;
    scl = 500;
    max_d = 1e6;
    min_d = 1;
    Calculate();
}

void Camera::Save(FILE* file)
{
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

Camera::Camera(FILE* file)
{
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
    Calculate();
}

void Camera::Calculate()
{
    p.y = -cos(theta) * cos(phi);
    p.x = sin(theta) * cos(phi);
    p.z = sin(phi);
    p = p * r + look;

    // 下面可能会产生奇异的情况。
    Vector3 verticalAxis = (look - p).GetNormalized();
    Vector3 horizontalAxis = Cross(verticalAxis, Vector3(0, 0, 1)).GetNormalized();
    Vector3 depthAxis = Cross(horizontalAxis, verticalAxis);
    mt = Matrix3(horizontalAxis, verticalAxis, depthAxis).Transpose();
    mt = Matrix3::Rotation(Vector3(0, 1, 0), psi) * mt;
    ct = vp.topLeftPosition + DVector2(vp.w, vp.h) / 2;
}

void Camera::CalculateFirstPerson()
{
    // 第一人称模式，舍弃了 look, r。
    Vector3 verticalAxis;
    verticalAxis.z = sin(phi);
    verticalAxis.x = cos(phi) * sin(theta);
    verticalAxis.y = cos(phi) * cos(theta);
    Vector3 horizontalAxis = Cross(verticalAxis, Vector3(0, 0, 1)).GetNormalized();
    Vector3 depthAxis = Cross(horizontalAxis, verticalAxis);

    mt = Matrix3(horizontalAxis, verticalAxis, depthAxis).Transpose();
    mt = Matrix3::Rotation(Vector3(0, 1, 0), psi) * mt;
    ct = vp.topLeftPosition + DVector2(vp.w, vp.h) / 2;
}
