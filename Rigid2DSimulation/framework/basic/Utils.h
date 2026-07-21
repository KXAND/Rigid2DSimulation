#pragma once
#include "PlatformIncludes.h"

double constexpr PI = 3.1415926535;
double constexpr EU = 2.7182818284;



// 注意 insd 是开区间，clmp 闭区间。
template<class T>
bool isBetween(T v, T low, T high) { return low <= v && v < high; }
template<class T>
T const& clamp(T v, T a, T b) { return min(b, max(a, v)); }

inline int mod(int a, int b) { return (a % b + b) % b; }
inline double modf(double a, double b) { return fmod(fmod(a, b) + b, b); }
inline double modp(double an) { return modf(an + PI, 2 * PI) - PI; }

// 注意 [a, b] 是闭区间，但 sz 是开的。
int GenerateRandomInt(int a, int b);
int GenerateRandomInt(int sz);
double GenerateRandomFloat(double a, double b);
double GenerateRandomFloat(double b);
double GenerateNormalRandomFloat(double mean, double stddev);

wstring FormatFixed2(double d);
wstring ToWString(string const& s);
string ToString(wstring const& s);
double atof(string const& s);
double wtof(wstring const& s);
FILE* wfopen(wstring const& nm, wstring const& arg);

void HideConsole();
void ShowConsole();
void HideConsoleCursor();
void PrintConsole(wstring const& s, bool newline = false, int width = 80);

void SetLocale();
wstring GetClipboard();
void SetClipboard(wstring const& s);

double GetValue(double x, vector<double> const& ys);
double FetchNumber(int& i, wstring const& s);
wstring FilterCharacters(wstring const& chs, bool ignore_newline = false);

