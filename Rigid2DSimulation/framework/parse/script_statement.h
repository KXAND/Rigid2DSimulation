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
wstring ToWString(ScriptStatement const& p);

ScriptStatement ParseDefinition(vector<BracketGroup> const& in);
ScriptStatement ParseReturn(vector<BracketGroup> const& in);
ScriptStatement ParseAssignment(vector<BracketGroup> const& in);
ScriptStatement ParseIf(vector<BracketGroup> const& in);
ScriptStatement ParseWhile(vector<BracketGroup> const& in);

ScriptStatement ParseStatement(vector<BracketGroup> const& in);
vector<ScriptStatement> ParseStatements(vector<BracketGroup> const& in);

