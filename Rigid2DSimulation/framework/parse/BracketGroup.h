#pragma once
#include "Token.h"

struct BracketGroup {
	int type = 0;
	// 1 代表 ()，2 代表 {}, 3 代表 []。
	Token token;
	vector<BracketGroup> children;
};
wstring ToWString(BracketGroup const& group);
vector<BracketGroup> AnalyzeBrackets(vector<Token> const& tokens);

