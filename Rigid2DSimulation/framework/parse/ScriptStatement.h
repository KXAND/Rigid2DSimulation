#pragma once
#include "ScriptExpression.h"

struct ScriptStatement {
	wstring type = L"empty";
	// assign, single, define, return, if, while
	ScriptExpression ret;
	ScriptExpression single;
	ScriptExpression left, right;

	wstring def_fun;
	vector<ScriptStatement> procs;
	vector<wstring> def_pars;

	ScriptExpression lp_con;
	vector<ScriptStatement> lp;
	ScriptExpression if_con;
	vector<ScriptStatement> if_procs, else_procs;

	ScriptStatement() = default;
	ScriptStatement(wstring const& type);
};
wstring ToWString(ScriptStatement const& p);

ScriptStatement ParseDefinition(vector<BracketGroup> const& in);
ScriptStatement ParseReturn(vector<BracketGroup> const& in);
ScriptStatement ParseAssignment(vector<BracketGroup> const& in);
ScriptStatement ParseIf(vector<BracketGroup> const& in);
ScriptStatement ParseWhile(vector<BracketGroup> const& in);

ScriptStatement ParseStatement(vector<BracketGroup> const& in);
vector<ScriptStatement> ParseStatements(vector<BracketGroup> const& in);

