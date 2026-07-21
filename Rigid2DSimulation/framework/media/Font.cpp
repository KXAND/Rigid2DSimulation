#include "Font.h"

Font::Font() : Font(20) {}
Font::Font(int h) : h(h) {}
Tile const& Font::operator[](wchar_t c) const {
	auto it = fts.find(c);
	return it == fts.end() ? missing : it->second;
}
wstring Font::rnd(int n) const {
	wstring s;
	for (int i = (0), iEnd = int(n); i < iEnd; ++i) { s += chi[GenerateRandomInt(chi.size())]; } return s;
}

bool Font::load_font(wstring const& name, wchar_t c) {
	wstring path = L"./fonts/" + name + L".tile";
	bool ret = false; Tile t0(path, &ret);
	if (ret) {
		Tile t1 = Tile(t0.width * h / t0.height, h, t0, t0.Rect());
		fts[c] = t1; return true;
	}
	else { return false; }
}
bool Font::init() {
	wstring sym = (wstring)NUMBERS + NORMAL_SYMBOLS + LOWERS;
	wstring upp = (wstring)UPPERS;
	wstring cha = (wstring)CHINESE_CHARS + ADDITIONAL_CHINESE_CHARS;

	load_font(L"less", L'<');
	load_font(L"quote", L'\"');
	load_font(L"blank", L' ');
	load_font(L"slash", L'/');
	load_font(L"colon", L':');
	load_font(L"greater", L'>');
	load_font(L"vertical", L'|');
	load_font(L"asterisk", L'*');
	load_font(L"question", L'?');
	load_font(L"backslash", L'\\');
	for (auto c : sym) { load_font({ c }, c); }
	for (auto c : upp) { load_font({ c, c }, c); }
	for (auto c : cha) {
		if (load_font({ c }, c)) { chi.push_back(c); }
	}

	wstring path = L"./fonts/missing.tile";
	bool ret = false; Tile t0(path, &ret);
	if (ret) {
		Tile t1 = Tile(t0.width * h / t0.height, h, t0, t0.Rect());
		missing = t1; return true;
	}
	else { return false; }
}

