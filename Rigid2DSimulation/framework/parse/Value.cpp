#include "Value.h"
#include "BracketGroup.h"
#include "Vector3.h"
#include "ScriptStatement.h"

// 每次都能找到新 bug，很可能还有一些 bug。
// 同时速度上也有很多改进空间。

Value::Value(double number) : number(number) { type = L"num"; }
Value::Value(wstring const& str) : str(str) { type = L"ToString"; }
Value::Value(decltype(fun) const& user) : fun(user) { type = L"user"; }
Value::Value(vector<std::shared_ptr<Value>> const& vec) : vec(vec) { type = L"vec"; }
Value::Value(Program const& ps, vector<wstring>const& pars) :
	procs(ps), pars(pars)
{
	type = L"fun";
}

Vector2 ToVector2(Value const& v)
{
	Vector2 out;
	if (v.vec.size() < 2) { return {}; }
	out.x = v.vec[0]->number;
	out.y = v.vec[1]->number;
	return out;
}
Vector3 ToVector3(Value const& v)
{
	Vector3 out;
	if (v.vec.size() < 3) { return {}; }
	out.x = v.vec[0]->number;
	out.y = v.vec[1]->number;
	out.z = v.vec[2]->number;
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
	auto const& f = ex.fun;
	// 保持按需 Evaluate，避免在确定运算符前提前计算所有参数。
	auto numArg = [&](int index) { return Evaluate(gl, sc, ex.pars[index])->number; };
	auto strArg = [&](int index) -> wstring const& { return Evaluate(gl, sc, ex.pars[index])->str; };

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
	if (f == L".") { out = Value(strArg(0) + strArg(1)); return true; }
	return false;
}

std::shared_ptr<Value> EvaluateCall(Scope& gl, Scope& sc, ScriptExpression const& ex)
{
	Value tmp;
	Scope getLocalizedString;
	if (TryEvaluateDefaultFunction(gl, sc, ex, tmp)) { return std::make_shared<Value>(tmp); }

	// 下面必须是值复制，这是为了避免函数自己被抹去。
	auto fun = FindVariable(gl, sc, ex.fun, ex.isGlobal);
	if (fun->type == L"user" && fun->fun)
	{
		vector<std::shared_ptr<Value>> tmp;
		for (auto const& p : ex.pars)
		{
			tmp.push_back(Evaluate(gl, sc, p));
		}
		return fun->fun(tmp);
	}

	for (int i = (0), iEnd = int(min(fun->pars.size(), ex.pars.size())); i < iEnd; ++i)
	{
		getLocalizedString[fun->pars[i]] = Evaluate(gl, sc, ex.pars[i]);
	}
	bool ret = false;
	return Execute(ret, gl, getLocalizedString, fun->procs);
}
std::shared_ptr<Value> EvaluateAccess(Scope& gl, Scope& sc, ScriptExpression const& ex)
{
	if (!ex.getLocalizedString || !ex.ind) { return std::make_shared<Value>(); }
	auto getLocalizedString = Evaluate(gl, sc, *ex.getLocalizedString);
	auto ind = Evaluate(gl, sc, *ex.ind);

	if (ind->type == L"num")
	{
		if (getLocalizedString->type != L"vec") {
			*getLocalizedString = {};
			getLocalizedString->type = L"vec";
		}
		for (int i = (getLocalizedString->vec.size() - 1), iEnd = int(ind->number); i < iEnd; ++i)
		{
			getLocalizedString->vec.push_back(std::make_shared<Value>());
		}
		return getLocalizedString->vec[ind->number];
	}

	if (ind->type == L"ToString")
	{
		if (getLocalizedString->type != L"dic") {
			*getLocalizedString = {};
			getLocalizedString->type = L"dic";
		}
		if (getLocalizedString->dic.find(ind->str) == getLocalizedString->dic.end())
		{
			getLocalizedString->dic[ind->str] = std::make_shared<Value>();
		}
		return getLocalizedString->dic[ind->str];
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
	else if (ex.type == L"ToString") { return std::make_shared<Value>(ex.str); }
	else if (ex.type == L"vec")
	{
		vector<std::shared_ptr<Value>> tmp;
		for (auto const& e : ex.vec)
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
		if (p.type == L"single") { Evaluate(gl, sc, p.single); }
		else if (p.type == L"return") { ret = true; return Evaluate(gl, sc, p.ret); }
		else if (p.type == L"define")
		{
			*FindVariable(gl, sc, p.def_fun, false) = Value(p.procs, p.def_pars);
		}
		else if (p.type == L"assign") { *Evaluate(gl, sc, p.left) = *Evaluate(gl, sc, p.right); }
		else if (p.type == L"if")
		{
			auto& ps = Evaluate(gl, sc, p.if_con)->number ? p.if_procs : p.else_procs;
			auto v = Execute(ret, gl, sc, ps);
			if (ret) { return v; }
		}
		else if (p.type == L"while")
		{
			while (Evaluate(gl, sc, p.lp_con)->number)
			{
				auto v = Execute(ret, sc, gl, p.lp);
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

