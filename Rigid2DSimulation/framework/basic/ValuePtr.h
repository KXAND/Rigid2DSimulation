#pragma once
#include "Utils.h"

// 移动相关的还没添加，对这方面不是很有把握。
template <class T>
struct ValuePtr
{
    std::shared_ptr<T> pointer;

    ValuePtr() = default;
    ValuePtr(T const& value)
    {
        pointer = std::make_shared<T>(value);
    }
    ValuePtr(ValuePtr const& other)
    {
        if (other)
        {
            pointer = std::make_shared<T>(*other);
        }
        else
        {
            pointer.reset();
        }
    }

    operator bool() const
    {
        return (bool)pointer;
    }

    void operator=(ValuePtr const& other)
    {
        if (other)
        {
            pointer = std::make_shared<T>(*other);
        }
        else
        {
            pointer.reset();
        }
    }

    std::shared_ptr<T>& operator->()
    {
        return pointer;
    }
    T& operator*()
    {
        return *pointer;
    }
    T const& operator*() const
    {
        return *pointer;
    }
};
