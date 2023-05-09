#include "../tokenizer/tokenizer.hpp"

void program(tokenizer* t);
void program_parts(tokenizer* t);
int exp(tokenizer* t);
void block_commands(tokenizer* t);
int set_command(tokenizer* t);
int func_call(tokenizer* t);