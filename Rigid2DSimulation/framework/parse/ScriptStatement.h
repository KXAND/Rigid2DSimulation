#pragma once
#include "ScriptExpression.h"

struct ScriptStatement {
	wstring type = L"empty";
	// assign, single, define, return, if, while
	ScriptExpression returnExpression;
	ScriptExpression expression;
	ScriptExpression leftExpression, rightExpression;

	wstring functionName;
	vector<ScriptStatement> bodyStatements;
	vector<wstring> parameterNames;

	ScriptExpression loopCondition;
	vector<ScriptStatement> loopStatements;
	ScriptExpression condition;
	vector<ScriptStatement> thenStatements, elseStatements;

	ScriptStatement() = default;
	ScriptStatement(wstring const& type);
};
wstring ToWString(ScriptStatement const& statement);

ScriptStatement ParseDefinition(vector<BracketGroup> const& in);
ScriptStatement ParseReturn(vector<BracketGroup> const& in);
ScriptStatement ParseAssignment(vector<BracketGroup> const& in);
ScriptStatement ParseIf(vector<BracketGroup> const& in);
ScriptStatement ParseWhile(vector<BracketGroup> const& in);

ScriptStatement ParseStatement(vector<BracketGroup> const& in);
vector<ScriptStatement> ParseStatements(vector<BracketGroup> const& in);

