#include "ScriptStatement.h"
#include "BracketGroup.h"

ScriptStatement::ScriptStatement(wstring const& type) : type(type) {}
wstring ToWString(ScriptStatement const& p) {
	wstring tmp = p.type;
	if (p.type == L"define") {
		tmp += L":" + p.def_fun + L",(";
		for (auto& s : p.def_pars) { tmp += s + L","; }
		tmp += L"),{";
		for (auto& pp : p.procs) { tmp += L"\n" + ToWString(pp) + L","; }
		tmp += L"}";
	}
	else if (p.type == L"if") {
		tmp += L":" + ToWString(p.if_con) + L",{";
		for (auto& pp : p.if_procs) { tmp += L"\n" + ToWString(pp) + L","; }
		tmp += L"}{";
		for (auto& pp : p.else_procs) { tmp += L"\n" + ToWString(pp) + L","; }
		tmp += L"}";
	}
	else if (p.type == L"while") {
		tmp += L":" + ToWString(p.lp_con) + L",{";
		for (auto& pp : p.lp) { tmp += L"\n" + ToWString(pp) + L","; }
		tmp += L"}";
	}
	else if (p.type == L"return") { tmp += L":" + ToWString(p.ret); }
	else if (p.type == L"single") { tmp += L":" + ToWString(p.single); }
	else if (p.type == L"assign") {
		tmp += L":" + ToWString(p.left) + L"," + ToWString(p.right);
	}
	return tmp;
}

ScriptStatement ParseDefinition(vector<BracketGroup> const& in) {
	if (in.size() < 4) { return ScriptStatement(L"error"); }
	// 不想让这里的函数和参数也牵扯到 global 里面去，感觉完全不必要。
	if (in[1].tk.type != L"var" || in[1].tk.isGlobal) { return ScriptStatement(L"error"); }
	ScriptStatement p(L"define");
	p.def_fun = in[1].tk.Value;
	bool comma = true;

	for (auto const& b : in[2].bras) {
		if (comma) {
			p.def_pars.push_back(b.tk.Value);
			if (b.tk.type != L"var" || b.tk.isGlobal) { return ScriptStatement(L"error"); }
		}
		else if (b.tk.type != L",") { return ScriptStatement(L"error"); }
		comma = !comma;
	}
	p.procs = ParseStatements(in[3].bras);
	return p;
}
ScriptStatement ParseReturn(vector<BracketGroup> const& in) {
	ScriptStatement p(L"return");
	auto tmp = vector<BracketGroup>(in.begin() + 1, in.end());
	p.ret = tmp.empty() ? ScriptExpression(L"null") : ParseExpression(tmp);
	return p;
}
ScriptStatement ParseAssignment(vector<BracketGroup> const& in) {
	ScriptStatement p(L"assign");
	auto it = in.begin();
	for (; it != in.end() && it->tk.type != L"="; ++it);
	p.left = ParseExpression(vector<BracketGroup>(in.begin(), it));
	p.right = ParseExpression(vector<BracketGroup>(it + 1, in.end()));
	return p;
}
ScriptStatement ParseIf(vector<BracketGroup> const& in) {
	if (in.size() < 3) { return ScriptStatement(L"error"); }
	if (in[1].type != 1 || in[2].type != 2) { return ScriptStatement(L"error"); }

	ScriptStatement p(L"if");
	p.if_con = ParseExpression(in[1].bras);
	p.if_procs = ParseStatements(in[2].bras);
	if (in.size() != 3) {
		if (in.size() != 5) { return ScriptStatement(L"error"); }
		if (in[3].tk.type != L"else" || in[4].type != 2) { return ScriptStatement(L"error"); }
		p.else_procs = ParseStatements(in[4].bras);
	}
	return p;
}
ScriptStatement ParseWhile(vector<BracketGroup> const& in) {
	if (in.size() != 3) { return ScriptStatement(L"error"); }
	if (in[1].type != 1 || in[2].type != 2) { return ScriptStatement(L"error"); }
	ScriptStatement p(L"while");
	p.lp_con = ParseExpression(in[1].bras);
	p.lp = ParseStatements(in[2].bras);
	return p;
}

ScriptStatement ParseStatement(vector<BracketGroup> const& in) {
	if (in.empty()) { return ScriptStatement(L"error"); }
	if (in[0].tk.type == L"def") { return ParseDefinition(in); }
	if (in[0].tk.type == L"return") { return ParseReturn(in); }
	if (in[0].tk.type == L"if") { return ParseIf(in); }
	if (in[0].tk.type == L"while") { return ParseWhile(in); }
	for (auto const& b : in) {
		if (b.tk.type == L"=") { return ParseAssignment(in); }
	}
	ScriptStatement p(L"single");
	p.single = ParseExpression(in);
	return p;
}
vector<ScriptStatement> ParseStatements(vector<BracketGroup> const& in) {
	vector<BracketGroup> cur;
	vector<ScriptStatement> out;
	bool curly = false;

	for (auto const& b : in) {
		if (b.tk.type == L";") {
			if (!cur.empty()) {
				auto p = ParseStatement(cur);
				if (p.type == L"error") { return {}; }
				out.push_back(p);
				cur.clear();
			}
		}
		else {
			if (b.tk.type != L"else" && curly) {
				auto p = ParseStatement(cur);
				if (p.type == L"error") { return {}; }
				out.push_back(p);
				cur.clear();
			}
			cur.push_back(b);
		}
		curly = (b.type == 2);
	}

	if (curly) {
		auto p = ParseStatement(cur);
		if (p.type == L"error") { return {}; }
		out.push_back(p);
	}
	else if (!cur.empty()) {
		return {};
	}
	return out;
}

