#pragma once
#include "script_expr.h"

struct ScriptStatement {
	wstring typ = L"empty";
	// assign, single, define, return, if, while
	ScriptExpr ret;
	ScriptExpr single;
	ScriptExpr left, right;

	wstring def_fun;
	vector<ScriptStatement> procs;
	vector<wstring> def_pars;

	ScriptExpr lp_con;
	vector<ScriptStatement> lp;
	ScriptExpr if_con;
	vector<ScriptStatement> if_procs, else_procs;

	ScriptStatement() = default;
	ScriptStatement(wstring const& typ);
};
wstring tw(ScriptStatement const& p);

ScriptStatement parsing_def(vector<BracketGroup> const& in);
ScriptStatement parsing_ret(vector<BracketGroup> const& in);
ScriptStatement parsing_assign(vector<BracketGroup> const& in);
ScriptStatement parsing_if(vector<BracketGroup> const& in);
ScriptStatement parsing_while(vector<BracketGroup> const& in);

ScriptStatement parsing_proc(vector<BracketGroup> const& in);
vector<ScriptStatement> parsing_procs(vector<BracketGroup> const& in);

