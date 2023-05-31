#include "records.hpp"

void program(tokenizer* t);
void program_parts(tokenizer* t, Scope* scope);
bool exp(tokenizer* t, Scope* scope);
void block_commands(tokenizer* t, Scope* scope);
void arguments(tokenizer* t, Scope* scope);