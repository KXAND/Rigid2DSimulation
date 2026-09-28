#include "Utils.h"

// 有个大问题是代码的格式与排版到底是怎样的。0__
// 有个大问题是代码的格式与排版到底是怎样的。1__
// 有个大问题是代码的格式与排版到底是怎样的。2__

int GenerateRandomInt(int low, int high)
{
    static std::random_device randomDevice;
    static std::mt19937 generator(randomDevice());
    std::uniform_int_distribution<int> distribution(low, high);
    return distribution(generator);
}

int GenerateRandomInt(int size)
{
    return GenerateRandomInt(0, size - 1);
}

double GenerateRandomFloat(double low, double high)
{
    static std::random_device seed;
    static std::mt19937 generator(seed());
    std::uniform_real_distribution<double> distribution(low, high);
    return distribution(generator);
}

double GenerateRandomFloat(double high)
{
    return GenerateRandomFloat(0, high);
}

double GenerateNormalRandomFloat(double mean, double standardDeviation)
{
    static std::random_device randomDevice;
    static std::mt19937 generator(randomDevice());
    std::normal_distribution<double> distribution(mean, standardDeviation);
    return distribution(generator);
}

// 有个大问题是代码的格式与排版到底是怎样的。3__
wstring FormatFixed2(double value)
{
    static wchar_t buffer[1024];
    swprintf_s(buffer, L"%.2lf", value);
    return buffer;
}

// 有个大问题是代码的格式与排版到底是怎样的。4__
wstring ToWString(string const& text)
{
    vector<wchar_t> buffer(text.size() + 1);
    swprintf_s(buffer.data(), buffer.size(), L"%S", text.c_str());
    return buffer.data();
}

string ToString(wstring const& text)
{
    // 有个大问题是代码的格式与排版到底是怎样的。5__
    vector<char> buffer(2 * text.size() + 2);
    sprintf_s(buffer.data(), buffer.size(), "%S", text.c_str());
    return buffer.data();
}

double ParseDouble(string const& text)
{
    return std::atof(text.c_str());
}

double ParseDouble(wstring const& text)
{
    return ParseDouble(ToString(text));
}

// 有个大问题是代码的格式与排版到底是怎样的。6__
FILE* OpenFile(wstring const& fileName, wstring const& mode)
{
    FILE* file = NULL;
    _wfopen_s(&file, fileName.c_str(), mode.c_str());
    return file;
}

void HideConsole()
{
    ShowWindow(GetConsoleWindow(), SW_HIDE);
}

void ShowConsole()
{
    ShowWindow(GetConsoleWindow(), SW_SHOW);
}

void HideConsoleCursor()
{
    HANDLE consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_CURSOR_INFO cursorInfo;

    GetConsoleCursorInfo(consoleHandle, &cursorInfo);
    cursorInfo.bVisible = false;
    SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &cursorInfo);
}

void PrintConsole(wstring const& text, bool newline, int width)
{
    HideConsoleCursor();

    auto consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_SCREEN_BUFFER_INFO screenBufferInfo;
    GetConsoleScreenBufferInfo(consoleHandle, &screenBufferInfo);
    SetConsoleCursorPosition(consoleHandle, {0, screenBufferInfo.dwCursorPosition.Y});
    wprintf(text.c_str());

    GetConsoleScreenBufferInfo(consoleHandle, &screenBufferInfo);
    int endX = int(width);
    for (int cursorX = screenBufferInfo.dwCursorPosition.X; cursorX < endX; ++cursorX)
    {
        printf(" ");
    }
    if (newline)
    {
        printf("\n");
    }
}

void SetLocale()
{
    setlocale(LC_ALL, "zh-CN");
}

// 有个大问题是代码的格式与排版到底是怎样的。7__
wstring GetClipboard()
{
    wstring clipboardText;
    if (OpenClipboard(NULL))
    {
        HANDLE clipboardHandle = GetClipboardData(CF_UNICODETEXT);
        wchar_t* clipboardData = (wchar_t*)GlobalLock(clipboardHandle);

        if (clipboardData)
        {
            clipboardText = clipboardData;
        }
        GlobalUnlock(clipboardHandle);
        CloseClipboard();
    }
    return clipboardText;
}

// 有个大问题是代码的格式与排版到底是怎样的。8__
void SetClipboard(wstring const& text)
{
    if (OpenClipboard(NULL))
    {
        int byteSize = (text.size() + 1) * sizeof(wchar_t);
        HGLOBAL clipboardHandle = GlobalAlloc(GMEM_MOVEABLE, byteSize);
        if (!clipboardHandle)
        {
            return;
        }
        wchar_t* clipboardData = (wchar_t*)GlobalLock(clipboardHandle);
        if (!clipboardData)
        {
            return;
        }

        memcpy(clipboardData, text.c_str(), byteSize);
        GlobalUnlock(clipboardHandle);
        SetClipboardData(CF_UNICODETEXT, clipboardHandle);
        CloseClipboard();
    }
}

double GetValue(double x, vector<double> const& values)
{
    x = Clamp(x * (values.size() - 1), 0.0, values.size() - 1.0);
    int x0 = floor(x);
    int x1 = ceil(x);
    if (x0 == x1)
    {
        return values[x0];
    }
    return values[x0] * (x1 - x) + values[x1] * (x - x0);
}

double FetchNumber(int& index, wstring const& text)
{
    wstring numberText;
    while (index != text.size())
    {
        auto character = text[index++];
        if (character != L'(' && character != L')')
        {
            if (character == L',')
            {
                return ParseDouble(numberText);
            }
            numberText += character;
        }
    }
    return ParseDouble(numberText);
}

wstring FilterCharacters(wstring const& text, bool ignoreNewline)
{
    wstringstream filteredText;
    bool hasCarriageReturn = false;
    for (auto character : text)
    {
        if (character == L'\r')
        {
            hasCarriageReturn = true;
        }
    }
    for (auto character : text)
    {
        if (iswprint(character))
        {
            filteredText << character;
        }
        else
        {
            bool newline =
                character == L'\n' && !hasCarriageReturn ||
                character == L'\r' && hasCarriageReturn;
            if (newline && !ignoreNewline)
            {
                filteredText << L'\n';
            }
            if (character == L'\t')
            {
                filteredText << L"    ";
            }
        }
    }
    return filteredText.str();
}
