#pragma once
#include "utils.h"
#include "script_statement.h"

struct Var;
typedef function<std::shared_ptr<Var>(vector<std::shared_ptr<Var>> const&)> Function;
typedef vector<ScriptStatement> Program;
struct Var {
	// 其实还不是很确定 typ 有没有用，这里会有些乱掉。
	wstring typ = L"null";
	// null, num, str, fun, vec, user, dic
	double num = 0;
	wstring str;
	Function fun;
	Program procs;
	vector<wstring> pars;
	vector<std::shared_ptr<Var>> vec;
	map<wstring, std::shared_ptr<Var>> dic;

	Var() = default;
	Var(double num);
	Var(wstring const& str);
	Var(Function const& fun);
	Var(vector<std::shared_ptr<Var>> const& vec);
	Var(Program const& ps, vector<wstring> const& pars);
};

struct vector2;
struct vector3;
vector2 ToVector2(Var const& v);
vector3 ToVector3(Var const& v);

typedef map<wstring, std::shared_ptr<Var>> Scope;
std::shared_ptr<Var> FindVariable(Scope& curScope, Scope& sc, wstring nm, bool isgl);
// 下面不用常值引用是为了减少不必要的复制。但是说实话我不知道有没有必要。
bool TryEvaluateDefaultFunction(Scope& curScope, Scope& sc, ScriptExpr const& ex, Var& out);

std::shared_ptr<Var> EvaluateCall(Scope& curScope, Scope& sc, ScriptExpr const& ex);
std::shared_ptr<Var> EvaluateAccess(Scope& curScope, Scope& sc, ScriptExpr const& ex);
std::shared_ptr<Var> Evaluate(Scope& curScope, Scope& sc, ScriptExpr const& ex);

// 有一个风险是一个函数会不会在执行过程中把自己抹去，要注意如何规避掉。
std::shared_ptr<Var> Execute(bool& ret, Scope& curScope, Scope& sc, Program const& ps);
std::shared_ptr<Var> Execute(Scope& curScope, Program const& ps);
Program Compile(wstring const& code);

