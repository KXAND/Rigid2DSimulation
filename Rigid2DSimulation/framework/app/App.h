#pragma once
#include "Font.h"
#include "WaveOutput.h"
#include "AppItem.h"

int constexpr kChineseLanguage = 0;
int constexpr kEnglishLanguage = 1;

LRESULT CALLBACK WindowProcedure(HWND windowHandle, UINT message, WPARAM wParam, LPARAM lParam);
struct App
{
    int width = 0, height = 0;
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
    void Run(bool showConsole = false);

    void ResetDepthBuffer();
    void PresentScreen();
    void AddLog(wstring const& text);
    wstring GetLocalizedString(wstring const& id) const;

    void InitializeBitmapInfo();
    void LoadLocalizationFile();
    void RegisterWindowClass();
    void CreateApplicationWindow();
};
