#pragma once

#include <cstddef>
#include <exception>
#include <string>

class ScriptError : public std::exception
{
public:
    explicit ScriptError(std::wstring const& message) : message(message)
    {
    }

    char const* what() const noexcept override
    {
        return "DSL script error";
    }

    std::wstring const& GetMessage() const
    {
        return message;
    }

private:
    std::wstring message;
};

inline void RequireArgumentCount(std::wstring const& functionName, std::size_t expected, std::size_t actual)
{
    if (expected != actual)
    {
        throw ScriptError(functionName + L" expects " + std::to_wstring(expected) +
                          L" arguments, but received " + std::to_wstring(actual) + L".");
    }
}
