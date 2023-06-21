#pragma once

#ifndef __variabl_
#define __variabl_

#include "pch.hpp"
#include "rvalue.hpp"

struct var_declr_data
{
	dataTypes_e type = dataTypes_e::UNKNOWN;
	const std::string& name;
	size_t size = 0ull; //optional unless the datatype is not a constant size

};

class Variable
{
public:
	Variable() = delete;
	//Variable(const Variable&) = delete;
	//Variable(Variable&) = delete;
	Variable(const var_declr_data& init_data);


	std::string identifier;
private:
	VariableValue value;
	dataTypes_e type = dataTypes_e::UNKNOWN;

};

class VariableTable
{
public:
	static VariableTable& getInstance() {
		static VariableTable instance;
		return instance;
	}
	void insert(const Variable& v) {
		table.insert(std::make_pair(v.identifier, v));
	}

	auto find(const std::string& v) const {
		auto found = table.find(v);
		return found != table.end() ? std::make_optional(found) : std::nullopt;
	}

	void erase(const std::string& v) noexcept(true) {
		table.erase(v);
	}

private:
	std::unordered_map<std::string, Variable> table;
};

#endif