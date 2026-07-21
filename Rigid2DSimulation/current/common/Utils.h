#pragma once
#include "Value.h"

inline std::shared_ptr<Value> FindConfigValue(Value const& configuration, wchar_t const* key)
{
	auto it = configuration.dictionary.find(key);
	return it == configuration.dictionary.end() ? std::shared_ptr<Value>() : it->second;
}

template<class T>
inline bool TryReadConfigNumber(Value const& configuration, wchar_t const* key, T& output)
{
	auto value = FindConfigValue(configuration, key);
	if (!value) { return false; }
	output = value->number;
	return true;
}