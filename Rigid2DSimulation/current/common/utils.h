#pragma once
#include "var.h"

inline std::shared_ptr<Var> FindConfigValue(Var const& cfg, wchar_t const* key)
{
	auto it = cfg.dic.find(key);
	return it == cfg.dic.end() ? std::shared_ptr<Var>() : it->second;
}

template<class T>
inline bool TryReadConfigNumber(Var const& cfg, wchar_t const* key, T& out)
{
	auto value = FindConfigValue(cfg, key);
	if (!value) { return false; }
	out = value->num;
	return true;
}