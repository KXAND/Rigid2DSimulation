#pragma once
#include "dvec.h"

struct AppInput {
	double msw = 0;
	wstring chs;
	dVector2 msp, msp_old;
	array<BYTE, 3> msd{}, msd_old{};
	array<int, 256> kbmsg{};
	array<BYTE, 256> kbd{}, kbd_old{};
	// ����˵������, �ڴ����ƶ�ʱ����ʹ������ͣ��
	bool isBbarClicked = false;

	// �Ҿ����������߼����ǱȽ���֣������ж���֮����
	// �Ժ�Ҫ��һ�£�����ע��������ֲ�����Ĳ��ö��������ɵġ�
	bool mouse_owned = false;
	bool mouse_tracked = false;
	TRACKMOUSEEVENT mouse_track{};

	bool isMouseClickedDown(int m) { return msd[m] && !msd_old[m]; }
	bool isKeyPressedDown(int k) { return kbd[k] && !kbd_old[k]; }

	void track_mouse();
	bool sync_mouse(HWND hwnd);

	void init(HWND hwnd);
	void reset();
};

struct app_fps {
	int frm_cnt = 0;
	int frm_cnt_max = 0;
	long long ns_cnt = 0;
	double fps = 0, frm_time = 0;
	long long real_dt = 0, real_dur = 0;
	std::chrono::high_resolution_clock::time_point tm;

	app_fps();
	void run();
	void update();

	double dt() const { return real_dt / 1e9; }
	double dur() const { return real_dur / 1e9; }
};

struct app_owners {
	void* keyboardOwner = NULL;
	void* hovered = NULL;
	void* wheeled = NULL;
	double hoveredDepth = -DBL_MAX, wheeledDepth = -DBL_MAX;

	void reset();
	void remove(void* o);
	void free_kb(void* o);
};
