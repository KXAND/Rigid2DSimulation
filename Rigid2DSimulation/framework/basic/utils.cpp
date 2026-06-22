#include "utils.h"

// �и��������Ǵ���ĸ�ʽ���Ű浽���������ġ�
// ����Ҫ���������������Ҫ�ȶ��ԣ�����Ҫ�������ÿ����ܾ��ᡣ
// ������Щ�ط����߼�ʵ����̫���ˣ����� parse ��һ��ġ�

int generateRadomInt(int a, int b)
{
	static std::random_device rd;
	static std::mt19937 generator(rd());
	std::uniform_int_distribution<int> d(a, b);
	return d(generator);
}
int generateRadomInt(int sz)
{
	return generateRadomInt(0, sz - 1);
}

double generateRadomFloat(double lowerBound, double upperBound)
{
	static std::random_device seed;
	static std::mt19937 generator(seed());
	std::uniform_real_distribution<double> d(lowerBound, upperBound);
	return d(generator);
}
double generateRadomFloat(double upperBound)
{
	return generateRadomFloat(0, upperBound);
}

double generateNormalRadomFloat(double mean, double stddev)
{
	static std::random_device rd;
	static std::mt19937 generator(rd());
	std::normal_distribution<double> d(mean, stddev);
	return d(generator);
}

// �� sprintf_s ���Ա�֤��ȫ������֪�� 1024 �Ŀռ��Ƿ��㹻��
wstring tw2(double d)
{
	static wchar_t tmp[1024];
	swprintf_s(tmp, L"%.2lf", d); return tmp;
}
// ����Ĺ淶Ӧ�òο� MSDN��
wstring tw(string const& s)
{
	vector<wchar_t> tmp(s.size() + 1);
	swprintf_s(tmp.data(), tmp.size(), L"%S", s.c_str());
	return tmp.data();
}
string to_string(wstring const& s)
{
	// ��������û�к�ʵ��һ���������С�ˡ�
	vector<char> tmp(2 * s.size() + 2);
	sprintf_s(tmp.data(), tmp.size(), "%S", s.c_str());
	return tmp.data();
}
double atof(string const& s) { return atof(s.c_str()); }
double wtof(wstring const& s) { return atof(to_string(s)); }
// ������ _wfopen�������б������⡣
FILE* wfopen(wstring const& nm, wstring const& arg)
{
	FILE* f = NULL;
	_wfopen_s(&f, nm.c_str(), arg.c_str()); return f;
}

void hide_console() { ShowWindow(GetConsoleWindow(), SW_HIDE); }
void show_console() { ShowWindow(GetConsoleWindow(), SW_SHOW); }
void hide_console_cursor()
{
	HANDLE hdl = GetStdHandle(STD_OUTPUT_HANDLE);
	CONSOLE_CURSOR_INFO info;

	GetConsoleCursorInfo(hdl, &info);
	info.bVisible = false;
	SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &info);
}
void print_console(wstring const& s, bool newline, int width)
{
	hide_console_cursor();

	auto hdl = GetStdHandle(STD_OUTPUT_HANDLE);
	CONSOLE_SCREEN_BUFFER_INFO info;
	GetConsoleScreenBufferInfo(hdl, &info);
	SetConsoleCursorPosition(hdl, { 0, info.dwCursorPosition.Y });
	wprintf(s.c_str());

	GetConsoleScreenBufferInfo(hdl, &info);
	rep(i, info.dwCursorPosition.X, width) { printf(" "); }
	if (newline) { printf("\n"); }
}

void set_locale() { setlocale(LC_ALL, "zh-CN"); }
// �Ҳ�������� Lock �� Unlock ��ʲô��˼��
wstring get_clipboard()
{
	wstring clip;
	if (OpenClipboard(NULL))
	{
		HANDLE hdl = GetClipboardData(CF_UNICODETEXT);
		wchar_t* p = (wchar_t*) GlobalLock(hdl);

		if (p) { clip = p; }
		GlobalUnlock(hdl);
		CloseClipboard();
	} return clip;
}
// �� MSDN �����Ӹĵģ���̫���������д���
void set_clipboard(wstring const& s)
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

double get_val(double x, vector<double> const& ys)
{
	x = clamp(x * (ys.size() - 1), 0.0, ys.size() - 1.0);
	int x0 = floor(x), x1 = ceil(x);
	if (x0 == x1) { return ys[x0]; }
	return ys[x0] * (x1 - x) + ys[x1] * (x - x0);
}
double fetch_num(int& i, wstring const& s)
{
	wstring tmp;
	while (i != s.size())
	{
		auto c = s[i++];
		if (c != L'(' && c != L')')
		{
			if (c == L',') { return wtof(tmp); }
			tmp += c;
		}
	} return wtof(tmp);
}
wstring sift_chs(wstring const& chs, bool ignore_newline)
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
			if (newline && !ignore_newline) { s << L'\n'; }
			if (c == L'\t') { s << L"    "; }
		}
	} return s.str();
}

