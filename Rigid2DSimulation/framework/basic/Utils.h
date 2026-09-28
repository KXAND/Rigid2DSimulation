#pragma once
#include "PlatformIncludes.h"

double constexpr kPi = 3.1415926535;
double constexpr kEulerNumber = 2.7182818284;

// 注意 insd 是开区间，clmp 闭区间。
template <class T>
bool IsBetween(T value, T low, T high)
{
    return low <= value && value < high;
}

template <class T>
T const& Clamp(T value, T low, T high)
{
    return min(high, max(low, value));
}

inline int PositiveModulo(int value, int modulus)
{
    return (value % modulus + modulus) % modulus;
}

inline double PositiveModulo(double value, double modulus)
{
    return fmod(fmod(value, modulus) + modulus, modulus);
}

inline double WrapRadians(double angle)
{
    return PositiveModulo(angle + kPi, 2 * kPi) - kPi;
}

// 注意 [a, b] 是闭区间，但 sz 是开的。
int GenerateRandomInt(int low, int high);
int GenerateRandomInt(int size);
double GenerateRandomFloat(double low, double high);
double GenerateRandomFloat(double high);
double GenerateNormalRandomFloat(double mean, double standardDeviation);

wstring FormatFixed2(double value);
wstring ToWString(string const& text);
string ToString(wstring const& text);
double ParseDouble(string const& text);
double ParseDouble(wstring const& text);
FILE* OpenFile(wstring const& fileName, wstring const& mode);

void HideConsole();
void ShowConsole();
void HideConsoleCursor();
void PrintConsole(wstring const& text, bool newline = false, int width = 80);

void SetLocale();
wstring GetClipboard();
void SetClipboard(wstring const& text);

double GetValue(double x, vector<double> const& values);
double FetchNumber(int& index, wstring const& text);
wstring FilterCharacters(wstring const& text, bool ignoreNewline = false);
