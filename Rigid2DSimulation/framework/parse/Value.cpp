#include "Value.h"
#include "BracketGroup.h"
#include "Vector3.h"
#include "ScriptStatement.h"

// 每次都能找到新 bug，很可能还有一些 bug。
// 同时速度上也有很多改进空间。

Value::Value(double number) : number(number) { type = L"num"; }
Value::Value(wstring const& text) : stringValue(text) { type = L"ToString"; }
Value::Value(Function const& nativeFunction) : nativeFunction(nativeFunction) { type = L"user"; }
Value::Value(vector<std::shared_ptr<Value>> const& elements) : elements(elements) { type = L"vec"; }
Value::Value(Program const& ps, vector<wstring>const& arguments) :
	bodyStatements(ps), arguments(arguments)
{
	type = L"fun";
}

Vector2 ToVector2(Value const& v)
{
	Vector2 out;
	if (v.elements.size() < 2) { return {}; }
	out.x = v.elements[0]->number;
	out.y = v.elements[1]->number;
	return out;
}
Vector3 ToVector3(Value const& v)
{
	Vector3 out;
	if (v.elements.size() < 3) { return {}; }
	out.x = v.elements[0]->number;
	out.y = v.elements[1]->number;
	out.z = v.elements[2]->number;
	return out;
}

std::shared_ptr<Value> FindVariable(Scope& gl, Scope& sc, wstring nm, bool isgl)
{
	if (isgl)
	{
		if (gl.find(nm) == gl.end()) { gl[nm] = std::make_shared<Value>(); }
		return gl[nm];
	}
	else
	{
		if (sc.find(nm) == sc.end()) { sc[nm] = std::make_shared<Value>(); }
		return sc[nm];
	}
}
bool TryEvaluateDefaultFunction(Scope& gl, Scope& sc, ScriptExpression const& ex, Value& out)
{
	auto const& f = ex.functionName;
	// 保持按需 Evaluate，避免在确定运算符前提前计算所有参数。
	auto numArg = [&](int index) { return Evaluate(gl, sc, ex.arguments[index])->number; };
	auto stringArg = [&](int index) -> wstring const& { return Evaluate(gl, sc, ex.arguments[index])->stringValue; };

	if (f == L"!") { out = Value(!numArg(0)); return true; }
	if (f == L"++") { out = Value(numArg(0)); return true; }
	if (f == L"--") { out = Value(-numArg(0)); return true; }
	if (f == L"Sin") { out = Value(sin(numArg(0)));  return true; }
	if (f == L"Cos") { out = Value(cos(numArg(0)));  return true; }
	if (f == L"Abs") { out = Value(abs(numArg(0)));  return true; }
	if (f == L"+") { out = Value(numArg(0) + numArg(1)); return true; }
	if (f == L"-") { out = Value(numArg(0) - numArg(1)); return true; }
	if (f == L"*") { out = Value(numArg(0) * numArg(1)); return true; }
	if (f == L"/") { out = Value(numArg(0) / numArg(1)); return true; }
	if (f == L"<") { out = Value(numArg(0) < numArg(1)); return true; }
	if (f == L">") { out = Value(numArg(0) > numArg(1)); return true; }
	if (f == L"<=") { out = Value(numArg(0) <= numArg(1)); return true; }
	if (f == L">=") { out = Value(numArg(0) >= numArg(1)); return true; }
	if (f == L"==") { out = Value(numArg(0) == numArg(1)); return true; }
	if (f == L"!=") { out = Value(numArg(0) != numArg(1)); return true; }
	if (f == L"&&") { out = Value(numArg(0) && numArg(1)); return true; }
	if (f == L"||") { out = Value(numArg(0) || numArg(1)); return true; }
	if (f == L"Pow") { out = Value(pow(numArg(0), numArg(1))); return true; }
	if (f == L"Random") { out = Value(GenerateRandomFloat(numArg(0), numArg(1))); return true; }
	if (f == L"RandomInt") { out = Value(GenerateRandomInt(numArg(0), numArg(1))); return true; }
	if (f == L"ToString") { out = Value(FormatFixed2(numArg(0))); return true; }
	if (f == L".") { out = Value(stringArg(0) + stringArg(1)); return true; }
	return false;
}

