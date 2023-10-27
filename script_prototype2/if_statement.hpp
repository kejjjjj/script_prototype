#ifndef istatment
#define istatment

#include "script.hpp"
#include "scope.hpp"


void eval_if_statement(script_t& script, scr_scope_t*& block, code_segment_t code);
void eval_else_statement(script_t& script, scr_scope_t*& block, code_segment_t code);



#endif