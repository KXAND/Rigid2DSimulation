#pragma once
#include "token.h"

struct BracketGroup {
	int typ = 0;
	// 1 代表 ()，2 代表 {}, 3 代表 []。
	token tk;
	vector<BracketGroup> bras;
};
wstring tw(BracketGroup const& b);
vector<BracketGroup> bra_analysis(vector<token> const& in);

