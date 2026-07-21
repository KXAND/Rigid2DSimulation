#include "AppItem.h"

void AppInput::TrackMouse() {
	if (!isMouseLeaveTracked) {
		isMouseLeaveTracked = true;
		TrackMouseEvent(&mouseLeaveTracking);
	}
}
bool AppInput::SyncMouse(HWND hwnd) {
	POINT pnt{}; BOOL ret = GetCursorPos(&pnt);
	if (!ret) { return false; }
	ret = ScreenToClient(hwnd, &pnt);
	if (!ret) { return false; }

	previousMousePosition = mousePosition; mousePosition = { pnt.x, pnt.y };
	if (!isMouseInputOwned) {
		bool hold = HIWORD(GetAsyncKeyState(VK_LBUTTON));
		if (!hold) { mouseButtonDown[0] = false; }
		hold = HIWORD(GetAsyncKeyState(VK_MBUTTON));
		if (!hold) { mouseButtonDown[1] = false; }
		hold = HIWORD(GetAsyncKeyState(VK_RBUTTON));
		if (!hold) { mouseButtonDown[2] = false; }
	} return true;
}

void AppInput::Initialize(HWND hwnd) {
	mouseLeaveTracking.cbSize = sizeof(mouseLeaveTracking);
	mouseLeaveTracking.dwFlags = TME_LEAVE;
	mouseLeaveTracking.hwndTrack = hwnd;
}
void AppInput::Reset() {
	mouseWheelDelta = 0; typedCharacters.clear();
	// mousePosition 在 App 中已经做过了。
	previousMouseButtonDown = mouseButtonDown; previousKeyDown = keyDown;
	keyMessageCounts.fill(0); isTitleBarClicked = false;
}

FrameRate::FrameRate() { frameSampleCount = 10; }
void FrameRate::Start() { previousFrameTimePoint = std::chrono::high_resolution_clock::now(); }
void FrameRate::Update() {
	auto now = std::chrono::high_resolution_clock::now();
	elapsedNanoseconds += deltaNanoseconds = (now - previousFrameTimePoint).count();
	previousFrameTimePoint = now; frameCount++; sampleNanoseconds += deltaNanoseconds;

	if (frameCount == frameSampleCount) {
		averageFrameTime = (double)sampleNanoseconds / frameCount * 1e-9;
		framesPerSecond = 1 / averageFrameTime;
		frameCount = sampleNanoseconds = 0;
	}
}

void InputOwners::Reset() {
	hoveredDepth = wheeledDepth = -DBL_MAX; keyboardOwner = hovered = wheeled = NULL;
}
void InputOwners::Remove(void* o) {
	if (keyboardOwner == o) { keyboardOwner = NULL; }
	if (hovered == o) { hovered = NULL; }
	if (wheeled == o) { wheeled = NULL; }
}
void InputOwners::ReleaseKeyboard(void* o) {
	if (keyboardOwner == o) { keyboardOwner = NULL; }
}

