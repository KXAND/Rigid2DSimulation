#include "App.h"
#include "Tile.h"

App::App()
{
	width = 800; height = 600;
	title = L"程序"; windowClassName = L"class";
}
void App::Init()
{
	AddLog(L"开始加载程序.\n");
	SetLocale(); screen = Tile(width, height);
	depthBuffer = initialDepthBuffer = DepthBuffer(width * height, -DBL_MAX);
	bool isFontLoaded = font.Initialize();
	AddLog(isFontLoaded ? L"成功加载字体模块.\n" : L"加载字体模块失败.\n");

	HRESULT dpiAwarenessResult = SetProcessDpiAwareness(PROCESS_PER_MONITOR_DPI_AWARE);
	AddLog(dpiAwarenessResult == S_OK ? L"成功设置 DPI 感知.\n" : L"设置 DPI 感知失败.\n");
	instanceHandle = GetModuleHandle(NULL);
	AddLog(instanceHandle ? L"成功取得模块句柄.\n" : L"取得模块句柄失败.\n");

	RegisterWindowClass(); CreateApplicationWindow(); bool isAudioLoaded = waveOutput.Initialize();
	AddLog(isAudioLoaded ? L"成功加载声音模块.\n" : L"加载声音模块失败.\n");
	input.Initialize(windowHandle); InitializeBitmapInfo(); LoadLocalizationFile();

	deviceContext = GetDC(windowHandle);
	AddLog(deviceContext ? L"成功取得设备环境句柄.\n" : L"取得设备环境句柄失败.\n");
	systemCursor = LoadCursorW(NULL, IDC_ARROW);
	AddLog(systemCursor ? L"成功加载光标.\n" : L"加载光标失败.\n");
}
void App::Run(bool showConsole)
{
	if (!showConsole) { HideConsole(); }
	MSG message{}; frameRate.Start(); ShowWindow(windowHandle, SW_SHOW);

	while (true)
	{
		if (PeekMessageW(&message, NULL, 0, 0, PM_REMOVE))
		{
			if (message.message == WM_QUIT) { break; }
			TranslateMessage(&message);
			DispatchMessageW(&message);
		}
		else
		{
			bool isWindowTitleUpdated = SetWindowText(windowHandle, title.c_str());
			if (!isWindowTitleUpdated) { AddLog(L"设置标题失败.\n"); }
			frameRate.Update(); ResetDepthBuffer();
			bool isMouseSynchronized = input.SyncMouse(windowHandle);
			if (!isMouseSynchronized) { AddLog(L"同步鼠标位置失败.\n");
			}
			Update(); bool isAudioPlayed = waveOutput.Play();
			if (!isAudioPlayed) { AddLog(L"播放声音失败.\n"); }
			PresentScreen(); input.Reset(); inputOwners.Reset();
		}
	} ReleaseDC(windowHandle, deviceContext);
	waveOutput.Shutdown(); AddLog(L"退出程序.\n\n");
}

void App::ResetDepthBuffer()
{
	memcpy(depthBuffer.data(), initialDepthBuffer.data(), depthBuffer.size() * sizeof(double));
}
void App::PresentScreen()
{
	int isScreenPresented = SetDIBitsToDevice(deviceContext, 0, 0, width, height,
		0, 0, 0, height, screen.colors.data(), &bitmapInfo, DIB_RGB_COLORS);
	if (!isScreenPresented) { AddLog(L"展示屏幕失败.\n"); }
}
void App::AddLog(wstring const& text)
{
	FILE* logFile = OpenFile(L"运行记录.log", L"a");
	if (!logFile) { PrintConsole(L"运行记录打开失败。", true); }
	else { fwprintf(logFile, text.c_str()); fclose(logFile); }
}
wstring App::GetLocalizedString(wstring const& id) const
{
	if (id.empty()) { return {}; }

	auto it = localizationDict.find(id);
	if (it == localizationDict.end()) { return {}; }
	auto const& translations = it->second;
	return languageIdx >= translations.size() ? L"" : translations[languageIdx];
}

void App::InitializeBitmapInfo()
{
	BITMAPINFOHEADER bitmapInfoHeader{};
	bitmapInfoHeader.biSize = sizeof(bitmapInfoHeader);
	bitmapInfoHeader.biWidth = width;
	bitmapInfoHeader.biHeight = -height;
	bitmapInfoHeader.biPlanes = 1;
	bitmapInfoHeader.biBitCount = 24;
	bitmapInfoHeader.biCompression = BI_RGB;
	bitmapInfo.bmiHeader = bitmapInfoHeader;
}
void App::LoadLocalizationFile()
{
	FILE* file = OpenFile(L"localization.txt", L"r");
	if (!file) { return; }

	bool dollar = false;
	wstringstream textBuffer; wstring id;
	vector<wstring> translations; wchar_t character = getwc(file);
	while (character != WEOF)
	{
		if (character == L'$')
		{
			if (dollar)
			{
				translations.push_back(textBuffer.str());
				textBuffer = wstringstream();
			} dollar = !dollar;
		}
		else if (dollar) { textBuffer << character; }
		else if (!iswspace(character))
		{
			if (!translations.empty())
			{
				localizationDict[id] = translations;
				translations.clear(); id.clear();
			} id += character;
		} character = getwc(file);
	}
	if (!translations.empty()) { localizationDict[id] = translations; }
	fclose(file);
}
void App::RegisterWindowClass()
{
	WNDCLASSEXW windowClass{};
	windowClass.cbSize = sizeof(windowClass);
	windowClass.lpfnWndProc = WindowProcedure;
	windowClass.hInstance = instanceHandle;
	windowClass.lpszClassName = windowClassName.c_str();

	ATOM windowClassAtom = RegisterClassExW(&windowClass);
	AddLog(windowClassAtom ? L"成功注册窗口类.\n" : L"注册窗口类失败.\n");
}
void App::CreateApplicationWindow()
{
	RECT clientRect{}; clientRect.right = width; clientRect.bottom = height;
	DWORD windowStyle = WS_SYSMENU | WS_CAPTION;
	BOOL isWindowRectAdjusted = AdjustWindowRect(&clientRect, windowStyle, FALSE);
	AddLog(isWindowRectAdjusted ? L"成功调整窗口矩形.\n" : L"调整窗口矩形失败.\n");
	int windowWidth = clientRect.right - clientRect.left;
	int windowHeight = clientRect.bottom - clientRect.top;

	windowHandle = CreateWindowExW(0, windowClassName.c_str(),
		title.c_str(), windowStyle, 0, 0, windowWidth, windowHeight, NULL, NULL, instanceHandle, this);
	AddLog(windowHandle ? L"成功创建窗口.\n" : L"创建窗口失败.\n");
}

