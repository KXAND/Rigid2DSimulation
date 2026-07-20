#include "var.h"
#include "bracket_group.h"
#include "vector3.h"
#include "script_statement.h"

// 每次都能找到新 bug，很可能还有一些 bug。
// 同时速度上也有很多改进空间。

Var::Var(double num) : num(num) { typ = L"num"; }
Var::Var(wstring const& str) : str(str) { typ = L"str"; }
Var::Var(decltype(fun) const& user) : fun(user) { typ = L"user"; }
Var::Var(vector<ptr<Var>> const& vec) : vec(vec) { typ = L"vec"; }
Var::Var(Program const& ps, vector<wstring>const& pars) :
	procs(ps), pars(pars)
{
	typ = L"fun";
}

vector2 ToVector2(Var const& v)
{
	vector2 out;
	if (v.vec.size() < 2) { return {}; }
	out.x = v.vec[0]->num;
	out.y = v.vec[1]->num;
	return out;
}
vector3 ToVector3(Var const& v)
{
	vector3 out;
	if (v.vec.size() < 3) { return {}; }
	out.x = v.vec[0]->num;
	out.y = v.vec[1]->num;
	out.z = v.vec[2]->num;
	return out;
}

ptr<Var> FindVariable(Scope& gl, Scope& sc, wstring nm, bool isgl)
{
	if (isgl)
	{
		if (gl.find(nm) == gl.end()) { gl[nm] = msh<Var>(); }
		return gl[nm];
	}
	else
	{
		if (sc.find(nm) == sc.end()) { sc[nm] = msh<Var>(); }
		return sc[nm];
	}
}
bool TryEvaluateDefaultFunction(Scope& gl, Scope& sc, ScriptExpr const& ex, Var& out)
{
	auto const& f = ex.fun;
	// 保持按需 Evaluate，避免在确定运算符前提前计算所有参数。
	auto numArg = [&](int index) { return Evaluate(gl, sc, ex.pars[index])->num; };
	auto strArg = [&](int index) -> wstring const& { return Evaluate(gl, sc, ex.pars[index])->str; };

	if (f == L"!") { out = Var(!numArg(0)); return true; }
	if (f == L"++") { out = Var(numArg(0)); return true; }
	if (f == L"--") { out = Var(-numArg(0)); return true; }
	if (f == L"sin") { out = Var(sin(numArg(0)));  return true; }
	if (f == L"cos") { out = Var(cos(numArg(0)));  return true; }
	if (f == L"abs") { out = Var(abs(numArg(0)));  return true; }
	if (f == L"+") { out = Var(numArg(0) + numArg(1)); return true; }
	if (f == L"-") { out = Var(numArg(0) - numArg(1)); return true; }
	if (f == L"*") { out = Var(numArg(0) * numArg(1)); return true; }
	if (f == L"/") { out = Var(numArg(0) / numArg(1)); return true; }
	if (f == L"<") { out = Var(numArg(0) < numArg(1)); return true; }
	if (f == L">") { out = Var(numArg(0) > numArg(1)); return true; }
	if (f == L"<=") { out = Var(numArg(0) <= numArg(1)); return true; }
	if (f == L">=") { out = Var(numArg(0) >= numArg(1)); return true; }
	if (f == L"==") { out = Var(numArg(0) == numArg(1)); return true; }
	if (f == L"!=") { out = Var(numArg(0) != numArg(1)); return true; }
	if (f == L"&&") { out = Var(numArg(0) && numArg(1)); return true; }
	if (f == L"||") { out = Var(numArg(0) || numArg(1)); return true; }
	if (f == L"pow") { out = Var(pow(numArg(0), numArg(1))); return true; }
	if (f == L"rand") { out = Var(GenerateRandomFloat(numArg(0), numArg(1))); return true; }
	if (f == L"rand_int") { out = Var(GenerateRandomInt(numArg(0), numArg(1))); return true; }
	if (f == L"str") { out = Var(tw2(numArg(0))); return true; }
	if (f == L".") { out = Var(strArg(0) + strArg(1)); return true; }
	return false;
}

