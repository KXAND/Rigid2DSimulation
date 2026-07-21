#include "Utils.h"

// 有个大问题是代码的格式与排版到底是怎样的。
// 又想要不死板教条，又想要稳定性，又想要读起来好看，很纠结。
// 后面有些地方的逻辑实在是太乱了，点名 parse 那一块的。

int GenerateRandomInt(int a, int b)
{
	static std::random_device rd;
	static std::mt19937 generator(rd());
	std::uniform_int_distribution<int> d(a, b);
	return d(generator);
}
int GenerateRandomInt(int sz)
{
	return GenerateRandomInt(0, sz - 1);
}

double GenerateRandomFloat(double lowerBound, double upperBound)
{
	static std::random_device seed;
	static std::mt19937 generator(seed());
	std::uniform_real_distribution<double> d(lowerBound, upperBound);
	return d(generator);
}
double GenerateRandomFloat(double upperBound)
{
	return GenerateRandomFloat(0, upperBound);
}

double GenerateNormalRandomFloat(double mean, double stddev)
{
	static std::random_device rd;
	static std::mt19937 generator(rd());
	std::normal_distribution<double> d(mean, stddev);
	return d(generator);
}

// 用 sprintf_s 可以保证安全，但不知道 1024 的空间是否足够。
wstring FormatFixed2(double d)
{
	static wchar_t tmp[1024];
	swprintf_s(tmp, L"%.2lf", d); return tmp;
}
// 下面的规范应该参考 MSDN。
wstring ToWString(string const& s)
{
	vector<wchar_t> tmp(s.size() + 1);
	swprintf_s(tmp.data(), tmp.size(), L"%S", s.c_str());
	return tmp.data();
}
string ToString(wstring const& s)
{
	// 我忘了有没有核实过一定是这个大小了。
	vector<char> tmp(2 * s.size() + 2);
	sprintf_s(tmp.data(), tmp.size(), "%S", s.c_str());
	return tmp.data();
}
double ParseDouble(string const& s) { return std::atof(s.c_str()); }
double ParseDouble(wstring const& s) { return ParseDouble(ToString(s)); }
// 必须用 _wfopen，否则有编码问题。
FILE* OpenFile(wstring const& fileName, wstring const& mode)
{
	FILE* f = NULL;
	_wfopen_s(&f, fileName.c_str(), mode.c_str()); return f;
}

void HideConsole() { ShowWindow(GetConsoleWindow(), SW_HIDE); }
void ShowConsole() { ShowWindow(GetConsoleWindow(), SW_SHOW); }
void HideConsoleCursor()
{
	HANDLE hdl = GetStdHandle(STD_OUTPUT_HANDLE);
	CONSOLE_CURSOR_INFO info;

	GetConsoleCursorInfo(hdl, &info);
	info.bVisible = false;
	SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &info);
}
void PrintConsole(wstring const& s, bool newline, int width)
{
	HideConsoleCursor();

	auto hdl = GetStdHandle(STD_OUTPUT_HANDLE);
	CONSOLE_SCREEN_BUFFER_INFO info;
	GetConsoleScreenBufferInfo(hdl, &info);
	SetConsoleCursorPosition(hdl, { 0, info.dwCursorPosition.Y });
	wprintf(s.c_str());

	GetConsoleScreenBufferInfo(hdl, &info);
	for (int i = (info.dwCursorPosition.X), iEnd = int(width); i < iEnd; ++i) { printf(" "); }
	if (newline) { printf("\n"); }
}

void SetLocale() { setlocale(LC_ALL, "zh-CN"); }
// 我不懂下面的 Lock 和 Unlock 是什么意思。
wstring GetClipboard()
{
	wstring AudioClip;
	if (OpenClipboard(NULL))
	{
		HANDLE hdl = GetClipboardData(CF_UNICODETEXT);
		wchar_t* p = (wchar_t*) GlobalLock(hdl);

		if (p) { AudioClip = p; }
		GlobalUnlock(hdl);
		CloseClipboard();
	} return AudioClip;
}
// 从 MSDN 的例子改的，不太懂，可能有错。
void SetClipboard(wstring const& s)
{
	if (OpenClipboard(NULL))
	{
		int sz = (s.size() + 1) * sizeof(wchar_t);
		HGLOBAL hdl = GlobalAlloc(GMEM_MOVEABLE, sz);
		if (!hdl) { return; }
		wchar_t* p = (wchar_t*) GlobalLock(hdl);
		if (!p) { return; }

		memcpy(p, s.c_str(), sz);
		GlobalUnlock(hdl);
		SetClipboardData(CF_UNICODETEXT, hdl);
		CloseClipboard();
	}
}

double GetValue(double x, vector<double> const& ys)
{
	x = Clamp(x * (ys.size() - 1), 0.0, ys.size() - 1.0);
	int x0 = floor(x), x1 = ceil(x);
	if (x0 == x1) { return ys[x0]; }
	return ys[x0] * (x1 - x) + ys[x1] * (x - x0);
}
double FetchNumber(int& i, wstring const& s)
{
	wstring tmp;
	while (i != s.size())
	{
		auto c = s[i++];
		if (c != L'(' && c != L')')
		{
			if (c == L',') { return ParseDouble(tmp); }
			tmp += c;
		}
	} return ParseDouble(tmp);
}
wstring FilterCharacters(wstring const& chs, bool ignoreNewline)
{
	wstringstream s; bool cr = false;
	for (auto c : chs) if (c == L'\r') { cr = true; }
	for (auto c : chs)
	{
		if (iswprint(c)) { s << c; }
		else
		{
			bool newline =
				c == L'\n' && !cr ||
				c == L'\r' && cr;
			if (newline && !ignoreNewline) { s << L'\n'; }
			if (c == L'\t') { s << L"    "; }
		}
	} return s.str();
}

