#pragma once
#include "var.h"

inline ptr<Var> find_config_value(Var const& cfg, wchar_t const* key)
{
	auto it = cfg.dic.find(key);
	return it == cfg.dic.end() ? ptr<Var>() : it->second;
}

template<class T>
inline bool try_read_config_number(Var const& cfg, wchar_t const* key, T& out)
{
	auto value = find_config_value(cfg, key);
	if (!value) { return false; }
	out = value->num;
	return true;
}