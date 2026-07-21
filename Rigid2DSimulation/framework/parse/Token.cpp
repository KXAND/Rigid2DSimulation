#include "Token.h"

Token::Token(wstring const& typ) : typ(typ) {}
Token::Token(double num) : num(num) { typ = L"num"; }
Token Token::CreateString(wstring const& text) {
	Token tk(L"str");
	tk.str = text; return tk;
}
Token Token::CreateVariable(wstring const& value, bool isGlobal) {
	Token tk(L"var");
	tk.Value = value; tk.global = isGlobal; return tk;
}
wstring ToWString(Token const& tk) {
	wstring tmp = L"[" + tk.typ;
	if (tk.typ == L"var") { tmp += (tk.global ? L":&" : L":") + tk.Value; }
	if (tk.typ == L"num") { tmp += L":" + FormatFixed2(tk.num); }
	if (tk.typ == L"str") { tmp += L":" + tk.str; }
	tmp += L"]"; return tmp;
}

Token ParseNumberToken(wstring const& in, int& id) {
	wstring tmp = { in[id] }; bool ok = true;
	bool dot = false;
	while (ok && ++id != in.size()) {
		auto c = in[id];
		if (c == L'.') {
			if (dot) { return Token(L"error"); }
			dot = true;
		}
		ok = iswdigit(c) || c == L'.';
		if (ok) { tmp += c; }
	}
	auto c = in[id];
	if (isalpha(c) || c == L'_') { return Token(L"error"); }
	return Token(ParseDouble(tmp));
}
Token ParseStringToken(wstring const& in, int& id) {
	wstring tmp; bool ok = true;
	bool escape = false;
	while (ok && ++id != in.size()) {
		auto c = in[id];
		if (c == L'\n') { return Token(L"error"); }
		if (escape) {
			if (c == L'n') { c = L'\n'; }
			escape = false; ok = true;
		} else { escape = (c == L'\\'); ok = (c != L'\"'); }
		if (ok && !escape) { tmp += c; }
	} ++id;
	return Token::CreateString(tmp);
}
Token ParseMarkToken(wstring const& in, int& id) {
	vector<wstring> two = {
		L"||", L"&&", L"==", L"!=", L"<=", L">=", L"//"
	};
	if (id + 1 < in.size())
	for (auto const& s : two)
	if (in[id] == s[0] && in[id + 1] == s[1]) { id += 2; return Token(s); }

	if (id < in.size())
	for (auto c : (wstring)L",.+-*/=<>!(){}[]&; \n\"")
	if (in[id] == c) { ++id; return Token(wstring({ c })); }
	return Token(L"error");
}
Token ParseGlobalVariableToken(wstring const& in, int& id) {
	wstring tmp; bool ok = true;
	// 下面这行判断的顺序很重要，ok 不能移到右边。
	while (ok && ++id != in.size()) {
		auto c = in[id];
		ok = iswalpha(c) || c == L'_' || iswdigit(c);
		if (ok) { tmp += c; }
	} return Token::CreateVariable(tmp, true);
}
Token ParseVariableOrReservedToken(wstring const& in, int& id) {
	wstring tmp = { in[id] }; bool ok = true;
	// 下面这行判断的顺序很重要，ok 不能移到右边。
	while (ok && ++id != in.size()) {
		auto c = in[id];
		ok = iswalpha(c) || c == L'_' || iswdigit(c);
		if (ok) { tmp += c; }
	}
	bool reserved =
		tmp == L"def" || tmp == L"return" || tmp == L"if" ||
		tmp == L"else" || tmp == L"while" || tmp == L"null";
	if (reserved) { return Token(tmp); }
	return Token::CreateVariable(tmp, false);
}
Token ParseToken(wstring const& in, int& id) {
	auto c = in[id];
	if (c == L'&') { return ParseGlobalVariableToken(in, id); }
	if (iswalpha(c) || c == L'_') { return ParseVariableOrReservedToken(in, id); }
	if (iswdigit(c)) { return ParseNumberToken(in, id); }
	if (c == L'\"') { return ParseStringToken(in, id); }
	return ParseMarkToken(in, id);
}

vector<Token> NormalizeSigns(vector<Token> const& in) {
	vector<Token> out;
	bool add = false; int sgn = 1;

	for (auto const& tk : in) {
		bool old_add = add;
		add = tk.typ == L"+" || tk.typ == L"-";
		if (add) {
			if (old_add) { sgn *= tk.typ == L"+" ? 1 : -1; }
			else { sgn = tk.typ == L"+" ? 1 : -1; } continue;
		} else if (old_add) {
			out.push_back(Token(sgn == 1 ? L"+" : L"-"));
		} out.push_back(tk);
	}
	if (add) { out.push_back(Token(sgn == 1 ? L"+" : L"-")); }
	return out;
}
vector<Token> Tokenize(wstring const& in) {
	int id = 0; vector<Token> tmp;
	while (id != in.size()) {
		// 你之前在 tk 前面加了 const &，千万不要在这样了。
		auto tk = ParseToken(in, id);
		bool blank =
			tk.typ == L" " || tk.typ == L"\n";
		if (tk.typ == L"//") {
			while (id != in.size() && in[id++] != L'\n');
		}
		else if (tk.typ == L"error") { return {}; }
		else if (!blank) { tmp.push_back(tk); }
	} return NormalizeSigns(tmp);
}

