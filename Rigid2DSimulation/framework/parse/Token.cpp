#include "Token.h"

Token::Token(wstring const& type) : type(type)
{
}

Token::Token(double number) : number(number)
{
    type = L"num";
}

Token Token::CreateString(wstring const& text)
{
    Token token(L"str");
    token.stringValue = text;
    return token;
}

Token Token::CreateVariable(wstring const& value, bool isGlobal)
{
    Token token(L"var");
    token.variableName = value;
    token.isGlobal = isGlobal;
    return token;
}

wstring ToWString(Token const& token)
{
    wstring serializedText = L"[" + token.type;
    if (token.type == L"var")
    {
        serializedText += (token.isGlobal ? L":&" : L":") + token.variableName;
    }
    if (token.type == L"num")
    {
        serializedText += L":" + FormatFixed2(token.number);
    }
    if (token.type == L"str")
    {
        serializedText += L":" + token.stringValue;
    }
    serializedText += L"]";
    return serializedText;
}

Token ParseNumberToken(wstring const& source, int& index)
{
    wstring numberText = {source[index]};
    bool isValid = true;
    bool hasDecimalPoint = false;

    while (isValid && ++index != source.size())
    {
        auto character = source[index];
        if (character == L'.')
        {
            if (hasDecimalPoint)
            {
                return Token(L"error");
            }
            hasDecimalPoint = true;
        }
        isValid = iswdigit(character) || character == L'.';
        if (isValid)
        {
            numberText += character;
        }
    }

    auto character = source[index];
    if (isalpha(character) || character == L'_')
    {
        return Token(L"error");
    }
    return Token(ParseDouble(numberText));
}

Token ParseStringToken(wstring const& source, int& index)
{
    wstring stringValue;
    bool isValid = true;
    bool isEscaped = false;

    while (isValid && ++index != source.size())
    {
        auto character = source[index];
        if (character == L'\n')
        {
            return Token(L"error");
        }
        if (isEscaped)
        {
            if (character == L'n')
            {
                character = L'\n';
            }
            isEscaped = false;
            isValid = true;
        }
        else
        {
            isEscaped = character == L'\\';
            isValid = character != L'"';
        }
        if (isValid && !isEscaped)
        {
            stringValue += character;
        }
    }
    ++index;
    return Token::CreateString(stringValue);
}

Token ParseMarkToken(wstring const& source, int& index)
{
    vector<wstring> twoCharacterMarks = {
        L"||", L"&&", L"==", L"!=", L"<=", L">=", L"//"};
    if (index + 1 < source.size())
    {
        for (auto const& mark : twoCharacterMarks)
        {
            if (source[index] == mark[0] && source[index + 1] == mark[1])
            {
                index += 2;
                return Token(mark);
            }
        }
    }

    if (index < source.size())
    {
        for (auto character : (wstring)L",.+-*/=<>!(){}[]&; \n\"")
        {
            if (source[index] == character)
            {
                ++index;
                return Token(wstring({character}));
            }
        }
    }
    return Token(L"error");
}

Token ParseGlobalVariableToken(wstring const& source, int& index)
{
    wstring variableName;
    bool isValid = true;
    // 下面这行判断的顺序很重要，isValid 不能移到右边。
    while (isValid && ++index != source.size())
    {
        auto character = source[index];
        isValid = iswalpha(character) || character == L'_' || iswdigit(character);
        if (isValid)
        {
            variableName += character;
        }
    }
    return Token::CreateVariable(variableName, true);
}

Token ParseVariableOrReservedToken(wstring const& source, int& index)
{
    wstring variableName = {source[index]};
    bool isValid = true;
    // 下面这行判断的顺序很重要，isValid 不能移到右边。
    while (isValid && ++index != source.size())
    {
        auto character = source[index];
        isValid = iswalpha(character) || character == L'_' || iswdigit(character);
        if (isValid)
        {
            variableName += character;
        }
    }

    bool isReserved =
        variableName == L"def" || variableName == L"return" || variableName == L"if" ||
        variableName == L"else" || variableName == L"while" || variableName == L"null";
    if (isReserved)
    {
        return Token(variableName);
    }
    return Token::CreateVariable(variableName, false);
}

Token ParseToken(wstring const& source, int& index)
{
    auto character = source[index];
    if (character == L'&')
    {
        return ParseGlobalVariableToken(source, index);
    }
    if (iswalpha(character) || character == L'_')
    {
        return ParseVariableOrReservedToken(source, index);
    }
    if (iswdigit(character))
    {
        return ParseNumberToken(source, index);
    }
    if (character == L'"')
    {
        return ParseStringToken(source, index);
    }
    return ParseMarkToken(source, index);
}

vector<Token> NormalizeSigns(vector<Token> const& tokens)
{
    vector<Token> result;
    bool isSign = false;
    int sign = 1;

    for (auto const& token : tokens)
    {
        bool previousIsSign = isSign;
        isSign = token.type == L"+" || token.type == L"-";
        if (isSign)
        {
            if (previousIsSign)
            {
                sign *= token.type == L"+" ? 1 : -1;
            }
            else
            {
                sign = token.type == L"+" ? 1 : -1;
            }
            continue;
        }
        else if (previousIsSign)
        {
            result.push_back(Token(sign == 1 ? L"+" : L"-"));
        }
        result.push_back(token);
    }
    if (isSign)
    {
        result.push_back(Token(sign == 1 ? L"+" : L"-"));
    }
    return result;
}

vector<Token> Tokenize(wstring const& source)
{
    int index = 0;
    vector<Token> tokens;
    while (index != source.size())
    {
        // 你之前在 token 前面加了 const &，千万不要在这样了。
        auto token = ParseToken(source, index);
        bool blank = token.type == L" " || token.type == L"\n";
        if (token.type == L"//")
        {
            while (index != source.size() && source[index++] != L'\n')
            {
            }
        }
        else if (token.type == L"error")
        {
            return {};
        }
        else if (!blank)
        {
            tokens.push_back(token);
        }
    }
    return NormalizeSigns(tokens);
}
