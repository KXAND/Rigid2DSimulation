#include "ScriptStatement.h"
#include "BracketGroup.h"

ScriptStatement::ScriptStatement(wstring const& type) : type(type)
{
}

wstring ToWString(ScriptStatement const& statement)
{
    wstring serializedText = statement.type;
    if (statement.type == L"define")
    {
        serializedText += L":" + statement.functionName + L",(";
        for (auto& parameterName : statement.parameterNames)
        {
            serializedText += parameterName + L",";
        }
        serializedText += L"),{";
        for (auto& childStatement : statement.bodyStatements)
        {
            serializedText += L"\n" + ToWString(childStatement) + L",";
        }
        serializedText += L"}";
    }
    else if (statement.type == L"if")
    {
        serializedText += L":" + ToWString(statement.condition) + L",{";
        for (auto& childStatement : statement.thenStatements)
        {
            serializedText += L"\n" + ToWString(childStatement) + L",";
        }
        serializedText += L"}{";
        for (auto& childStatement : statement.elseStatements)
        {
            serializedText += L"\n" + ToWString(childStatement) + L",";
        }
        serializedText += L"}";
    }
    else if (statement.type == L"while")
    {
        serializedText += L":" + ToWString(statement.loopCondition) + L",{";
        for (auto& childStatement : statement.loopStatements)
        {
            serializedText += L"\n" + ToWString(childStatement) + L",";
        }
        serializedText += L"}";
    }
    else if (statement.type == L"return")
    {
        serializedText += L":" + ToWString(statement.returnExpression);
    }
    else if (statement.type == L"single")
    {
        serializedText += L":" + ToWString(statement.expression);
    }
    else if (statement.type == L"assign")
    {
        serializedText += L":" + ToWString(statement.leftExpression) + L"," + ToWString(statement.rightExpression);
    }
    return serializedText;
}

ScriptStatement ParseDefinition(vector<BracketGroup> const& groups)
{
    if (groups.size() < 4)
    {
        return ScriptStatement(L"error");
    }
    // 不想让这里的函数和参数也牵扯到 global 里面去，感觉完全不必要。
    if (groups[1].token.type != L"var" || groups[1].token.isGlobal)
    {
        return ScriptStatement(L"error");
    }
    ScriptStatement statement(L"define");
    statement.functionName = groups[1].token.variableName;
    bool comma = true;

    for (auto const& group : groups[2].children)
    {
        if (comma)
        {
            statement.parameterNames.push_back(group.token.variableName);
            if (group.token.type != L"var" || group.token.isGlobal)
            {
                return ScriptStatement(L"error");
            }
        }
        else if (group.token.type != L",")
        {
            return ScriptStatement(L"error");
        }
        comma = !comma;
    }
    statement.bodyStatements = ParseStatements(groups[3].children);
    return statement;
}

ScriptStatement ParseReturn(vector<BracketGroup> const& groups)
{
    ScriptStatement statement(L"return");
    auto expressionGroups = vector<BracketGroup>(groups.begin() + 1, groups.end());
    statement.returnExpression = expressionGroups.empty() ? ScriptExpression(L"null") : ParseExpression(expressionGroups);
    return statement;
}

ScriptStatement ParseAssignment(vector<BracketGroup> const& groups)
{
    ScriptStatement statement(L"assign");
    auto assignmentIterator = groups.begin();
    for (; assignmentIterator != groups.end() && assignmentIterator->token.type != L"="; ++assignmentIterator)
    {
    }
    statement.leftExpression = ParseExpression(vector<BracketGroup>(groups.begin(), assignmentIterator));
    statement.rightExpression = ParseExpression(vector<BracketGroup>(assignmentIterator + 1, groups.end()));
    return statement;
}

ScriptStatement ParseIf(vector<BracketGroup> const& groups)
{
    if (groups.size() < 3)
    {
        return ScriptStatement(L"error");
    }
    if (groups[1].type != 1 || groups[2].type != 2)
    {
        return ScriptStatement(L"error");
    }

    ScriptStatement statement(L"if");
    statement.condition = ParseExpression(groups[1].children);
    statement.thenStatements = ParseStatements(groups[2].children);
    if (groups.size() != 3)
    {
        if (groups.size() != 5)
        {
            return ScriptStatement(L"error");
        }
        if (groups[3].token.type != L"else" || groups[4].type != 2)
        {
            return ScriptStatement(L"error");
        }
        statement.elseStatements = ParseStatements(groups[4].children);
    }
    return statement;
}

ScriptStatement ParseWhile(vector<BracketGroup> const& groups)
{
    if (groups.size() != 3)
    {
        return ScriptStatement(L"error");
    }
    if (groups[1].type != 1 || groups[2].type != 2)
    {
        return ScriptStatement(L"error");
    }
    ScriptStatement statement(L"while");
    statement.loopCondition = ParseExpression(groups[1].children);
    statement.loopStatements = ParseStatements(groups[2].children);
    return statement;
}

ScriptStatement ParseStatement(vector<BracketGroup> const& groups)
{
    if (groups.empty())
    {
        return ScriptStatement(L"error");
    }
    if (groups[0].token.type == L"def")
    {
        return ParseDefinition(groups);
    }
    if (groups[0].token.type == L"return")
    {
        return ParseReturn(groups);
    }
    if (groups[0].token.type == L"if")
    {
        return ParseIf(groups);
    }
    if (groups[0].token.type == L"while")
    {
        return ParseWhile(groups);
    }
    for (auto const& group : groups)
    {
        if (group.token.type == L"=")
        {
            return ParseAssignment(groups);
        }
    }
    ScriptStatement statement(L"single");
    statement.expression = ParseExpression(groups);
    return statement;
}

vector<ScriptStatement> ParseStatements(vector<BracketGroup> const& groups)
{
    vector<BracketGroup> currentGroups;
    vector<ScriptStatement> statements;
    bool isCurlyGroup = false;

    for (auto const& group : groups)
    {
        if (group.token.type == L";")
        {
            if (!currentGroups.empty())
            {
                auto statement = ParseStatement(currentGroups);
                if (statement.type == L"error")
                {
                    return {};
                }
                statements.push_back(statement);
                currentGroups.clear();
            }
        }
        else
        {
            if (group.token.type != L"else" && isCurlyGroup)
            {
                auto statement = ParseStatement(currentGroups);
                if (statement.type == L"error")
                {
                    return {};
                }
                statements.push_back(statement);
                currentGroups.clear();
            }
            currentGroups.push_back(group);
        }
        isCurlyGroup = group.type == 2;
    }

    if (isCurlyGroup)
    {
        auto statement = ParseStatement(currentGroups);
        if (statement.type == L"error")
        {
            return {};
        }
        statements.push_back(statement);
    }
    else if (!currentGroups.empty())
    {
        return {};
    }
    return statements;
}
