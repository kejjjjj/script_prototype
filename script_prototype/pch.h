#pragma once

#ifndef PCH_H
#define PCH_H

#include <Windows.h>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <string_view>
#include <filesystem>
#include <direct.h>
#include <array>
#include <list>
#include <ranges>


#include "filetools.hpp"
#include "variables.hpp"
#include "common.hpp"
#include "expressions.hpp"
#include "eval_statement.hpp"
#include "compiler_expressions.hpp"
#include "runtime_expressions.hpp"
#include "compiler_whenstatement.hpp"
#include "when_statement.hpp"
#include "runtime.hpp"
#include "compiler.hpp"
#include "allocations.hpp"


#endif