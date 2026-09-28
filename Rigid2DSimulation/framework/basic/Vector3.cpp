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

Camera::Camera(DRect viewport) : viewport(viewport)
{
    radian = 1000;
    scale = 500;
    maximumDistance = 1e6;
    minimumDistance = 1;
    Calculate();
}

void Camera::Save(FILE* file)
{
    fwrite(&(viewport), sizeof(viewport), 1, file);
    fwrite(&(position), sizeof(position), 1, file);
    fwrite(&(look), sizeof(look), 1, file);
    fwrite(&(scale), sizeof(scale), 1, file);
    fwrite(&(radian), sizeof(radian), 1, file);
    fwrite(&(minimumDistance), sizeof(minimumDistance), 1, file);
    fwrite(&(maximumDistance), sizeof(maximumDistance), 1, file);
    fwrite(&(theta), sizeof(theta), 1, file);
    fwrite(&(phi), sizeof(phi), 1, file);
    fwrite(&(psi), sizeof(psi), 1, file);
}

Camera::Camera(FILE* file)
{
    fread(&(viewport), sizeof(viewport), 1, file);
    fread(&(position), sizeof(position), 1, file);
    fread(&(look), sizeof(look), 1, file);
    fread(&(scale), sizeof(scale), 1, file);
    fread(&(radian), sizeof(radian), 1, file);
    fread(&(minimumDistance), sizeof(minimumDistance), 1, file);
    fread(&(maximumDistance), sizeof(maximumDistance), 1, file);
    fread(&(theta), sizeof(theta), 1, file);
    fread(&(phi), sizeof(phi), 1, file);
    fread(&(psi), sizeof(psi), 1, file);
    Calculate();
}

void Camera::Calculate()
{
    position.y = -cos(theta) * cos(phi);
    position.x = sin(theta) * cos(phi);
    position.z = sin(phi);
    position = position * radian + look;

    // 下面可能会产生奇异的情况。
    Vector3 verticalAxis = (look - position).GetNormalized();
    Vector3 horizontalAxis = Cross(verticalAxis, Vector3(0, 0, 1)).GetNormalized();
    Vector3 depthAxis = Cross(horizontalAxis, verticalAxis);
    transformMatrix = Matrix3(horizontalAxis, verticalAxis, depthAxis).Transpose();
    transformMatrix = Matrix3::Rotation(Vector3(0, 1, 0), psi) * transformMatrix;
    center = viewport.topLeftPosition + DVector2(viewport.width, viewport.height) / 2;
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

    transformMatrix = Matrix3(horizontalAxis, verticalAxis, depthAxis).Transpose();
    transformMatrix = Matrix3::Rotation(Vector3(0, 1, 0), psi) * transformMatrix;
    center = viewport.topLeftPosition + DVector2(viewport.width, viewport.height) / 2;
}
