#pragma once
#include "Font.h"
#include "WaveOutput.h"
#include "AppItem.h"

int constexpr LAN_CHI = 0;
int constexpr LAN_ENG = 1;

LRESULT CALLBACK WindowProcedure(HWND wnd, UINT msg, WPARAM wp, LPARAM lp);
struct App {
	int w = 0, h = 0;
	Tile screen;
	DepthBuffer depthBuffer, initialDepthBuffer;

	Font font;
	int languageIdx = 0;
	map<wstring, vector<wstring>> localizationDict;

	WaveOutput waveOutput;
	FrameRate frameRate;
	AppInput input;
	InputOwners inputOwners;

	HDC deviceContext = NULL;
	HWND windowHandle = NULL;
	BITMAPINFO bitmapInfo{};
	HCURSOR systemCursor{};
	HINSTANCE instanceHandle = NULL;

	wstring title;
	bool useSystemCursor = true;
	wstring windowClassName;

	App();
	void Init();
	virtual void Update() {}
	void Run(bool ShowConsole = false);

	void ResetDepthBuffer();
	void PresentScreen();
	void AddLog(wstring const& s);
	wstring GetLocalizedString(wstring const& id) const;

	void InitializeBitmapInfo();
	void LoadLocalizationFile();
	void RegisterWindowClass();
	void CreateApplicationWindow();
};

