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
            while(cur_line[pos] != '"')
            {
                t->value += cur_line[pos++];
                if(pos >= cur_line.size())
                    readTokens();
            }
            t->value += cur_line[pos];
            change_state(cur_line[pos++], t);
        }
        if(white_space.count(cur_line[pos]))
        {
            if(state != 0)
                end_token(t);
            if(cur_line[pos] == '\n')
                line++;
        }
        else if(double_operators.count(cur_line[pos]))
        {
            if(state != 0 && state != DOUBLE_OP)
                end_token(t);
            else
                change_state(cur_line[pos], t);
        }
        else if(single_op.count(cur_line[pos]))
        {
            if(state != 0)
                end_token(t);
            else
                change_state(cur_line[pos], t);
        }
        else if(op.count(cur_line[pos]))
        {
            if(op_states.count(state) || state == 0)
                change_state(cur_line[pos], t);
            else
                end_token(t);
        }
        else
            change_state(cur_line[pos], t);

        if(t->line != 0)
            return t;
        t->value += cur_line[pos];
        // std::cout << cur_line[pos] << " " << state << std::endl;
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
    case DOUBLE_OP:
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
        try
        {
            state = transitions[state].at(input);
        }
        catch(const std::out_of_range& e)
        {
            if(numbers.count(input))
                state = INT;
            else if(letters.count(input))
                state = VARIABLE;
            else
                std::cerr << "error - continueing" << std::endl;
        }
        
    }
    else if(state == DOUBLE_OP)
    {
        if(input != t->value[t->value.length() - 1])
            end_token(t);
    }
    else if(state == OP_END || state == STRING_END || state == BRACKET_CLOSE || state == BRACKET_OPEN || state == CURLY_CLOSE || state == CURLY_OPEN || state == CURLY_CLOSE || state == CURLY_OPEN)
    {
        end_token(t);
    }
    else if(state == INT)
    {
        if(!numbers.count(input))
        {
            try
            {
                state = transitions[state].at(input);
            }
            catch(const std::out_of_range& e)
            {
                end_token(t);
            }
        }
    }
    else if(state == FLOATING_POINT)
    {
        if(numbers.count(input))
            state = FLOAT;
        else
            end_token(t);
    }
    else if(state == FLOAT)
    {
        if(!numbers.count(input))
            end_token(t);
    }
    else if(state == STRING)
    {
        if(input == '"')
            state = STRING_END;
    }
    else if(state == 62)
    {
        if(letters.count(input))
            state = VARIABLE;
        else
            end_token(t);
    }
    else if(op_states.count(state))
    {
        if (transitions[state].contains(input))
            state = transitions[state].at(input);
        else
            end_token(t);
    }
    else
    {
        try
        {
            state = transitions[state].at(input);
        }
        catch(const std::out_of_range& e)
        {
            if(variable_char.count(input))
                state = VARIABLE;
            else
                end_token(t);
        }
    }
}