#pragma once
#include "Utils.h"

struct Token {
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
wstring ToWString(Token const& tk);

Token ParseNumberToken(wstring const& in, int& id);
Token ParseStringToken(wstring const& in, int& id);
Token ParseMarkToken(wstring const& in, int& id);
Token ParseGlobalVariableToken(wstring const& in, int& id);
Token ParseVariableOrReservedToken(wstring const& in, int& id);
Token ParseToken(wstring const& in, int& id);

vector<Token> NormalizeSigns(vector<Token> const& in);
vector<Token> Tokenize(wstring const& in);

