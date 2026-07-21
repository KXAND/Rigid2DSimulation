#pragma once
#include "ValuePtr.h"
#include "BracketGroup.h"

// 这里空间可能占的太大了，运行时多态可以解决。但是懒得麻烦了。
struct ScriptExpr {
	wstring typ = L"null";
	// vec 和 access 其实都可以放到 call 的框架里。
	// 但是实现的时候我还是希望分开来，虽然也没啥理由就是了。
	// num, var, str, call, null, vec, access

	bool global = false;
	double num = 0;
	wstring str;
	wstring Value;
	wstring fun;
	vector<ScriptExpr> pars;
	vector<ScriptExpr> vec;
	ValuePtr<ScriptExpr> getLocalizedString, ind;

	ScriptExpr() = default;
	ScriptExpr(wstring const& typ);
	ScriptExpr(double num);
	ScriptExpr(wstring const& fun, vector<ScriptExpr> const& pars);
	static ScriptExpr mkstr(wstring const& str);
	static ScriptExpr mkvar(wstring const& Value, bool gl);
};
wstring ToWString(ScriptExpr const& ex);

struct pre_expr {
	bool istk = false;
	ScriptExpr ex;
	token tk;

	pre_expr() = default;
	pre_expr(ScriptExpr const& ex);
	pre_expr(token const& tk);
};

bool IsUnaryOperator(token const& tk);
bool IsBinaryOperator(token const& tk);

bool ParseNegation(vector<pre_expr> const& in, ScriptExpr& out);
bool ParseBinary(vector<pre_expr> const& in, ScriptExpr& out, vector<wstring> const& mark);
ScriptExpr ParsePreExpression(vector<pre_expr> const& in);

ScriptExpr ParseCall(vector<BracketGroup> const& in);
ScriptExpr ParseBinaryExpressions(vector<BracketGroup> const& in);
ScriptExpr ParseVector(vector<BracketGroup> const& in);
ScriptExpr ParseAccess(vector<BracketGroup> const& in);
ScriptExpr ParseExpression(vector<BracketGroup> const& in);

