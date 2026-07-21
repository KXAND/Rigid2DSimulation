#pragma once
#include "utils.h"

// 移动相关的还没添加，对这方面不是很有把握。
template<class T> struct ValuePtr {
	std::shared_ptr<T> p;
	ValuePtr() = default;
	ValuePtr(T const& t) { p = std::make_shared<T>(t); }
	ValuePtr(ValuePtr const& v) {
		if (v) { p = std::make_shared<T>(*v); } else { p.reset(); }
	}
	operator bool() const { return (bool)p; }
	void operator=(ValuePtr const& v) {
		if (v) { p = std::make_shared<T>(*v); } else { p.reset(); }
	}

	std::shared_ptr<T>& operator->() { return p; }
	T& operator*() { return *p; }
	T const& operator*() const { return *p; }
};


