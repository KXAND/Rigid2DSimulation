#pragma once
#include "Utils.h"
#include "ScriptStatement.h"

struct Value;
typedef function<std::shared_ptr<Value>(vector<std::shared_ptr<Value>> const&)> Function;
typedef vector<ScriptStatement> Program;
struct Value {
	// 其实还不是很确定 type 有没有用，这里会有些乱掉。
	wstring type = L"null";
	// null, num, str, fun, vec, user, dic
	double number = 0;
	wstring stringValue;
	Function nativeFunction;
	Program procs;
	vector<wstring> arguments;
	vector<std::shared_ptr<Value>> vec;
	map<wstring, std::shared_ptr<Value>> dic;

	Value() = default;
	Value(double number);
	Value(wstring const& text);
	Value(Function const& nativeFunction);
	Value(vector<std::shared_ptr<Value>> const& vec);
	Value(Program const& ps, vector<wstring> const& arguments);
};

struct Vector2;
struct Vector3;
Vector2 ToVector2(Value const& v);
Vector3 ToVector3(Value const& v);

typedef map<wstring, std::shared_ptr<Value>> Scope;
std::shared_ptr<Value> FindVariable(Scope& curScope, Scope& sc, wstring nm, bool isgl);
// 下面不用常值引用是为了减少不必要的复制。但是说实话我不知道有没有必要。
bool TryEvaluateDefaultFunction(Scope& curScope, Scope& sc, ScriptExpression const& ex, Value& out);

std::shared_ptr<Value> EvaluateCall(Scope& curScope, Scope& sc, ScriptExpression const& ex);
std::shared_ptr<Value> EvaluateAccess(Scope& curScope, Scope& sc, ScriptExpression const& ex);
std::shared_ptr<Value> Evaluate(Scope& curScope, Scope& sc, ScriptExpression const& ex);

// 有一个风险是一个函数会不会在执行过程中把自己抹去，要注意如何规避掉。
std::shared_ptr<Value> Execute(bool& ret, Scope& curScope, Scope& sc, Program const& ps);
std::shared_ptr<Value> Execute(Scope& curScope, Program const& ps);
Program Compile(wstring const& code);

