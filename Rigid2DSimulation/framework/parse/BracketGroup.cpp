#include "BracketGroup.h"

wstring ToWString(BracketGroup const& b) {
	wstring tmp;
	if (b.type == 0) { tmp += ToWString(b.tk); }
	else {
		tmp += std::to_wstring(b.type) + L" {";
		for (auto const& bb : b.bras) { tmp += ToWString(bb) + L", "; }
		tmp += L"}";
	}
	return tmp;
}

vector<BracketGroup> AnalyzeBrackets(vector<Token> const& in) {
	vector<BracketGroup> out;
	int bra1 = 0, bra2 = 0, bra3 = 0;
	BracketGroup b;
	vector<Token> tmp;
	bool zero = true;
	for (auto const& tk : in) {
		if (tk.type == L"(") {
			++bra1;
			if (!zero) { tmp.push_back(tk); }
			zero = false;
			continue;
		}
		else if (tk.type == L"{") {
			++bra2;
			if (!zero) { tmp.push_back(tk); }
			zero = false;
			continue;
		}
		else if (tk.type == L"[") {
			++bra3;
			if (!zero) { tmp.push_back(tk); }
			zero = false;
			continue;
		}
		else if (tk.type == L")") { --bra1; b.type = 1; }
		else if (tk.type == L"}") { --bra2; b.type = 2; }
		else if (tk.type == L"]") { --bra3; b.type = 3; }
		else { b.type = 0; }

		zero = bra1 == 0 && bra2 == 0 && bra3 == 0;
		if (zero) {
			if (b.type) {
				b.bras = AnalyzeBrackets(tmp);
				tmp.clear();
			}
			else {
				b.tk = tk;
			}
			out.push_back(b);
			b = {};
			// 其它不相关的地方需要是零值，后面为了省事有时会利用这一点。
		}
		else {
			tmp.push_back(tk);
		}
	}
	return zero ? out : vector<BracketGroup>();
}

