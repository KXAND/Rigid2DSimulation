#pragma once
#include "ValuePtr.h"
#include "BracketGroup.h"

// 这里空间可能占的太大了，运行时多态可以解决。但是懒得麻烦了。
struct ScriptExpression {
	wstring typ = L"null";
	// vec 和 access 其实都可以放到 call 的框架里。
	// 但是实现的时候我还是希望分开来，虽然也没啥理由就是了。
	// num, var, str, call, null, vec, access

	bool global = false;
	double num = 0;
	wstring str;
	wstring Value;
	wstring fun;
	vector<ScriptExpression> pars;
	vector<ScriptExpression> vec;
	ValuePtr<ScriptExpression> getLocalizedString, ind;

	ScriptExpression() = default;
	ScriptExpression(wstring const& typ);
	ScriptExpression(double num);
	ScriptExpression(wstring const& fun, vector<ScriptExpression> const& pars);
	static ScriptExpression mkstr(wstring const& str);
	static ScriptExpression mkvar(wstring const& Value, bool gl);
};
wstring ToWString(ScriptExpression const& ex);

struct PreExpression {
	bool istk = false;
	ScriptExpression ex;
	Token tk;

	PreExpression() = default;
	PreExpression(ScriptExpression const& ex);
	PreExpression(Token const& tk);
};

bool IsUnaryOperator(Token const& tk);
bool IsBinaryOperator(Token const& tk);

bool ParseNegation(vector<PreExpression> const& in, ScriptExpression& out);
bool ParseBinary(vector<PreExpression> const& in, ScriptExpression& out, vector<wstring> const& mark);
ScriptExpression ParsePreExpression(vector<PreExpression> const& in);

ScriptExpression ParseCall(vector<BracketGroup> const& in);
ScriptExpression ParseBinaryExpressions(vector<BracketGroup> const& in);
ScriptExpression ParseVector(vector<BracketGroup> const& in);
ScriptExpression ParseAccess(vector<BracketGroup> const& in);
ScriptExpression ParseExpression(vector<BracketGroup> const& in);

