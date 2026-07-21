#include "App.h"

int created(App*& app, WPARAM wp, LPARAM lp) {
	auto pcs = (CREATESTRUCT*)lp;
	app = (App*)pcs->lpCreateParams;
	return 0;
}
int destroyed(App* app, WPARAM wp, LPARAM lp) {
	PostQuitMessage(0);
	return 0;
}
int HandleKillFocus(App* app, WPARAM wp, LPARAM lp) {
	app->input.kbd.fill(false);
	return 0;
}
int HandleSetCursor(App* app, WPARAM wp, LPARAM lp) {
	HCURSOR cs =
		LOWORD(lp) != HTCLIENT || app->use_sys_cs ?
		app->sys_hcs : NULL;
	SetCursor(cs);
	return 0;
}
int HandleCaptureChanged(App* app, WPARAM wp, LPARAM lp) {
	app->input.isBbarClicked = true;
	app->input.kbd.fill(false);
	return 0;
}
int HandleCharacter(App* app, WPARAM wp, LPARAM lp) {
	app->input.chs += (wchar_t)wp;
	return 0;
}
int HandleKeyUp(App* app, WPARAM wp, LPARAM lp) {
	app->input.kbd[wp] = false;
	return 0;
}
int HandleKeyDown(App* app, WPARAM wp, LPARAM lp) {
	app->input.kbmsg[wp]++;
	app->input.kbd[wp] = true;
	return 0;
}
int HandleMouseMove(App* app, WPARAM wp, LPARAM lp) {
	return 0;
}
int HandleMouseWheel(App* app, WPARAM wp, LPARAM lp) {
	int dt = GET_WHEEL_DELTA_WPARAM(wp);
	app->input.msw = (double)dt / WHEEL_DELTA;
	return 0;
}

// 有很多情况要考虑：如果鼠标被拔掉了，会怎么样。
// 可能一帧之内鼠标做了很多事情。
// 窗口最小化或者被遮挡之后，如何判断鼠标是否在窗口内。
// 好像鼠标在桌面之外的时候不会有 MOUSELEAVE 消息。
int HandleMouseLeave(App* app, WPARAM wp, LPARAM lp) {
	app->input.mouse_owned = false;
	app->input.mouse_tracked = false;
	return 0;
}
int HandleLeftMouseUp(App* app, WPARAM wp, LPARAM lp) {
	app->input.msd[0] = false;
	app->input.mouse_owned = true;
	return 0;
}
int HandleLeftMouseDown(App* app, WPARAM wp, LPARAM lp) {
	app->input.msd[0] = true;
	app->input.mouse_owned = true;
	app->input.track_mouse();
	return 0;
}
int HandleMiddleMouseUp(App* app, WPARAM wp, LPARAM lp) {
	app->input.msd[1] = false;
	app->input.mouse_owned = true;
	return 0;
}
int HandleMiddleMouseDown(App* app, WPARAM wp, LPARAM lp) {
	app->input.msd[1] = true;
	app->input.mouse_owned = true;
	app->input.track_mouse();
	return 0;
}
int HandleRightMouseUp(App* app, WPARAM wp, LPARAM lp) {
	app->input.msd[2] = false;
	app->input.mouse_owned = true;
	return 0;
}
int HandleRightMouseDown(App* app, WPARAM wp, LPARAM lp) {
	app->input.msd[2] = true;
	app->input.mouse_owned = true;
	app->input.track_mouse();
	return 0;
}

// 我搞不清楚什么是 capture changed。
// 实际中发现在点击标题栏的时候会有这个消息。
LRESULT CALLBACK WindowProcedure(HWND wnd, UINT msg, WPARAM wp, LPARAM lp) {
	static App* app = NULL;
	switch (msg) {
	case WM_CREATE: return created(app, wp, lp);
	case WM_DESTROY: return destroyed(app, wp, lp);
	case WM_KILLFOCUS: return HandleKillFocus(app, wp, lp);
	case WM_SETCURSOR: return HandleSetCursor(app, wp, lp);
	case WM_CAPTURECHANGED: return HandleCaptureChanged(app, wp, lp);
	case WM_CHAR: return HandleCharacter(app, wp, lp);
	case WM_KEYUP: return HandleKeyUp(app, wp, lp);
	case WM_KEYDOWN: return HandleKeyDown(app, wp, lp);
	case WM_MOUSEMOVE: return HandleMouseMove(app, wp, lp);
	case WM_MOUSEWHEEL: return HandleMouseWheel(app, wp, lp);
	case WM_MOUSELEAVE: return HandleMouseLeave(app, wp, lp);
	case WM_LBUTTONUP: return HandleLeftMouseUp(app, wp, lp);
	case WM_LBUTTONDOWN: return HandleLeftMouseDown(app, wp, lp);
	case WM_MBUTTONUP: return HandleMiddleMouseUp(app, wp, lp);
	case WM_MBUTTONDOWN: return HandleMiddleMouseDown(app, wp, lp);
	case WM_RBUTTONUP: return HandleRightMouseUp(app, wp, lp);
	case WM_RBUTTONDOWN: return HandleRightMouseDown(app, wp, lp);
	default: return DefWindowProc(wnd, msg, wp, lp);
	}
}

