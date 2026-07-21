#include "AppItem.h"

void AppInput::TrackMouse() {
	if (!mouse_tracked) {
		mouse_tracked = true;
		TrackMouseEvent(&mouse_track);
	}
}
bool AppInput::SyncMouse(HWND hwnd) {
	POINT pnt{}; BOOL ret = GetCursorPos(&pnt);
	if (!ret) { return false; }
	ret = ScreenToClient(hwnd, &pnt);
	if (!ret) { return false; }

	msp_old = msp; msp = { pnt.x, pnt.y };
	if (!mouse_owned) {
		bool hold = HIWORD(GetAsyncKeyState(VK_LBUTTON));
		if (!hold) { msd[0] = false; }
		hold = HIWORD(GetAsyncKeyState(VK_MBUTTON));
		if (!hold) { msd[1] = false; }
		hold = HIWORD(GetAsyncKeyState(VK_RBUTTON));
		if (!hold) { msd[2] = false; }
	} return true;
}

void AppInput::init(HWND hwnd) {
	mouse_track.cbSize = sizeof(mouse_track);
	mouse_track.dwFlags = TME_LEAVE;
	mouse_track.hwndTrack = hwnd;
}
void AppInput::reset() {
	msw = 0; chs.clear();
	// msp 在 App 中已经做过了。
	msd_old = msd; kbd_old = kbd;
	kbmsg.fill(0); isBbarClicked = false;
}

FrameRate::FrameRate() { frm_cnt_max = 10; }
void FrameRate::run() { tm = std::chrono::high_resolution_clock::now(); }
void FrameRate::update() {
	auto now = std::chrono::high_resolution_clock::now();
	real_dur += real_dt = (now - tm).count();
	tm = now; frm_cnt++; ns_cnt += real_dt;

	if (frm_cnt == frm_cnt_max) {
		frm_time = (double)ns_cnt / frm_cnt * 1e-9;
		fps = 1 / frm_time;
		frm_cnt = ns_cnt = 0;
	}
}

void InputOwners::reset() {
	hoveredDepth = wheeledDepth = -DBL_MAX; keyboardOwner = hovered = wheeled = NULL;
}
void InputOwners::remove(void* o) {
	if (keyboardOwner == o) { keyboardOwner = NULL; }
	if (hovered == o) { hovered = NULL; }
	if (wheeled == o) { wheeled = NULL; }
}
void InputOwners::ReleaseKeyboard(void* o) {
	if (keyboardOwner == o) { keyboardOwner = NULL; }
}

