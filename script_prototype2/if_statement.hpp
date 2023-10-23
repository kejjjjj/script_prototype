#ifndef istatment
#define istatment

#include "script.hpp"
#include "scope.hpp"

void eval_if_statement(script_t& script, scr_scope_t*& block, code_segment_t code);
void create_if_statement(script_t& script, scr_scope_t* scope, code_segment_t& _code);

#endif