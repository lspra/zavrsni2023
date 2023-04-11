#include <string>
#include <fstream>
#include <iostream>
#include "tokenizer.h"
#include "transitions.h"

tokenizer::~tokenizer()
{
    file.close();
}

Token *tokenizer::get_next_token()
{
    Token *t = new Token();
    if(pos >= cur_line.size())
        readTokens();
    for (; pos < cur_line.size(); pos++)
    {
        if(state == STRING)
        {
            //TODO add escaping
            while(cur_line[pos] != '"')
            {
                t->value += cur_line[pos++];
                if(pos >= cur_line.size())
                    readTokens();
            }
            t->value += cur_line[pos];
            change_state(cur_line[pos++], t);
        }
        if(white_space.contains(cur_line[pos]))
        {
            if(state != 0)
                end_token(t);
            if(cur_line[pos] == '\n')
                line++;
        }
        else if(double_operators.contains(cur_line[pos]))
        {
            if(state != 0 && state != OPERATOR_MODIFY)
                end_token(t);
            else
                change_state(cur_line[pos], t);
        }
        else if(single_op.contains(cur_line[pos]))
        {
            if(state != 0)
                end_token(t);
            else
                change_state(cur_line[pos], t);
        }
        else if(op.contains(cur_line[pos]))
        {
            if(op_states.contains(state) || state == 0)
                change_state(cur_line[pos], t);
            else
                end_token(t);
        }
        else
            change_state(cur_line[pos], t);

        if(t->line != 0)
            return t;
        t->value += cur_line[pos];
    }
    end_token(t);
    return t;
}

void tokenizer::readTokens()
{
    file >> cur_line;
    pos = 0;
}

Token_Type tokenizer::get_token_type()
{
    if(state <= 57)
        return VARIABLE;
    switch (state)
    {
    case OP_END:
    case OP_LETTERS:
    case 64:
    case 65:
    case 66:
        return OPERATOR;
    case STRING_END:
    case 58:
    case 60:
        return CONST;
    case 59:
    case 61:
    case 62:
        std::cerr << "TODO add error";
        return OPERATOR;
    default:
        return (Token_Type) state;
    }
}

void tokenizer::end_token(Token* t)
{
    t->type = get_token_type();
    t->line = line;
    state = 0;
}

void tokenizer::change_state(char input, Token* t)
{
    if(state == 0)
    {
        if(transitions[state].contains(input))
            state = transitions[state].at(input);
        else if(numbers.contains(input))
            state = INT;
        else if(letters.contains(input))
            state = VARIABLE;
        else
            std::cerr << "error - continueing" << std::endl;
    }
    else if(state == OPERATOR_MODIFY && input != t->value[t->value.length() - 1])
        end_token(t);
    else if(state == OP_END || state == STRING_END || state == BRACKET_CLOSE || state == BRACKET_OPEN || state == CURLY_CLOSE || state == CURLY_OPEN || state == CURLY_CLOSE || state == CURLY_OPEN)
        end_token(t);
    else if(state == INT && !numbers.contains(input))
    {
        if(transitions[state].contains(input))
            state = transitions[state].at(input);
        else
            end_token(t);
    }
    else if(state == FLOATING_POINT)
    {
        if(numbers.contains(input))
            state = FLOAT;
        else
            end_token(t);
    }
    else if(state == FLOAT && !numbers.contains(input))
        end_token(t);
    else if(state == STRING)
    {
        // TODO add escaping
        if(input == '"')
            state = STRING_END;
    }
    else if(state == 62)
    {
        if(letters.contains(input))
            state = VARIABLE;
        else
            end_token(t);
    }
    else if(op_states.contains(state))
    {
        if (transitions[state].contains(input))
            state = transitions[state].at(input);
        else
            end_token(t);
    }
    else if(transitions[state].contains(input))
        state = transitions[state].at(input);
    else if(variable_char.contains(input))
        state = VARIABLE;
    else
        end_token(t);
}