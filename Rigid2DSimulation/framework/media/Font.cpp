#include "Font.h"

Font::Font() : Font(20) {}
Font::Font(int h) : h(h) {}
Tile const& Font::operator[](wchar_t c) const {
	auto it = fts.find(c);
	return it == fts.end() ? missing : it->second;
}
wstring Font::GenerateRandomText(int n) const {
	wstring s;
	for (int i = (0), iEnd = int(n); i < iEnd; ++i) { s += chi[GenerateRandomInt(chi.size())]; } return s;
}

bool Font::LoadFont(wstring const& name, wchar_t c) {
	wstring path = L"./fonts/" + name + L".tile";
	bool ret = false; Tile t0(path, &ret);
	if (ret) {
		Tile t1 = Tile(t0.width * h / t0.height, h, t0, t0.Rect());
		fts[c] = t1; return true;
	}
	else { return false; }
}
bool Font::Initialize() {
	wstring sym = (wstring)NUMBERS + NORMAL_SYMBOLS + LOWERS;
	wstring upp = (wstring)UPPERS;
	wstring cha = (wstring)CHINESE_CHARS + ADDITIONAL_CHINESE_CHARS;

	LoadFont(L"less", L'<');
	LoadFont(L"quote", L'\"');
	LoadFont(L"blank", L' ');
	LoadFont(L"slash", L'/');
	LoadFont(L"colon", L':');
	LoadFont(L"greater", L'>');
	LoadFont(L"vertical", L'|');
	LoadFont(L"asterisk", L'*');
	LoadFont(L"question", L'?');
	LoadFont(L"backslash", L'\\');
	for (auto c : sym) { LoadFont({ c }, c); }
	for (auto c : upp) { LoadFont({ c, c }, c); }
	for (auto c : cha) {
		if (LoadFont({ c }, c)) { chi.push_back(c); }
	}

	wstring path = L"./fonts/missing.tile";
	bool ret = false; Tile t0(path, &ret);
	if (ret) {
		Tile t1 = Tile(t0.width * h / t0.height, h, t0, t0.Rect());
		missing = t1; return true;
	}
	else { return false; }
}

