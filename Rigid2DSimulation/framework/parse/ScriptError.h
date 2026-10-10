#pragma once

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
