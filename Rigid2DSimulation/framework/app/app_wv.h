#pragma once
#include "utils.h"

// ���ﷴ�����ã���֪����û�и��õ�ʵ�ַ�ʽ��
struct wvblk {
	WAVEHDR hdr{};
	vector<int16_t> dat;

	wvblk(int n = 0);
	int n() const;
	bool done() const;
	bool play(HWAVEOUT hwo);
	bool finish(HWAVEOUT hwo);
};

struct app_wv {
	HWAVEOUT hwo{};
	int sp = 0;
	int n_safe = 0;
	int n_blocks = 0;
	double len_block = 0;
	WAVEFORMATEX wofm{};

	int n_play = 0;
	vector<double> wvin;
	// ʵ������������ָ���Ǳ�Ҫ��, �����漰�����ƻ��д��󣬿��ܸ� winapi �йء�
	deque<ptr<wvblk>> wvblks;

	app_wv();
	int len_block_n() const;
	bool init();
	bool play();
	void exit();
};

