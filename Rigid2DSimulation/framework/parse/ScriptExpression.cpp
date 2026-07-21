#include "ScriptExpression.h"
#include "BracketGroup.h"

ScriptExpression::ScriptExpression(wstring const& typ) : typ(typ) {}
ScriptExpression::ScriptExpression(double num) : num(num) { typ = L"num"; }
ScriptExpression::ScriptExpression(wstring const& fun, vector<ScriptExpression> const& pars) :
	fun(fun), pars(pars) {
	typ = L"call";
}
ScriptExpression ScriptExpression::mkstr(wstring const& str) {
	auto ex = ScriptExpression(L"str");
	ex.str = str;
	return ex;
}
ScriptExpression ScriptExpression::mkvar(wstring const& Value, bool gl) {
	auto ex = ScriptExpression(L"var");
	ex.Value = Value;
	ex.global = gl;
	return ex;
}

wstring ToWString(ScriptExpression const& ex) {
	wstring tmp = L"[" + ex.typ;
	if (ex.typ == L"var") { tmp += (ex.global ? L":&" : L":") + ex.Value; }
	else if (ex.typ == L"num") { tmp += L":" + FormatFixed2(ex.num); }
	else if (ex.typ == L"str") { tmp += L":" + ex.str; }
	else if (ex.typ == L"access") {
		if (!ex.getLocalizedString || !ex.ind) { return {}; }
		tmp += L":" + ToWString(*ex.getLocalizedString) + L"," + ToWString(*ex.ind);
	}
	else if (ex.typ == L"vec") {
		tmp += L":";
		for (auto const& v : ex.vec) { tmp += ToWString(v) + L","; }
	}
	else if (ex.typ == L"call") {
		tmp += L":" + ex.fun + L",";
		for (auto const& par : ex.pars) { tmp += ToWString(par) + L","; }
	}
	tmp += L"]";
	return tmp;
}

PreExpression::PreExpression(ScriptExpression const& ex) : ex(ex) {}
PreExpression::PreExpression(Token const& tk) : tk(tk) { istk = true; }

bool IsUnaryOperator(Token const& tk) {
	// 由于 +，- 的特殊性，它们都被当成二元运算符处理。
	if (tk.typ.empty()) { return false; }
	auto const& s = tk.typ;
	return s == L"!";
}
bool IsBinaryOperator(Token const& tk) {
	if (tk.typ.empty()) { return false; }
	return !iswalnum(tk.typ[0]) && !IsUnaryOperator(tk);
}

bool ParseNegation(vector<PreExpression> const& in, ScriptExpression& out) {
	for (int i = (0), iEnd = int(in.size()); i < iEnd; ++i) {
		auto const& pre = in[i];
		if (!pre.istk) { continue; }
		auto const& s = pre.tk.typ;

		if ((s == L"+" || s == L"-") && (i == 0 || in[i - 1].istk)) {
			bool bad = i == in.size() - 1 || in[i + 1].istk;
			if (bad) {
				out = ScriptExpression(L"error");
				return false;
			}

			auto tmp = vector<PreExpression>(in.begin(), in.begin() + i);
			tmp.push_back(ScriptExpression(s + s, { in[i + 1].ex }));
			for (int j = (i + 2), jEnd = int(in.size()); j < jEnd; ++j) { tmp.push_back(in[j]); }
			out = ParsePreExpression(tmp);
			return true;
		}
	}
	return false;
}
bool ParseBinary(vector<PreExpression> const& in, ScriptExpression& out, vector<wstring> const& mark) {
	for (int i = (0), iEnd = int(in.size()); i < iEnd; ++i) {
		auto const& pre = in[i];
		if (!pre.istk) { continue; }
		auto const& s = pre.tk.typ;

		if (find(mark.begin(), mark.end(), s) != mark.end()) {
			bool bad =
				i == 0 || i == in.size() - 1 ||
				in[i - 1].istk || in[i + 1].istk;
			if (bad) {
				out = ScriptExpression(L"error");
				return false;
			}

			auto tmp = vector<PreExpression>(in.begin(), in.begin() + i - 1);
			tmp.push_back(ScriptExpression(s, { in[i - 1].ex, in[i + 1].ex }));
			for (int j = (i + 2), jEnd = int(in.size()); j < jEnd; ++j) { tmp.push_back(in[j]); }
			out = ParsePreExpression(tmp);
			return true;
		}
	}
	return false;
}
ScriptExpression ParsePreExpression(vector<PreExpression> const& in) {
	if (in.empty()) { return ScriptExpression(L"error"); }
	if (in.size() == 1) {
		if (in[0].istk) { return ScriptExpression(L"error"); }
		return in[0].ex;
	}

	ScriptExpression out;
	if (ParseNegation(in, out)) { return out; }
	if (ParseBinary(in, out, { L"*", L"/" })) { return out; }
	if (ParseBinary(in, out, { L"+", L"-" })) { return out; }
	if (ParseBinary(in, out,
		{ L"==", L"!=", L"<=", L">=", L"<", L">" })) { return out; }
	if (ParseBinary(in, out, { L"&&", L"||" })) { return out; }
	if (ParseBinary(in, out, { L"." })) { return out; }
	return ScriptExpression(L"error");
}

