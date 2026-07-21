#pragma once
#include "Utils.h"

struct Token {
	wstring typ;
	double num = 0;
	wstring str;
	wstring Value;
	bool global = false;

	Token() = default;
	Token(wstring const& typ);
	Token(double num);
	static Token mkstr(wstring const& str);
	static Token mkvar(wstring const& Value, bool gl);
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

