#pragma once
#include "token.h"

struct bra {
	int typ = 0;
	// 1 ���� ()��2 ���� {}, 3 ���� []��
	token tk;
	vector<bra> bras;
};
wstring tw(bra const& b);
vector<bra> bra_analysis(vector<token> const& in);

