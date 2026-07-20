#pragma once
#include "utils.h"

// 移动相关的还没添加，对这方面不是很有把握。
template<class T> struct vptr {
	std::shared_ptr<T> p;
	vptr() = default;
	vptr(T const& t) { p = std::make_shared<T>(t); }
	vptr(vptr const& v) {
		if (v) { p = std::make_shared<T>(*v); } else { p.reset(); }
	}
	operator bool() const { return (bool)p; }
	void operator=(vptr const& v) {
		if (v) { p = std::make_shared<T>(*v); } else { p.reset(); }
	}

	std::shared_ptr<T>& operator->() { return p; }
	T& operator*() { return *p; }
	T const& operator*() const { return *p; }
};


