#include "ScriptExpression.h"
#include "BracketGroup.h"

ScriptExpression::ScriptExpression(wstring const& type) : type(type)
{
}

ScriptExpression::ScriptExpression(double number) : number(number)
{
    type = L"num";
}

ScriptExpression::ScriptExpression(wstring const& functionName, vector<ScriptExpression> const& arguments) :
    functionName(functionName),
    arguments(arguments)
{
    type = L"call";
}

ScriptExpression ScriptExpression::CreateString(wstring const& text)
{
    auto expression = ScriptExpression(L"str");
    expression.stringValue = text;
    return expression;
}

ScriptExpression ScriptExpression::CreateVariable(wstring const& value, bool isGlobal)
{
    auto expression = ScriptExpression(L"var");
    expression.variableName = value;
    expression.isGlobal = isGlobal;
    return expression;
}

wstring ToWString(ScriptExpression const& expression)
{
    wstring serializedText = L"[" + expression.type;
    if (expression.type == L"var")
    {
        serializedText += (expression.isGlobal ? L":&" : L":") + expression.variableName;
    }
    else if (expression.type == L"num")
    {
        serializedText += L":" + FormatFixed2(expression.number);
    }
    else if (expression.type == L"str")
    {
        serializedText += L":" + expression.stringValue;
    }
    else if (expression.type == L"access")
    {
        if (!expression.accessedExpression || !expression.indexExpression)
        {
            return {};
        }
        serializedText += L":" + ToWString(*expression.accessedExpression) + L"," + ToWString(*expression.indexExpression);
    }
    else if (expression.type == L"vec")
    {
        serializedText += L":";
        for (auto const& element : expression.elements)
        {
            serializedText += ToWString(element) + L",";
        }
    }
    else if (expression.type == L"call")
    {
        serializedText += L":" + expression.functionName + L",";
        for (auto const& argument : expression.arguments)
        {
            serializedText += ToWString(argument) + L",";
        }
    }
    serializedText += L"]";
    return serializedText;
}

PreExpression::PreExpression(ScriptExpression const& expression) : expression(expression)
{
}

PreExpression::PreExpression(Token const& token) : token(token)
{
    isToken = true;
}

bool IsUnaryOperator(Token const& token)
{
    // 由于 +，- 的特殊性，它们都被当成二元运算符处理。
    if (token.type.empty())
    {
        return false;
    }
    auto const& operatorType = token.type;
    return operatorType == L"!";
}

bool IsBinaryOperator(Token const& token)
{
    if (token.type.empty())
    {
        return false;
    }
    return !iswalnum(token.type[0]) && !IsUnaryOperator(token);
}

bool ParseNegation(vector<PreExpression> const& preExpressions, ScriptExpression& result)
{
    int expressionCount = int(preExpressions.size());
    for (int operatorIndex = 0; operatorIndex < expressionCount; ++operatorIndex)
    {
        auto const& currentItem = preExpressions[operatorIndex];
        if (!currentItem.isToken)
        {
            continue;
        }
        auto const& operatorType = currentItem.token.type;

        if ((operatorType == L"+" || operatorType == L"-") && (operatorIndex == 0 || preExpressions[operatorIndex - 1].isToken))
        {
            bool isInvalid = operatorIndex == preExpressions.size() - 1 || preExpressions[operatorIndex + 1].isToken;
            if (isInvalid)
            {
                result = ScriptExpression(L"error");
                return false;
            }

            auto reducedExpressions = vector<PreExpression>(preExpressions.begin(), preExpressions.begin() + operatorIndex);
            reducedExpressions.push_back(ScriptExpression(operatorType + operatorType, { preExpressions[operatorIndex + 1].expression }));
            int tailCount = int(preExpressions.size());
            for (int copyIndex = operatorIndex + 2; copyIndex < tailCount; ++copyIndex)
            {
                reducedExpressions.push_back(preExpressions[copyIndex]);
            }
            result = ParsePreExpression(reducedExpressions);
            return true;
        }
    }
    return false;
}

bool ParseBinary(vector<PreExpression> const& preExpressions, ScriptExpression& result, vector<wstring> const& operatorTypes)
{
    int expressionCount = int(preExpressions.size());
    for (int operatorIndex = 0; operatorIndex < expressionCount; ++operatorIndex)
    {
        auto const& currentItem = preExpressions[operatorIndex];
        if (!currentItem.isToken)
        {
            continue;
        }
        auto const& operatorType = currentItem.token.type;

        if (find(operatorTypes.begin(), operatorTypes.end(), operatorType) != operatorTypes.end())
        {
            bool isInvalid =
                operatorIndex == 0 || operatorIndex == preExpressions.size() - 1 ||
                preExpressions[operatorIndex - 1].isToken || preExpressions[operatorIndex + 1].isToken;
            if (isInvalid)
            {
                result = ScriptExpression(L"error");
                return false;
            }

            auto reducedExpressions = vector<PreExpression>(preExpressions.begin(), preExpressions.begin() + operatorIndex - 1);
            reducedExpressions.push_back(ScriptExpression(operatorType, { preExpressions[operatorIndex - 1].expression, preExpressions[operatorIndex + 1].expression }));
            int tailCount = int(preExpressions.size());
            for (int copyIndex = operatorIndex + 2; copyIndex < tailCount; ++copyIndex)
            {
                reducedExpressions.push_back(preExpressions[copyIndex]);
            }
            result = ParsePreExpression(reducedExpressions);
            return true;
        }
    }
    return false;
}

