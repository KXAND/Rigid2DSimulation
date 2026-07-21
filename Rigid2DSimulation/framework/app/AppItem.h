#pragma once
#include "DVector2.h"

struct AppInput {
	double mouseWheelDelta = 0;
	wstring typedCharacters;
	DVector2 mousePosition, previousMousePosition;
	array<BYTE, 3> mouseButtonDown{}, previousMouseButtonDown{};
	array<int, 256> keyMessageCounts{};
	array<BYTE, 256> keyDown{}, previousKeyDown{};
	// 比如说播音乐, 在窗口移动时可以使音乐暂停。
	bool isTitleBarClicked = false;

	// 我觉得这个鼠标逻辑还是比较奇怪，可能有多余之处。
	// 以后要改一下，但请注意这里各种不合理的布置都是有理由的。
	bool isMouseInputOwned = false;
	bool isMouseLeaveTracked = false;
	TRACKMOUSEEVENT mouseLeaveTracking{};

	bool IsMouseClickedDown(int m) { return mouseButtonDown[m] && !previousMouseButtonDown[m]; }
	bool IsKeyPressedDown(int k) { return keyDown[k] && !previousKeyDown[k]; }

	void TrackMouse();
	bool SyncMouse(HWND hwnd);

	void Initialize(HWND hwnd);
	void Reset();
};

struct FrameRate {
	int frameCount = 0;
	int frameSampleCount = 0;
	long long sampleNanoseconds = 0;
	double framesPerSecond = 0, averageFrameTime = 0;
	long long deltaNanoseconds = 0, elapsedNanoseconds = 0;
	std::chrono::high_resolution_clock::time_point previousFrameTimePoint;

	FrameRate();
	void Start();
	void Update();

	double GetDeltaTime() const { return deltaNanoseconds / 1e9; }
	double GetElapsedTime() const { return elapsedNanoseconds / 1e9; }
};

struct InputOwners {
	void* keyboardOwner = NULL;
	void* hovered = NULL;
	void* wheeled = NULL;
	double hoveredDepth = -DBL_MAX, wheeledDepth = -DBL_MAX;

	void Reset();
	void Remove(void* o);
	void ReleaseKeyboard(void* o);
};

