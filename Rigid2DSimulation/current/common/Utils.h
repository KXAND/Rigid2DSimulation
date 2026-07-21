#pragma once
#include "Value.h"

inline std::shared_ptr<Value> FindConfigValue(Value const& cfg, wchar_t const* key)
{
	auto it = cfg.dic.find(key);
	return it == cfg.dic.end() ? std::shared_ptr<Value>() : it->second;
}

template<class T>
inline bool TryReadConfigNumber(Value const& cfg, wchar_t const* key, T& out)
{
	auto value = FindConfigValue(cfg, key);
	if (!value) { return false; }
	out = value->num;
	return true;
}