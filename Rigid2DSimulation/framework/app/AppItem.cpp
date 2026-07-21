#include "AppItem.h"

void AppInput::TrackMouse() {
	if (!isMouseLeaveTracked) {
		isMouseLeaveTracked = true;
		TrackMouseEvent(&mouseLeaveTracking);
	}
}
bool AppInput::SyncMouse(HWND windowHandle) {
	POINT mouseScreenPosition{}; BOOL isPositionRetrieved = GetCursorPos(&mouseScreenPosition);
	if (!isPositionRetrieved) { return false; }
	isPositionRetrieved = ScreenToClient(windowHandle, &mouseScreenPosition);
	if (!isPositionRetrieved) { return false; }

	previousMousePosition = mousePosition; mousePosition = { mouseScreenPosition.x, mouseScreenPosition.y };
	if (!isMouseInputOwned) {
		bool isButtonPressed = HIWORD(GetAsyncKeyState(VK_LBUTTON));
		if (!isButtonPressed) { mouseButtonDown[0] = false; }
		isButtonPressed = HIWORD(GetAsyncKeyState(VK_MBUTTON));
		if (!isButtonPressed) { mouseButtonDown[1] = false; }
		isButtonPressed = HIWORD(GetAsyncKeyState(VK_RBUTTON));
		if (!isButtonPressed) { mouseButtonDown[2] = false; }
	} return true;
}

void AppInput::Initialize(HWND windowHandle) {
	mouseLeaveTracking.cbSize = sizeof(mouseLeaveTracking);
	mouseLeaveTracking.dwFlags = TME_LEAVE;
	mouseLeaveTracking.hwndTrack = windowHandle;
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
void InputOwners::Remove(void* owner) {
	if (keyboardOwner == owner) { keyboardOwner = NULL; }
	if (hovered == owner) { hovered = NULL; }
	if (wheeled == owner) { wheeled = NULL; }
}
void InputOwners::ReleaseKeyboard(void* owner) {
	if (keyboardOwner == owner) { keyboardOwner = NULL; }
}

