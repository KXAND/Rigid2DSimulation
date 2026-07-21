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
	Program bodyStatements;
	vector<wstring> arguments;
	vector<std::shared_ptr<Value>> elements;
	map<wstring, std::shared_ptr<Value>> dictionary;

	Value() = default;
	Value(double number);
	Value(wstring const& text);
	Value(Function const& nativeFunction);
	Value(vector<std::shared_ptr<Value>> const& elements);
	Value(Program const& statements, vector<wstring> const& arguments);
};

struct Vector2;
struct Vector3;
Vector2 ToVector2(Value const& value);
Vector3 ToVector3(Value const& value);

typedef map<wstring, std::shared_ptr<Value>> Scope;
std::shared_ptr<Value> FindVariable(Scope& globalScope, Scope& scope, wstring variableName, bool isGlobal);
// 下面不用常值引用是为了减少不必要的复制。但是说实话我不知道有没有必要。
bool TryEvaluateDefaultFunction(Scope& globalScope, Scope& scope, ScriptExpression const& expression, Value& result);

std::shared_ptr<Value> EvaluateCall(Scope& globalScope, Scope& scope, ScriptExpression const& expression);
std::shared_ptr<Value> EvaluateAccess(Scope& globalScope, Scope& scope, ScriptExpression const& expression);
std::shared_ptr<Value> Evaluate(Scope& globalScope, Scope& scope, ScriptExpression const& expression);

// 有一个风险是一个函数会不会在执行过程中把自己抹去，要注意如何规避掉。
std::shared_ptr<Value> Execute(bool& hasReturned, Scope& globalScope, Scope& scope, Program const& statements);
std::shared_ptr<Value> Execute(Scope& globalScope, Program const& statements);
Program Compile(wstring const& code);

