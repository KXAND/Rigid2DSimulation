#include "Value.h"
#include "BracketGroup.h"
#include "Vector3.h"
#include "ScriptStatement.h"

// 每次都能找到新 bug，很可能还有一些 bug。0__
// 每次都能找到新 bug，很可能还有一些 bug。1__

Value::Value(double number) : number(number)
{
    type = L"num";
}

Value::Value(wstring const& text) : stringValue(text)
{
    type = L"ToString";
}

Value::Value(Function const& nativeFunction) : nativeFunction(nativeFunction)
{
    type = L"user";
}

Value::Value(vector<std::shared_ptr<Value>> const& elements) : elements(elements)
{
    type = L"vec";
}

Value::Value(Program const& statements, vector<wstring> const& arguments) : bodyStatements(statements),
                                                                            arguments(arguments)
{
    type = L"fun";
}

Vector2 ToVector2(Value const& value)
{
    Vector2 result;
    if (value.elements.size() < 2)
    {
        return {};
    }
    result.x = value.elements[0]->number;
    result.y = value.elements[1]->number;
    return result;
}

Vector3 ToVector3(Value const& value)
{
    Vector3 result;
    if (value.elements.size() < 3)
    {
        return {};
    }
    result.x = value.elements[0]->number;
    result.y = value.elements[1]->number;
    result.z = value.elements[2]->number;
    return result;
}

std::shared_ptr<Value> FindVariable(Scope& globalScope, Scope& scope, wstring variableName, bool isGlobal)
{
    if (isGlobal)
    {
        if (globalScope.find(variableName) == globalScope.end())
        {
            globalScope[variableName] = std::make_shared<Value>();
        }
        return globalScope[variableName];
    }
    else
    {
        if (scope.find(variableName) == scope.end())
        {
            scope[variableName] = std::make_shared<Value>();
        }
        return scope[variableName];
    }
}

bool TryEvaluateDefaultFunction(Scope& globalScope, Scope& scope, ScriptExpression const& expression, Value& result)
{
    auto const& functionName = expression.functionName;
    // 每次都能找到新 bug，很可能还有一些 bug。2__
    auto numArg = [&](int index)
    {
        return Evaluate(globalScope, scope, expression.arguments[index])->number;
    };
    auto stringArg = [&](int index) -> wstring const&
    {
        return Evaluate(globalScope, scope, expression.arguments[index])->stringValue;
    };

    if (functionName == L"!")
    {
        result = Value(!numArg(0));
        return true;
    }
    if (functionName == L"++")
    {
        result = Value(numArg(0));
        return true;
    }
    if (functionName == L"--")
    {
        result = Value(-numArg(0));
        return true;
    }
    if (functionName == L"Sin")
    {
        result = Value(sin(numArg(0)));
        return true;
    }
    if (functionName == L"Cos")
    {
        result = Value(cos(numArg(0)));
        return true;
    }
    if (functionName == L"Abs")
    {
        result = Value(abs(numArg(0)));
        return true;
    }
    if (functionName == L"+")
    {
        result = Value(numArg(0) + numArg(1));
        return true;
    }
    if (functionName == L"-")
    {
        result = Value(numArg(0) - numArg(1));
        return true;
    }
    if (functionName == L"*")
    {
        result = Value(numArg(0) * numArg(1));
        return true;
    }
    if (functionName == L"/")
    {
        result = Value(numArg(0) / numArg(1));
        return true;
    }
    if (functionName == L"<")
    {
        result = Value(numArg(0) < numArg(1));
        return true;
    }
    if (functionName == L">")
    {
        result = Value(numArg(0) > numArg(1));
        return true;
    }
    if (functionName == L"<=")
    {
        result = Value(numArg(0) <= numArg(1));
        return true;
    }
    if (functionName == L">=")
    {
        result = Value(numArg(0) >= numArg(1));
        return true;
    }
    if (functionName == L"==")
    {
        result = Value(numArg(0) == numArg(1));
        return true;
    }
    if (functionName == L"!=")
    {
        result = Value(numArg(0) != numArg(1));
        return true;
    }
    if (functionName == L"&&")
    {
        result = Value(numArg(0) && numArg(1));
        return true;
    }
    if (functionName == L"||")
    {
        result = Value(numArg(0) || numArg(1));
        return true;
    }
    if (functionName == L"Pow")
    {
        result = Value(pow(numArg(0), numArg(1)));
        return true;
    }
    if (functionName == L"Random")
    {
        result = Value(GenerateRandomFloat(numArg(0), numArg(1)));
        return true;
    }
    if (functionName == L"RandomInt")
    {
        result = Value(GenerateRandomInt(numArg(0), numArg(1)));
        return true;
    }
    if (functionName == L"ToString")
    {
        result = Value(FormatFixed2(numArg(0)));
        return true;
    }
    if (functionName == L".")
    {
        result = Value(stringArg(0) + stringArg(1));
        return true;
    }
    return false;
}

