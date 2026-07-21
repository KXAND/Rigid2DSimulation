#include "App.h"
#include "Tile.h"

App::App()
{
	w = 800; h = 600;
	title = L"程序"; windowClassName = L"class";
}
void App::Init()
{
	AddLog(L"开始加载程序.\n");
	SetLocale(); screen = Tile(w, h);
	depthBuffer = initialDepthBuffer = DepthBuffer(w * h, -DBL_MAX);
	bool ret = font.Initialize();
	AddLog(ret ? L"成功加载字体模块.\n" : L"加载字体模块失败.\n");

	HRESULT hr = SetProcessDpiAwareness(PROCESS_PER_MONITOR_DPI_AWARE);
	AddLog(hr == S_OK ? L"成功设置 DPI 感知.\n" : L"设置 DPI 感知失败.\n");
	instanceHandle = GetModuleHandle(NULL);
	AddLog(instanceHandle ? L"成功取得模块句柄.\n" : L"取得模块句柄失败.\n");

	RegisterWindowClass(); CreateApplicationWindow(); ret = waveOutput.Initialize();
	AddLog(ret ? L"成功加载声音模块.\n" : L"加载声音模块失败.\n");
	input.Initialize(windowHandle); InitializeBitmapInfo(); LoadLocalizationFile();

	deviceContext = GetDC(windowHandle);
	AddLog(deviceContext ? L"成功取得设备环境句柄.\n" : L"取得设备环境句柄失败.\n");
	systemCursor = LoadCursorW(NULL, IDC_ARROW);
	AddLog(systemCursor ? L"成功加载光标.\n" : L"加载光标失败.\n");
}
void App::Run(bool console)
{
	if (!console) { HideConsole(); }
	MSG msg{}; frameRate.Start(); ShowWindow(windowHandle, SW_SHOW);

	while (true)
	{
		if (PeekMessageW(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT) { break; }
			TranslateMessage(&msg);
			DispatchMessageW(&msg);
		}
		else
		{
			bool ret = SetWindowText(windowHandle, title.c_str());
			if (!ret) { AddLog(L"设置标题失败.\n"); }
			frameRate.Update(); ResetDepthBuffer();
			ret = input.SyncMouse(windowHandle);
			if (!ret) { AddLog(L"同步鼠标位置失败.\n");
			}
			Update(); ret = waveOutput.Play();
			if (!ret) { AddLog(L"播放声音失败.\n"); }
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
	int ret = SetDIBitsToDevice(deviceContext, 0, 0, w, h,
		0, 0, 0, h, screen.colors.data(), &bitmapInfo, DIB_RGB_COLORS);
	if (!ret) { AddLog(L"展示屏幕失败.\n"); }
}
void App::AddLog(wstring const& s)
{
	FILE* f_log = OpenFile(L"运行记录.log", L"a");
	if (!f_log) { PrintConsole(L"运行记录打开失败。", true); }
	else { fwprintf(f_log, s.c_str()); fclose(f_log); }
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
	BITMAPINFOHEADER hd{};
	hd.biSize = sizeof(hd);
	hd.biWidth = w;
	hd.biHeight = -h;
	hd.biPlanes = 1;
	hd.biBitCount = 24;
	hd.biCompression = BI_RGB;
	bitmapInfo.bmiHeader = hd;
}
void App::LoadLocalizationFile()
{
	FILE* f = OpenFile(L"localization.txt", L"r");
	if (!f) { return; }

	bool dollar = false;
	wstringstream s; wstring id;
	vector<wstring> strs; wchar_t c = getwc(f);
	while (c != WEOF)
	{
		if (c == L'$')
		{
			if (dollar)
			{
				strs.push_back(s.str());
				s = wstringstream();
			} dollar = !dollar;
		}
		else if (dollar) { s << c; }
		else if (!iswspace(c))
		{
			if (!strs.empty())
			{
				localizationDict[id] = strs;
				strs.clear(); id.clear();
			} id += c;
		} c = getwc(f);
	}
	if (!strs.empty()) { localizationDict[id] = strs; }
	fclose(f);
}
void App::RegisterWindowClass()
{
	WNDCLASSEXW wc{};
	wc.cbSize = sizeof(wc);
	wc.lpfnWndProc = WindowProcedure;
	wc.hInstance = instanceHandle;
	wc.lpszClassName = windowClassName.c_str();

	ATOM ret = RegisterClassExW(&wc);
	AddLog(ret ? L"成功注册窗口类.\n" : L"注册窗口类失败.\n");
}
void App::CreateApplicationWindow()
{
	RECT cr{}; cr.right = w; cr.bottom = h;
	DWORD sty = WS_SYSMENU | WS_CAPTION;
	BOOL ret = AdjustWindowRect(&cr, sty, FALSE);
	AddLog(ret ? L"成功调整窗口矩形.\n" : L"调整窗口矩形失败.\n");
	int rw = cr.right - cr.left;
	int rh = cr.bottom - cr.top;

	windowHandle = CreateWindowExW(0, windowClassName.c_str(),
		title.c_str(), sty, 0, 0, rw, rh, NULL, NULL, instanceHandle, this);
	AddLog(windowHandle ? L"成功创建窗口.\n" : L"创建窗口失败.\n");
}

