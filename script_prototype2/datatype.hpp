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
enum class dataTypes_e : char
{
	CHAR,
	INT,
	FLOAT,
	UNKNOWN
};
dataTypes_e typeFromIntegerSuffix(const DWORD typeInfo);
std::string get_type_as_text(const dataTypes_e type);
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

	auto& get_list() const noexcept(true) {
		return built_in;
	};

private:
	const std::list<std::string> built_in = {
		{"char"},
		{"int"}, 
		{"float"} 
	};

	std::unordered_map<std::string, customType> table;
};

enum class declaration_modifiers_e : char
{
	ARRAY
};

struct datatype_declaration
{
	dataTypes_e dtype = dataTypes_e::UNKNOWN;
	std::list<declaration_modifiers_e> typeModifiers;
	
	bool has_same_modifiers(const datatype_declaration& other) const noexcept {

		if (typeModifiers.size() != other.typeModifiers.size())
			return false;

		auto other_it = other.typeModifiers.begin();
		for (auto it = typeModifiers.begin(); it != typeModifiers.end(); it++, other_it++) {

			if (*it != *other_it)
				return false;

		}
		return true;
	}

	std::string get_as_text() const noexcept {

		std::string type = get_type_as_text(dtype);

		if (typeModifiers.empty())
			return type;

		std::string modifiers;
		for (auto& i : typeModifiers) {
			switch (i) {
			case declaration_modifiers_e::ARRAY:
				modifiers += "[]";
				break;
			default:
				break;
			}
		}

		return type + modifiers;
	}

};

#endif