ptr<Var> EvaluateCall(Scope& gl, Scope& sc, ScriptExpr const& ex)
{
	Var tmp;
	Scope getLocalizedString;
	if (TryEvaluateDefaultFunction(gl, sc, ex, tmp)) { return msh<Var>(tmp); }

	// 下面必须是值复制，这是为了避免函数自己被抹去。
	auto fun = FindVariable(gl, sc, ex.fun, ex.global);
	if (fun->typ == L"user" && fun->fun)
	{
		vector<ptr<Var>> tmp;
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
ptr<Var> EvaluateAccess(Scope& gl, Scope& sc, ScriptExpr const& ex)
{
	if (!ex.getLocalizedString || !ex.ind) { return msh<Var>(); }
	auto getLocalizedString = Evaluate(gl, sc, *ex.getLocalizedString);
	auto ind = Evaluate(gl, sc, *ex.ind);

	if (ind->typ == L"num")
	{
		if (getLocalizedString->typ != L"vec") {
			*getLocalizedString = {};
			getLocalizedString->typ = L"vec";
		}
		for (int i = (getLocalizedString->vec.size() - 1), iEnd = int(ind->num); i < iEnd; ++i)
		{
			getLocalizedString->vec.push_back(msh<Var>());
		}
		return getLocalizedString->vec[ind->num];
	}

	if (ind->typ == L"str")
	{
		if (getLocalizedString->typ != L"dic") {
			*getLocalizedString = {};
			getLocalizedString->typ = L"dic";
		}
		if (getLocalizedString->dic.find(ind->str) == getLocalizedString->dic.end())
		{
			getLocalizedString->dic[ind->str] = msh<Var>();
		}
		return getLocalizedString->dic[ind->str];
	}

	return msh<Var>();
}
ptr<Var> Evaluate(Scope& gl, Scope& sc, ScriptExpr const& ex)
{
	if (ex.typ == L"call") { return EvaluateCall(gl, sc, ex); }
	else if (ex.typ == L"var")
	{
		if (ex.var == L"PI") { return msh<Var>(PI); }
		return FindVariable(gl, sc, ex.var, ex.global);
	}
	else if (ex.typ == L"num") { return msh<Var>(ex.num); }
	else if (ex.typ == L"str") { return msh<Var>(ex.str); }
	else if (ex.typ == L"vec")
	{
		vector<ptr<Var>> tmp;
		for (auto const& e : ex.vec)
		{
			tmp.push_back(msh<Var>(*Evaluate(gl, sc, e)));
		}
		return msh<Var>(tmp);
	}
	else if (ex.typ == L"access") { return EvaluateAccess(gl, sc, ex); }
	return msh<Var>();
}

ptr<Var> Execute(bool& ret, Scope& gl, Scope& sc, Program const& ps)
{
	for (auto const& p : ps)
	{
		if (p.typ == L"single") { Evaluate(gl, sc, p.single); }
		else if (p.typ == L"return") { ret = true; return Evaluate(gl, sc, p.ret); }
		else if (p.typ == L"define")
		{
			*FindVariable(gl, sc, p.def_fun, false) = Var(p.procs, p.def_pars);
		}
		else if (p.typ == L"assign") { *Evaluate(gl, sc, p.left) = *Evaluate(gl, sc, p.right); }
		else if (p.typ == L"if")
		{
			auto& ps = Evaluate(gl, sc, p.if_con)->num ? p.if_procs : p.else_procs;
			auto v = Execute(ret, gl, sc, ps);
			if (ret) { return v; }
		}
		else if (p.typ == L"while")
		{
			while (Evaluate(gl, sc, p.lp_con)->num)
			{
				auto v = Execute(ret, sc, gl, p.lp);
				if (ret) { return v; }
			}
		}
	}
	return msh<Var>();
}
ptr<Var> Execute(Scope& gl, Program const& ps)
{
	bool ret = false;
	return Execute(ret, gl, gl, ps);
}
Program Compile(wstring const& code)
{
	return ParseStatements(AnalyzeBrackets(Tokenize(code)));
}

