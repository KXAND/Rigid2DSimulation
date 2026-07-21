#pragma once
#include "ValuePtr.h"
#include "BracketGroup.h"

// 这里空间可能占的太大了，运行时多态可以解决。但是懒得麻烦了。
struct ScriptExpression {
    wstring type = L"null";
    // vec 和 access 其实都可以放到 call 的框架里。
    // 但是实现的时候我还是希望分开来，虽然也没啥理由就是了。
    // num, var, str, call, null, vec, access

    bool isGlobal = false;
    double number = 0;
    wstring stringValue;
    wstring variableName;
    wstring functionName;
    vector<ScriptExpression> arguments;
    vector<ScriptExpression> elements;
    ValuePtr<ScriptExpression> accessedExpression, indexExpression;

    ScriptExpression() = default;
    ScriptExpression(wstring const& type);
    ScriptExpression(double number);
    ScriptExpression(wstring const& functionName, vector<ScriptExpression> const& arguments);
    static ScriptExpression CreateString(wstring const& text);
    static ScriptExpression CreateVariable(wstring const& value, bool isGlobal);
};
wstring ToWString(ScriptExpression const& expression);

struct PreExpression {
    bool isToken = false;
    ScriptExpression expression;
    Token token;

    PreExpression() = default;
    PreExpression(ScriptExpression const& expression);
    PreExpression(Token const& token);
};

bool IsUnaryOperator(Token const& token);
bool IsBinaryOperator(Token const& token);

bool ParseNegation(vector<PreExpression> const& preExpressions, ScriptExpression& result);
bool ParseBinary(vector<PreExpression> const& preExpressions, ScriptExpression& result, vector<wstring> const& operatorTypes);
ScriptExpression ParsePreExpression(vector<PreExpression> const& preExpressions);

ScriptExpression ParseCall(vector<BracketGroup> const& groups);
ScriptExpression ParseBinaryExpressions(vector<BracketGroup> const& groups);
ScriptExpression ParseVector(vector<BracketGroup> const& groups);
ScriptExpression ParseAccess(vector<BracketGroup> const& groups);
ScriptExpression ParseExpression(vector<BracketGroup> const& groups);

