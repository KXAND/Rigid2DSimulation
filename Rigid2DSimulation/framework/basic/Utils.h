#pragma once
#include "PlatformIncludes.h"

double constexpr kPi = 3.1415926535;
double constexpr kEulerNumber = 2.7182818284;



// 注意 insd 是开区间，clmp 闭区间。
template<class T>
bool IsBetween(T v, T low, T high) { return low <= v && v < high; }
template<class T>
T const& Clamp(T v, T a, T b) { return min(b, max(a, v)); }

inline int PositiveModulo(int a, int b) { return (a % b + b) % b; }
inline double PositiveModulo(double a, double b) { return fmod(fmod(a, b) + b, b); }
inline double WrapRadians(double an) { return PositiveModulo(an + kPi, 2 * kPi) - kPi; }

// 注意 [a, b] 是闭区间，但 sz 是开的。
int GenerateRandomInt(int a, int b);
int GenerateRandomInt(int sz);
double GenerateRandomFloat(double a, double b);
double GenerateRandomFloat(double b);
double GenerateNormalRandomFloat(double mean, double stddev);

wstring FormatFixed2(double d);
wstring ToWString(string const& s);
string ToString(wstring const& s);
double ParseDouble(string const& s);
double ParseDouble(wstring const& s);
FILE* OpenFile(wstring const& fileName, wstring const& mode);

void HideConsole();
void ShowConsole();
void HideConsoleCursor();
void PrintConsole(wstring const& s, bool newline = false, int width = 80);

void SetLocale();
wstring GetClipboard();
void SetClipboard(wstring const& s);

double GetValue(double x, vector<double> const& ys);
double FetchNumber(int& i, wstring const& s);
wstring FilterCharacters(wstring const& chs, bool ignoreNewline = false);

