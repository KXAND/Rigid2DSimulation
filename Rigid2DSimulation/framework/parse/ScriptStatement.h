#pragma once
#include "ScriptExpression.h"

struct ScriptStatement
{
    wstring type = L"empty";
    // assign, single, define, return, if, while
    ScriptExpression returnExpression;
    ScriptExpression expression;
    ScriptExpression leftExpression;
    ScriptExpression rightExpression;

    wstring functionName;
    vector<ScriptStatement> bodyStatements;
    vector<wstring> parameterNames;

    ScriptExpression loopCondition;
    vector<ScriptStatement> loopStatements;
    ScriptExpression condition;
    vector<ScriptStatement> thenStatements;
    vector<ScriptStatement> elseStatements;

    ScriptStatement() = default;
    ScriptStatement(wstring const& type);
};
wstring ToWString(ScriptStatement const& statement);

ScriptStatement ParseDefinition(vector<BracketGroup> const& groups);
ScriptStatement ParseReturn(vector<BracketGroup> const& groups);
ScriptStatement ParseAssignment(vector<BracketGroup> const& groups);
ScriptStatement ParseIf(vector<BracketGroup> const& groups);
ScriptStatement ParseWhile(vector<BracketGroup> const& groups);

ScriptStatement ParseStatement(vector<BracketGroup> const& groups);
vector<ScriptStatement> ParseStatements(vector<BracketGroup> const& groups);