ScriptExpression ParseCall(vector<BracketGroup> const& in) {
	ScriptExpression ex(L"call");
	ex.fun = in[0].tk.Value;
	ex.global = in[0].tk.global;

	vector<BracketGroup> tmp;
	for (auto const& b : in[1].bras) {
		if (b.tk.typ == L",") {
			ex.pars.push_back(ParseExpression(tmp));
			tmp.clear();
		}
		else {
			tmp.push_back(b);
		}
	}
	if (!tmp.empty()) { ex.pars.push_back(ParseExpression(tmp)); }
	return ex;
}
ScriptExpression ParseBinaryExpressions(vector<BracketGroup> const& in) {
	vector<BracketGroup> tmp;
	vector<PreExpression> pres;
	for (auto const& b : in) {
		if (IsBinaryOperator(b.tk)) {
			if (!tmp.empty()) {
				pres.push_back(ParseExpression(tmp));
				tmp.clear();
			}
			pres.push_back(b.tk);
		}
		else {
			tmp.push_back(b);
		}
	}
	if (!tmp.empty()) { pres.push_back(ParseExpression(tmp)); }
	return ParsePreExpression(pres);
}
ScriptExpression ParseVector(vector<BracketGroup> const& in) {
	ScriptExpression ex(L"vec");
	vector<BracketGroup> tmp;
	for (auto const& b : in[0].bras) {
		if (b.tk.typ == L",") {
			ex.vec.push_back(ParseExpression(tmp));
			tmp.clear();
		}
		else {
			tmp.push_back(b);
		}
	}
	if (!tmp.empty()) { ex.vec.push_back(ParseExpression(tmp)); }
	return ex;
}
ScriptExpression ParseAccess(vector<BracketGroup> const& in) {
	ScriptExpression ex(L"access");
	ex.getLocalizedString = ParseExpression(vector<BracketGroup>(in.begin(), in.end() - 1));
	ex.ind = ParseExpression(in.back().bras);
	return ex;
}

ScriptExpression ParseExpression(vector<BracketGroup> const& in) {
	if (in.empty()) { return ScriptExpression(L"error"); }
	auto const& b = in[0];
	if (in.size() == 1) {
		if (b.typ == 0) {
			if (b.tk.typ == L"var") { return ScriptExpression::mkvar(b.tk.Value, b.tk.global); }
			else if (b.tk.typ == L"str") { return ScriptExpression::mkstr(b.tk.str); }
			else if (b.tk.typ == L"num") { return ScriptExpression(b.tk.num); }
			else if (b.tk.typ == L"null") { return ScriptExpression(L"null"); }
			else { return ScriptExpression(L"error"); }
		}
		else if (b.typ == 1) { return ParseExpression(b.bras); }
		else if (b.typ == 3) { return ParseVector(in); }
		else { return ScriptExpression(L"error"); }
	}

	bool call = in.size() == 2 &&
		in[0].tk.typ == L"var" && in[1].typ == 1;
	if (call) { return ParseCall(in); }

	bool uni = in.size() == 2 && IsUnaryOperator(b.tk);
	if (uni) { return ScriptExpression(b.tk.typ, { ParseExpression({ in[1] }) }); }

	for (auto const& br : in) {
		if (IsBinaryOperator(br.tk)) { return ParseBinaryExpressions(in); }
	}

	if (in.back().typ == 3) { return ParseAccess(in); }
	return ScriptExpression(L"error");
}

