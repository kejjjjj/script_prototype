#pragma once

#ifndef _datat
#define _datat

#include "pch.hpp"

struct customType
{
	const std::string& identifier;
	size_t size = 0;
	size_t numElements = 0;
};
enum class dataTypes_e
{
	
	INT,
	FLOAT,
	UNKNOWN
};
class dataTypeTable
{
public:
	static dataTypeTable& getInstance() {
		static dataTypeTable instance;
		return instance;
	}
	void insert(const customType& v) {
		table.insert(std::make_pair(v.identifier, v));
	}

	auto find(const std::string& v) const {
		auto found = table.find(v);
		return found != table.end() ? std::make_optional(found) : std::nullopt;
	}
	auto find_builtin(const std::string& v) const {
		auto found = std::find_if(built_in.begin(), built_in.end(), [&v](const std::string& str) { return !v.compare(str); });
		return found != built_in.end() ? std::make_optional(std::make_pair(found, std::distance(built_in.begin(), found))) : std::nullopt;
	}
	void erase(const std::string& v) noexcept(true) {
		table.erase(v);
	}

private:
	const std::list<std::string> built_in = {
		
		{"int"}, 
		{"float"} 
	};

	std::unordered_map<std::string, customType> table;
};

#endif