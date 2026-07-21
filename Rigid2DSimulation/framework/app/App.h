#pragma once
#include "Font.h"
#include "WaveOutput.h"
#include "AppItem.h"

int constexpr LAN_CHI = 0;
int constexpr LAN_ENG = 1;

LRESULT CALLBACK WindowProcedure(HWND wnd, UINT msg, WPARAM wp, LPARAM lp);
struct App {
	int w = 0, h = 0;
	Tile scr;
	DepthBuffer dscr, ini_dscr;

	Font ft;
	int languageIdx = 0;
	map<wstring, vector<wstring>> localizationDict;

	WaveOutput waveOutput;
	FrameRate fps;
	AppInput input;
	InputOwners own;

	HDC hdc = NULL;
	HWND hwnd = NULL;
	BITMAPINFO bmi{};
	HCURSOR sys_hcs{};
	HINSTANCE hinst = NULL;

	wstring title;
	bool use_sys_cs = true;
	wstring class_name;

	App();
	void Init();
	virtual void Update() {}
	void Run(bool ShowConsole = false);

	void ResetDepthBuffer();
	void PresentScreen();
	void addLog(wstring const& s);
	wstring getLocalizedString(wstring const& id) const;

	void InitializeBitmapInfo();
	void loadLocalizationFile();
	void RegisterWindowClass();
	void CreateApplicationWindow();
};

