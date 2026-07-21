#include "App.h"
#include "Tile.h"

App::App()
{
	w = 800; h = 600;
	title = L"程序"; class_name = L"class";
}
void App::Init()
{
	addLog(L"开始加载程序.\n");
	SetLocale(); scr = Tile(w, h);
	dscr = ini_dscr = DepthBuffer(w * h, -DBL_MAX);
	bool ret = ft.init();
	addLog(ret ? L"成功加载字体模块.\n" : L"加载字体模块失败.\n");

	HRESULT hr = SetProcessDpiAwareness(PROCESS_PER_MONITOR_DPI_AWARE);
	addLog(hr == S_OK ? L"成功设置 DPI 感知.\n" : L"设置 DPI 感知失败.\n");
	hinst = GetModuleHandle(NULL);
	addLog(hinst ? L"成功取得模块句柄.\n" : L"取得模块句柄失败.\n");

	RegisterWindowClass(); CreateApplicationWindow(); ret = waveOutput.Initialize();
	addLog(ret ? L"成功加载声音模块.\n" : L"加载声音模块失败.\n");
	input.init(hwnd); InitializeBitmapInfo(); loadLocalizationFile();

	hdc = GetDC(hwnd);
	addLog(hdc ? L"成功取得设备环境句柄.\n" : L"取得设备环境句柄失败.\n");
	sys_hcs = LoadCursorW(NULL, IDC_ARROW);
	addLog(sys_hcs ? L"成功加载光标.\n" : L"加载光标失败.\n");
}
void App::Run(bool console)
{
	if (!console) { HideConsole(); }
	MSG msg{}; fps.run(); ShowWindow(hwnd, SW_SHOW);

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
			bool ret = SetWindowText(hwnd, title.c_str());
			if (!ret) { addLog(L"设置标题失败.\n"); }
			fps.update(); ResetDepthBuffer();
			ret = input.SyncMouse(hwnd);
			if (!ret) { addLog(L"同步鼠标位置失败.\n");
			}
			Update(); ret = waveOutput.Play();
			if (!ret) { addLog(L"播放声音失败.\n"); }
			PresentScreen(); input.reset(); own.reset();
		}
	} ReleaseDC(hwnd, hdc);
	waveOutput.Shutdown(); addLog(L"退出程序.\n\n");
}

void App::ResetDepthBuffer()
{
	memcpy(dscr.data(), ini_dscr.data(), dscr.size() * sizeof(double));
}
void App::PresentScreen()
{
	int ret = SetDIBitsToDevice(hdc, 0, 0, w, h,
		0, 0, 0, h, scr.colors.data(), &bmi, DIB_RGB_COLORS);
	if (!ret) { addLog(L"展示屏幕失败.\n"); }
}
void App::addLog(wstring const& s)
{
	FILE* f_log = wfopen(L"运行记录.log", L"a");
	if (!f_log) { PrintConsole(L"运行记录打开失败。", true); }
	else { fwprintf(f_log, s.c_str()); fclose(f_log); }
}
wstring App::getLocalizedString(wstring const& id) const
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
	bmi.bmiHeader = hd;
}
void App::loadLocalizationFile()
{
	FILE* f = wfopen(L"localization.txt", L"r");
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
	wc.hInstance = hinst;
	wc.lpszClassName = class_name.c_str();

	ATOM ret = RegisterClassExW(&wc);
	addLog(ret ? L"成功注册窗口类.\n" : L"注册窗口类失败.\n");
}
void App::CreateApplicationWindow()
{
	RECT cr{}; cr.right = w; cr.bottom = h;
	DWORD sty = WS_SYSMENU | WS_CAPTION;
	BOOL ret = AdjustWindowRect(&cr, sty, FALSE);
	addLog(ret ? L"成功调整窗口矩形.\n" : L"调整窗口矩形失败.\n");
	int rw = cr.right - cr.left;
	int rh = cr.bottom - cr.top;

	hwnd = CreateWindowExW(0, class_name.c_str(),
		title.c_str(), sty, 0, 0, rw, rh, NULL, NULL, hinst, this);
	addLog(hwnd ? L"成功创建窗口.\n" : L"创建窗口失败.\n");
}

