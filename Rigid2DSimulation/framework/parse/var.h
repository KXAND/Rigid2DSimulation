#pragma once
#include "utils.h"
#include "proc.h"

struct Var;
typedef function<ptr<Var>(vector<ptr<Var>> const&)> Function;
typedef vector<proc> Program;
struct Var {
	// ��ʵ�����Ǻ�ȷ�� typ ��û���ã��������Щ�ҵ���
	wstring typ = L"null";
	// null, num, str, fun, vec, user, dic
	double num = 0;
	wstring str;
	Function fun;
	Program procs;
	vector<wstring> pars;
	vector<ptr<Var>> vec;
	map<wstring, ptr<Var>> dic;

	Var() = default;
	Var(double num);
	Var(wstring const& str);
	Var(Function const& fun);
	Var(vector<ptr<Var>> const& vec);
	Var(Program const& ps, vector<wstring> const& pars);
};

struct vector2;
struct vector3;
vector2 tv2(Var const& v);
vector3 tv3(Var const& v);

typedef map<wstring, ptr<Var>> Scope;
ptr<Var> findvar(Scope& curScope, Scope& sc, wstring nm, bool isgl);
// ���治�ó�ֵ������Ϊ�˼��ٲ���Ҫ�ĸ��ơ�����˵ʵ���Ҳ�֪����û�б�Ҫ��
bool default_fun(Scope& curScope, Scope& sc, expr const& ex, Var& out);

ptr<Var> eval_call(Scope& curScope, Scope& sc, expr const& ex);
ptr<Var> eval_access(Scope& curScope, Scope& sc, expr const& ex);
ptr<Var> eval(Scope& curScope, Scope& sc, expr const& ex);

// ��һ��������һ�������᲻����ִ�й����а��Լ�Ĩȥ��Ҫע����ι�ܵ���
ptr<Var> Execute(bool& ret, Scope& curScope, Scope& sc, Program const& ps);
ptr<Var> Execute(Scope& curScope, Program const& ps);
Program Compile(wstring const& code);

