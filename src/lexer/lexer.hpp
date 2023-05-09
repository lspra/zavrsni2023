#include "../tokenizer/tokenizer.hpp"

void program(tokenizer* t);
void program_parts(tokenizer* t);
bool exp(tokenizer* t);
void block_commands(tokenizer* t);
bool set_command(tokenizer* t);
bool func_call(tokenizer* t);