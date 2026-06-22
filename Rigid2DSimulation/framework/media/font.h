#pragma once
#include "tile.h"

wchar_t extern CHINESE_CHARS[];
wchar_t extern NUMBERS[];
wchar_t extern NORMAL_SYMBOLS[];
wchar_t extern UPPERS[];
wchar_t extern LOWERS[];
wchar_t extern ADDITIONAL_CHINESE_CHARS[];

// ���������񣺻�����600x600����ϸ��40�������60x60��
// ������ʷԭ����ɫһ����: (0,255,0)��
struct font {
	int h = 0;
	wstring chi;
	tile missing;
	map<wchar_t, tile> fts;

	font();
	font(int h);

	tile const& operator[](wchar_t c) const;
	// �ϸ���˵�õ����ķ������ã����������ܷ��㡣
	wstring rnd(int n) const;

	bool load_font(wstring const& name, wchar_t c);
	bool init();
};

