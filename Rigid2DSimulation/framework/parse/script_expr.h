#pragma once
#include "vptr.h"
#include "bracket_group.h"

// 这里空间可能占的太大了，运行时多态可以解决。但是懒得麻烦了。
struct ScriptExpr {
	wstring typ = L"null";
	// vec 和 access 其实都可以放到 call 的框架里。
	// 但是实现的时候我还是希望分开来，虽然也没啥理由就是了。
	// num, var, str, call, null, vec, access

	bool global = false;
	double num = 0;
	wstring str;
	wstring var;
	wstring fun;
	vector<ScriptExpr> pars;
	vector<ScriptExpr> vec;
	vptr<ScriptExpr> getLocalizedString, ind;

	ScriptExpr() = default;
	ScriptExpr(wstring const& typ);
	ScriptExpr(double num);
	ScriptExpr(wstring const& fun, vector<ScriptExpr> const& pars);
	static ScriptExpr mkstr(wstring const& str);
	static ScriptExpr mkvar(wstring const& var, bool gl);
};
wstring tw(ScriptExpr const& ex);

struct pre_expr {
	bool istk = false;
	ScriptExpr ex;
	token tk;

	pre_expr() = default;
	pre_expr(ScriptExpr const& ex);
	pre_expr(token const& tk);
};

bool is_uni_operator(token const& tk);
bool is_bin_operator(token const& tk);

bool parsing_neg(vector<pre_expr> const& in, ScriptExpr& out);
bool parsing_bin(vector<pre_expr> const& in, ScriptExpr& out, vector<wstring> const& mark);
ScriptExpr parsing_pre_expr(vector<pre_expr> const& in);

ScriptExpr parsing_call(vector<BracketGroup> const& in);
ScriptExpr parsing_bins(vector<BracketGroup> const& in);
ScriptExpr parsing_vec(vector<BracketGroup> const& in);
ScriptExpr parsing_access(vector<BracketGroup> const& in);
ScriptExpr parsing_expr(vector<BracketGroup> const& in);

