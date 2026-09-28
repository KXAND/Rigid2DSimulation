#pragma once
#include "Utils.h"

struct Token
{
    wstring type;
    double number = 0;
    wstring stringValue;
    wstring variableName;
    bool isGlobal = false;

    Token() = default;
    Token(wstring const& type);
    Token(double number);
    static Token CreateString(wstring const& text);
    static Token CreateVariable(wstring const& value, bool isGlobal);
};
wstring ToWString(Token const& token);

Token ParseNumberToken(wstring const& source, int& index);
Token ParseStringToken(wstring const& source, int& index);
Token ParseMarkToken(wstring const& source, int& index);
Token ParseGlobalVariableToken(wstring const& source, int& index);
Token ParseVariableOrReservedToken(wstring const& source, int& index);
Token ParseToken(wstring const& source, int& index);

vector<Token> NormalizeSigns(vector<Token> const& tokens);
vector<Token> Tokenize(wstring const& source);
