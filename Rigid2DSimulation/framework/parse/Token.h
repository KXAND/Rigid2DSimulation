#pragma once
#include "Utils.h"

struct token {
	wstring typ;
	double num = 0;
	wstring str;
	wstring var;
	bool global = false;

	token() = default;
	token(wstring const& typ);
	token(double num);
	static token mkstr(wstring const& str);
	static token mkvar(wstring const& var, bool gl);
};
wstring ToWString(token const& tk);

token ParseNumberToken(wstring const& in, int& id);
token ParseStringToken(wstring const& in, int& id);
token ParseMarkToken(wstring const& in, int& id);
token ParseGlobalVariableToken(wstring const& in, int& id);
token ParseVariableOrReservedToken(wstring const& in, int& id);
token ParseToken(wstring const& in, int& id);

vector<token> NormalizeSigns(vector<token> const& in);
vector<token> Tokenize(wstring const& in);