ScriptExpression ParsePreExpression(vector<PreExpression> const& preExpressions)
{
    if (preExpressions.empty())
    {
        return ScriptExpression(L"error");
    }
    if (preExpressions.size() == 1)
    {
        if (preExpressions[0].isToken)
        {
            return ScriptExpression(L"error");
        }
        return preExpressions[0].expression;
    }

    ScriptExpression result;
    if (ParseNegation(preExpressions, result))
    {
        return result;
    }
    if (ParseBinary(preExpressions, result, { L"*", L"/" }))
    {
        return result;
    }
    if (ParseBinary(preExpressions, result, { L"+", L"-" }))
    {
        return result;
    }
    if (ParseBinary(preExpressions, result, { L"==", L"!=", L"<=", L">=", L"<", L">" }))
    {
        return result;
    }
    if (ParseBinary(preExpressions, result, { L"&&", L"||" }))
    {
        return result;
    }
    if (ParseBinary(preExpressions, result, { L"." }))
    {
        return result;
    }
    return ScriptExpression(L"error");
}

ScriptExpression ParseCall(vector<BracketGroup> const& groups)
{
    ScriptExpression expression(L"call");
    expression.functionName = groups[0].token.variableName;
    expression.isGlobal = groups[0].token.isGlobal;

    vector<BracketGroup> argumentGroups;
    for (auto const& group : groups[1].children)
    {
        if (group.token.type == L",")
        {
            expression.arguments.push_back(ParseExpression(argumentGroups));
            argumentGroups.clear();
        }
        else
        {
            argumentGroups.push_back(group);
        }
    }
    if (!argumentGroups.empty())
    {
        expression.arguments.push_back(ParseExpression(argumentGroups));
    }
    return expression;
}

ScriptExpression ParseBinaryExpressions(vector<BracketGroup> const& groups)
{
    vector<BracketGroup> expressionGroups;
    vector<PreExpression> preExpressions;
    for (auto const& group : groups)
    {
        if (IsBinaryOperator(group.token))
        {
            if (!expressionGroups.empty())
            {
                preExpressions.push_back(ParseExpression(expressionGroups));
                expressionGroups.clear();
            }
            preExpressions.push_back(group.token);
        }
        else
        {
            expressionGroups.push_back(group);
        }
    }
    if (!expressionGroups.empty())
    {
        preExpressions.push_back(ParseExpression(expressionGroups));
    }
    return ParsePreExpression(preExpressions);
}

ScriptExpression ParseVector(vector<BracketGroup> const& groups)
{
    ScriptExpression expression(L"vec");
    vector<BracketGroup> elementGroups;
    for (auto const& group : groups[0].children)
    {
        if (group.token.type == L",")
        {
            expression.elements.push_back(ParseExpression(elementGroups));
            elementGroups.clear();
        }
        else
        {
            elementGroups.push_back(group);
        }
    }
    if (!elementGroups.empty())
    {
        expression.elements.push_back(ParseExpression(elementGroups));
    }
    return expression;
}

ScriptExpression ParseAccess(vector<BracketGroup> const& groups)
{
    ScriptExpression expression(L"access");
    expression.accessedExpression = ParseExpression(vector<BracketGroup>(groups.begin(), groups.end() - 1));
    expression.indexExpression = ParseExpression(groups.back().children);
    return expression;
}

ScriptExpression ParseExpression(vector<BracketGroup> const& groups)
{
    if (groups.empty())
    {
        return ScriptExpression(L"error");
    }
    auto const& firstGroup = groups[0];
    if (groups.size() == 1)
    {
        if (firstGroup.type == 0)
        {
            if (firstGroup.token.type == L"var")
            {
                return ScriptExpression::CreateVariable(firstGroup.token.variableName, firstGroup.token.isGlobal);
            }
            else if (firstGroup.token.type == L"str")
            {
                return ScriptExpression::CreateString(firstGroup.token.stringValue);
            }
            else if (firstGroup.token.type == L"num")
            {
                return ScriptExpression(firstGroup.token.number);
            }
            else if (firstGroup.token.type == L"null")
            {
                return ScriptExpression(L"null");
            }
            else
            {
                return ScriptExpression(L"error");
            }
        }
        else if (firstGroup.type == 1)
        {
            return ParseExpression(firstGroup.children);
        }
        else if (firstGroup.type == 3)
        {
            return ParseVector(groups);
        }
        else
        {
            return ScriptExpression(L"error");
        }
    }

    bool isCall = groups.size() == 2 && groups[0].token.type == L"var" && groups[1].type == 1;
    if (isCall)
    {
        return ParseCall(groups);
    }

    bool isUnary = groups.size() == 2 && IsUnaryOperator(firstGroup.token);
    if (isUnary)
    {
        return ScriptExpression(firstGroup.token.type, { ParseExpression({ groups[1] }) });
    }

    for (auto const& group : groups)
    {
        if (IsBinaryOperator(group.token))
        {
            return ParseBinaryExpressions(groups);
        }
    }

    if (groups.back().type == 3)
    {
        return ParseAccess(groups);
    }
    return ScriptExpression(L"error");
}
