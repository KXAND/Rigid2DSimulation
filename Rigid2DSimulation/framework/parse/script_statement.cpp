#include "script_statement.h"
#include "bracket_group.h"

ScriptStatement::ScriptStatement(wstring const& typ) : typ(typ) {}
wstring tw(ScriptStatement const& p) {
	wstring tmp = p.typ;
	if (p.typ == L"define") {
		tmp += L":" + p.def_fun + L",(";
		for (auto& s : p.def_pars) { tmp += s + L","; }
		tmp += L"),{";
		for (auto& pp : p.procs) { tmp += L"\n" + tw(pp) + L","; }
		tmp += L"}";
	}
	else if (p.typ == L"if") {
		tmp += L":" + tw(p.if_con) + L",{";
		for (auto& pp : p.if_procs) { tmp += L"\n" + tw(pp) + L","; }
		tmp += L"}{";
		for (auto& pp : p.else_procs) { tmp += L"\n" + tw(pp) + L","; }
		tmp += L"}";
	}
	else if (p.typ == L"while") {
		tmp += L":" + tw(p.lp_con) + L",{";
		for (auto& pp : p.lp) { tmp += L"\n" + tw(pp) + L","; }
		tmp += L"}";
	}
	else if (p.typ == L"return") { tmp += L":" + tw(p.ret); }
	else if (p.typ == L"single") { tmp += L":" + tw(p.single); }
	else if (p.typ == L"assign") {
		tmp += L":" + tw(p.left) + L"," + tw(p.right);
	}
	return tmp;
}

ScriptStatement parsing_def(vector<BracketGroup> const& in) {
	if (in.size() < 4) { return ScriptStatement(L"error"); }
	// 不想让这里的函数和参数也牵扯到 global 里面去，感觉完全不必要。
	if (in[1].tk.typ != L"var" || in[1].tk.global) { return ScriptStatement(L"error"); }
	ScriptStatement p(L"define");
	p.def_fun = in[1].tk.var;
	bool comma = true;

	for (auto const& b : in[2].bras) {
		if (comma) {
			p.def_pars.push_back(b.tk.var);
			if (b.tk.typ != L"var" || b.tk.global) { return ScriptStatement(L"error"); }
		}
		else if (b.tk.typ != L",") { return ScriptStatement(L"error"); }
		comma = !comma;
	}
	p.procs = parsing_procs(in[3].bras);
	return p;
}
ScriptStatement parsing_ret(vector<BracketGroup> const& in) {
	ScriptStatement p(L"return");
	auto tmp = vector<BracketGroup>(in.begin() + 1, in.end());
	p.ret = tmp.empty() ? ScriptExpr(L"null") : parsing_expr(tmp);
	return p;
}
ScriptStatement parsing_assign(vector<BracketGroup> const& in) {
	ScriptStatement p(L"assign");
	auto it = in.begin();
	for (; it != in.end() && it->tk.typ != L"="; ++it);
	p.left = parsing_expr(vector<BracketGroup>(in.begin(), it));
	p.right = parsing_expr(vector<BracketGroup>(it + 1, in.end()));
	return p;
}
ScriptStatement parsing_if(vector<BracketGroup> const& in) {
	if (in.size() < 3) { return ScriptStatement(L"error"); }
	if (in[1].typ != 1 || in[2].typ != 2) { return ScriptStatement(L"error"); }

	ScriptStatement p(L"if");
	p.if_con = parsing_expr(in[1].bras);
	p.if_procs = parsing_procs(in[2].bras);
	if (in.size() != 3) {
		if (in.size() != 5) { return ScriptStatement(L"error"); }
		if (in[3].tk.typ != L"else" || in[4].typ != 2) { return ScriptStatement(L"error"); }
		p.else_procs = parsing_procs(in[4].bras);
	}
	return p;
}
ScriptStatement parsing_while(vector<BracketGroup> const& in) {
	if (in.size() != 3) { return ScriptStatement(L"error"); }
	if (in[1].typ != 1 || in[2].typ != 2) { return ScriptStatement(L"error"); }
	ScriptStatement p(L"while");
	p.lp_con = parsing_expr(in[1].bras);
	p.lp = parsing_procs(in[2].bras);
	return p;
}

ScriptStatement parsing_proc(vector<BracketGroup> const& in) {
	if (in.empty()) { return ScriptStatement(L"error"); }
	if (in[0].tk.typ == L"def") { return parsing_def(in); }
	if (in[0].tk.typ == L"return") { return parsing_ret(in); }
	if (in[0].tk.typ == L"if") { return parsing_if(in); }
	if (in[0].tk.typ == L"while") { return parsing_while(in); }
	for (auto const& b : in) {
		if (b.tk.typ == L"=") { return parsing_assign(in); }
	}
	ScriptStatement p(L"single");
	p.single = parsing_expr(in);
	return p;
}
vector<ScriptStatement> parsing_procs(vector<BracketGroup> const& in) {
	vector<BracketGroup> cur;
	vector<ScriptStatement> out;
	bool curly = false;

	for (auto const& b : in) {
		if (b.tk.typ == L";") {
			if (!cur.empty()) {
				auto p = parsing_proc(cur);
				if (p.typ == L"error") { return {}; }
				out.push_back(p);
				cur.clear();
			}
		}
		else {
			if (b.tk.typ != L"else" && curly) {
				auto p = parsing_proc(cur);
				if (p.typ == L"error") { return {}; }
				out.push_back(p);
				cur.clear();
			}
			cur.push_back(b);
		}
		curly = (b.typ == 2);
	}

	if (curly) {
		auto p = parsing_proc(cur);
		if (p.typ == L"error") { return {}; }
		out.push_back(p);
	}
	else if (!cur.empty()) {
		return {};
	}
	return out;
}