std::shared_ptr<Value> EvaluateCall(Scope& globalScope, Scope& scope, ScriptExpression const& expression)
{
    Value defaultValue;
    Scope localScope;
    if (TryEvaluateDefaultFunction(globalScope, scope, expression, defaultValue))
    {
        return std::make_shared<Value>(defaultValue);
    }

    // 每次都能找到新 bug，很可能还有一些 bug。3__
    auto functionValue = FindVariable(globalScope, scope, expression.functionName, expression.isGlobal);
    if (functionValue->type == L"user" && functionValue->nativeFunction)
    {
        vector<std::shared_ptr<Value>> argumentValues;
        for (auto const& argument : expression.arguments)
        {
            argumentValues.push_back(Evaluate(globalScope, scope, argument));
        }
        return functionValue->nativeFunction(argumentValues);
    }

    int argumentCount = int(min(functionValue->arguments.size(), expression.arguments.size()));
    for (int argumentIndex = 0; argumentIndex < argumentCount; ++argumentIndex)
    {
        localScope[functionValue->arguments[argumentIndex]] = Evaluate(globalScope, scope, expression.arguments[argumentIndex]);
    }
    bool hasReturned = false;
    return Execute(hasReturned, globalScope, localScope, functionValue->bodyStatements);
}

std::shared_ptr<Value> EvaluateAccess(Scope& globalScope, Scope& scope, ScriptExpression const& expression)
{
    if (!expression.accessedExpression || !expression.indexExpression)
    {
        return std::make_shared<Value>();
    }
    auto accessedValue = Evaluate(globalScope, scope, *expression.accessedExpression);
    auto indexValue = Evaluate(globalScope, scope, *expression.indexExpression);

    if (indexValue->type == L"num")
    {
        if (accessedValue->type != L"vec")
        {
            *accessedValue = {};
            accessedValue->type = L"vec";
        }
        int targetIndex = int(indexValue->number);
        for (int elementIndex = int(accessedValue->elements.size() - 1); elementIndex < targetIndex; ++elementIndex)
        {
            accessedValue->elements.push_back(std::make_shared<Value>());
        }
        return accessedValue->elements[indexValue->number];
    }

    if (indexValue->type == L"ToString")
    {
        if (accessedValue->type != L"dic")
        {
            *accessedValue = {};
            accessedValue->type = L"dic";
        }
        if (accessedValue->dictionary.find(indexValue->stringValue) == accessedValue->dictionary.end())
        {
            accessedValue->dictionary[indexValue->stringValue] = std::make_shared<Value>();
        }
        return accessedValue->dictionary[indexValue->stringValue];
    }

    return std::make_shared<Value>();
}

std::shared_ptr<Value> Evaluate(Scope& globalScope, Scope& scope, ScriptExpression const& expression)
{
    if (expression.type == L"call")
    {
        return EvaluateCall(globalScope, scope, expression);
    }
    else if (expression.type == L"var")
    {
        if (expression.variableName == L"kPi")
        {
            return std::make_shared<Value>(kPi);
        }
        return FindVariable(globalScope, scope, expression.variableName, expression.isGlobal);
    }
    else if (expression.type == L"num")
    {
        return std::make_shared<Value>(expression.number);
    }
    else if (expression.type == L"ToString")
    {
        return std::make_shared<Value>(expression.stringValue);
    }
    else if (expression.type == L"vec")
    {
        vector<std::shared_ptr<Value>> elementValues;
        for (auto const& element : expression.elements)
        {
            elementValues.push_back(std::make_shared<Value>(*Evaluate(globalScope, scope, element)));
        }
        return std::make_shared<Value>(elementValues);
    }
    else if (expression.type == L"access")
    {
        return EvaluateAccess(globalScope, scope, expression);
    }
    return std::make_shared<Value>();
}

std::shared_ptr<Value> Execute(bool& hasReturned, Scope& globalScope, Scope& scope, Program const& statements)
{
    for (auto const& statement : statements)
    {
        if (statement.type == L"single")
        {
            Evaluate(globalScope, scope, statement.expression);
        }
        else if (statement.type == L"return")
        {
            hasReturned = true;
            return Evaluate(globalScope, scope, statement.returnExpression);
        }
        else if (statement.type == L"define")
        {
            *FindVariable(globalScope, scope, statement.functionName, false) = Value(statement.bodyStatements, statement.parameterNames);
        }
        else if (statement.type == L"assign")
        {
            *Evaluate(globalScope, scope, statement.leftExpression) = *Evaluate(globalScope, scope, statement.rightExpression);
        }
        else if (statement.type == L"if")
        {
            auto& selectedStatements = Evaluate(globalScope, scope, statement.condition)->number ? statement.thenStatements : statement.elseStatements;
            auto returnValue = Execute(hasReturned, globalScope, scope, selectedStatements);
            if (hasReturned)
            {
                return returnValue;
            }
        }
        else if (statement.type == L"while")
        {
            while (Evaluate(globalScope, scope, statement.loopCondition)->number)
            {
                auto returnValue = Execute(hasReturned, scope, globalScope, statement.loopStatements);
                if (hasReturned)
                {
                    return returnValue;
                }
            }
        }
    }
    return std::make_shared<Value>();
}

std::shared_ptr<Value> Execute(Scope& globalScope, Program const& statements)
{
    bool hasReturned = false;
    return Execute(hasReturned, globalScope, globalScope, statements);
}

Program Compile(wstring const& code)
{
    return ParseStatements(AnalyzeBrackets(Tokenize(code)));
}
