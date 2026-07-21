#pragma once
#include "Token.h"

struct BracketGroup {
	int typ = 0;
	// 1 代表 ()，2 代表 {}, 3 代表 []。
	Token tk;
	vector<BracketGroup> bras;
};
wstring ToWString(BracketGroup const& b);
vector<BracketGroup> AnalyzeBrackets(vector<Token> const& in);