std::shared_ptr<Value> EvaluateCall(Scope& gl, Scope& sc, ScriptExpression const& ex)
{
	Value tmp;
	Scope localScope;
	if (TryEvaluateDefaultFunction(gl, sc, ex, tmp)) { return std::make_shared<Value>(tmp); }

	// 下面必须是值复制，这是为了避免函数自己被抹去。
	auto functionValue = FindVariable(gl, sc, ex.functionName, ex.isGlobal);
	if (functionValue->type == L"user" && functionValue->nativeFunction)
	{
		vector<std::shared_ptr<Value>> tmp;
		for (auto const& p : ex.arguments)
		{
			tmp.push_back(Evaluate(gl, sc, p));
		}
		return functionValue->nativeFunction(tmp);
	}

	for (int i = (0), iEnd = int(min(functionValue->arguments.size(), ex.arguments.size())); i < iEnd; ++i)
	{
		localScope[functionValue->arguments[i]] = Evaluate(gl, sc, ex.arguments[i]);
	}
	bool ret = false;
	return Execute(ret, gl, localScope, functionValue->bodyStatements);
}
std::shared_ptr<Value> EvaluateAccess(Scope& gl, Scope& sc, ScriptExpression const& ex)
{
	if (!ex.accessedExpression || !ex.indexExpression) { return std::make_shared<Value>(); }
	auto accessedValue = Evaluate(gl, sc, *ex.accessedExpression);
	auto indexValue = Evaluate(gl, sc, *ex.indexExpression);

	if (indexValue->type == L"num")
	{
		if (accessedValue->type != L"vec") {
			*accessedValue = {};
			accessedValue->type = L"vec";
		}
		for (int i = (accessedValue->elements.size() - 1), iEnd = int(indexValue->number); i < iEnd; ++i)
		{
			accessedValue->elements.push_back(std::make_shared<Value>());
		}
		return accessedValue->elements[indexValue->number];
	}

	if (indexValue->type == L"ToString")
	{
		if (accessedValue->type != L"dic") {
			*accessedValue = {};
			accessedValue->type = L"dic";
		}
		if (accessedValue->dictionary.find(indexValue->stringValue) == accessedValue->dictionary.end())
		{
			accessedValue->dictionary[indexValue->stringValue] = std::make_shared<Value>();
		}
		return accessedValue->dictionary[indexValue->stringValue];
	}

	return std::make_shared<Value>();
}
std::shared_ptr<Value> Evaluate(Scope& gl, Scope& sc, ScriptExpression const& ex)
{
	if (ex.type == L"call") { return EvaluateCall(gl, sc, ex); }
	else if (ex.type == L"var")
	{
		if (ex.variableName == L"kPi") { return std::make_shared<Value>(kPi); }
		return FindVariable(gl, sc, ex.variableName, ex.isGlobal);
	}
	else if (ex.type == L"num") { return std::make_shared<Value>(ex.number); }
	else if (ex.type == L"ToString") { return std::make_shared<Value>(ex.stringValue); }
	else if (ex.type == L"vec")
	{
		vector<std::shared_ptr<Value>> tmp;
		for (auto const& e : ex.elements)
		{
			tmp.push_back(std::make_shared<Value>(*Evaluate(gl, sc, e)));
		}
		return std::make_shared<Value>(tmp);
	}
	else if (ex.type == L"access") { return EvaluateAccess(gl, sc, ex); }
	return std::make_shared<Value>();
}

std::shared_ptr<Value> Execute(bool& ret, Scope& gl, Scope& sc, Program const& ps)
{
	for (auto const& p : ps)
	{
		if (p.type == L"single") { Evaluate(gl, sc, p.expression); }
		else if (p.type == L"return") { ret = true; return Evaluate(gl, sc, p.returnExpression); }
		else if (p.type == L"define")
		{
			*FindVariable(gl, sc, p.functionName, false) = Value(p.bodyStatements, p.parameterNames);
		}
		else if (p.type == L"assign") { *Evaluate(gl, sc, p.leftExpression) = *Evaluate(gl, sc, p.rightExpression); }
		else if (p.type == L"if")
		{
			auto& ps = Evaluate(gl, sc, p.condition)->number ? p.thenStatements : p.elseStatements;
			auto v = Execute(ret, gl, sc, ps);
			if (ret) { return v; }
		}
		else if (p.type == L"while")
		{
			while (Evaluate(gl, sc, p.loopCondition)->number)
			{
				auto v = Execute(ret, sc, gl, p.loopStatements);
				if (ret) { return v; }
			}
		}
	}
	return std::make_shared<Value>();
}
std::shared_ptr<Value> Execute(Scope& gl, Program const& ps)
{
	bool ret = false;
	return Execute(ret, gl, gl, ps);
}
Program Compile(wstring const& code)
{
	return ParseStatements(AnalyzeBrackets(Tokenize(code)));
}

