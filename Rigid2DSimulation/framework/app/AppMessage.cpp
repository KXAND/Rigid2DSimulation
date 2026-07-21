#include "App.h"

int HandleCreate(App*& application, WPARAM wParam, LPARAM lParam) {
	auto createStruct = (CREATESTRUCT*)lParam;
	application = (App*)createStruct->lpCreateParams;
	return 0;
}
int HandleDestroy(App* application, WPARAM wParam, LPARAM lParam) {
	PostQuitMessage(0);
	return 0;
}
int HandleKillFocus(App* application, WPARAM wParam, LPARAM lParam) {
	application->input.keyDown.fill(false);
	return 0;
}
int HandleSetCursor(App* application, WPARAM wParam, LPARAM lParam) {
	HCURSOR cursor =
		LOWORD(lParam) != HTCLIENT || application->useSystemCursor ?
		application->systemCursor : NULL;
	SetCursor(cursor);
	return 0;
}
int HandleCaptureChanged(App* application, WPARAM wParam, LPARAM lParam) {
	application->input.isTitleBarClicked = true;
	application->input.keyDown.fill(false);
	return 0;
}
int HandleCharacter(App* application, WPARAM wParam, LPARAM lParam) {
	application->input.typedCharacters += (wchar_t)wParam;
	return 0;
}
int HandleKeyUp(App* application, WPARAM wParam, LPARAM lParam) {
	application->input.keyDown[wParam] = false;
	return 0;
}
int HandleKeyDown(App* application, WPARAM wParam, LPARAM lParam) {
	application->input.keyMessageCounts[wParam]++;
	application->input.keyDown[wParam] = true;
	return 0;
}
int HandleMouseMove(App* application, WPARAM wParam, LPARAM lParam) {
	return 0;
}
int HandleMouseWheel(App* application, WPARAM wParam, LPARAM lParam) {
	int wheelDelta = GET_WHEEL_DELTA_WPARAM(wParam);
	application->input.mouseWheelDelta = (double)wheelDelta / WHEEL_DELTA;
	return 0;
}

// 有很多情况要考虑：如果鼠标被拔掉了，会怎么样。
// 可能一帧之内鼠标做了很多事情。
// 窗口最小化或者被遮挡之后，如何判断鼠标是否在窗口内。
// 好像鼠标在桌面之外的时候不会有 MOUSELEAVE 消息。
int HandleMouseLeave(App* application, WPARAM wParam, LPARAM lParam) {
	application->input.isMouseInputOwned = false;
	application->input.isMouseLeaveTracked = false;
	return 0;
}
int HandleLeftMouseUp(App* application, WPARAM wParam, LPARAM lParam) {
	application->input.mouseButtonDown[0] = false;
	application->input.isMouseInputOwned = true;
	return 0;
}
int HandleLeftMouseDown(App* application, WPARAM wParam, LPARAM lParam) {
	application->input.mouseButtonDown[0] = true;
	application->input.isMouseInputOwned = true;
	application->input.TrackMouse();
	return 0;
}
int HandleMiddleMouseUp(App* application, WPARAM wParam, LPARAM lParam) {
	application->input.mouseButtonDown[1] = false;
	application->input.isMouseInputOwned = true;
	return 0;
}
int HandleMiddleMouseDown(App* application, WPARAM wParam, LPARAM lParam) {
	application->input.mouseButtonDown[1] = true;
	application->input.isMouseInputOwned = true;
	application->input.TrackMouse();
	return 0;
}
int HandleRightMouseUp(App* application, WPARAM wParam, LPARAM lParam) {
	application->input.mouseButtonDown[2] = false;
	application->input.isMouseInputOwned = true;
	return 0;
}
int HandleRightMouseDown(App* application, WPARAM wParam, LPARAM lParam) {
	application->input.mouseButtonDown[2] = true;
	application->input.isMouseInputOwned = true;
	application->input.TrackMouse();
	return 0;
}

// 我搞不清楚什么是 capture changed。
// 实际中发现在点击标题栏的时候会有这个消息。
LRESULT CALLBACK WindowProcedure(HWND windowHandle, UINT message, WPARAM wParam, LPARAM lParam) {
	static App* application = NULL;
	switch (message) {
	case WM_CREATE: return HandleCreate(application, wParam, lParam);
	case WM_DESTROY: return HandleDestroy(application, wParam, lParam);
	case WM_KILLFOCUS: return HandleKillFocus(application, wParam, lParam);
	case WM_SETCURSOR: return HandleSetCursor(application, wParam, lParam);
	case WM_CAPTURECHANGED: return HandleCaptureChanged(application, wParam, lParam);
	case WM_CHAR: return HandleCharacter(application, wParam, lParam);
	case WM_KEYUP: return HandleKeyUp(application, wParam, lParam);
	case WM_KEYDOWN: return HandleKeyDown(application, wParam, lParam);
	case WM_MOUSEMOVE: return HandleMouseMove(application, wParam, lParam);
	case WM_MOUSEWHEEL: return HandleMouseWheel(application, wParam, lParam);
	case WM_MOUSELEAVE: return HandleMouseLeave(application, wParam, lParam);
	case WM_LBUTTONUP: return HandleLeftMouseUp(application, wParam, lParam);
	case WM_LBUTTONDOWN: return HandleLeftMouseDown(application, wParam, lParam);
	case WM_MBUTTONUP: return HandleMiddleMouseUp(application, wParam, lParam);
	case WM_MBUTTONDOWN: return HandleMiddleMouseDown(application, wParam, lParam);
	case WM_RBUTTONUP: return HandleRightMouseUp(application, wParam, lParam);
	case WM_RBUTTONDOWN: return HandleRightMouseDown(application, wParam, lParam);
	default: return DefWindowProc(windowHandle, message, wParam, lParam);
	}
}

