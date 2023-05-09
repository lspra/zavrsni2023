#include <string>
#include <iostream>
#include <cctype>
#include "tokenizer.hpp"

std::unordered_set <std::string> keywords = {
    "class", "public", "private", "main", "return", "break", 
    "continue", "if", "else", "for", "while", "input", "print"
};

std::unordered_set <char> operator_modify = {'+', '-', '*', '/', '%', '|', '&', '^'};

std::unordered_set <std::string> data_types = {
    "bool", "int8", "int16", "int32", "int64", "uint8", "uint16",
    "uint32", "uint64", "float32", "float64", "float96", "string"
};

std::unordered_set <std::string> operators = {
    "<<", ">>", "=", "==", "!=", "<", "<=", ">", ">=", "~"
};

tokenizer::~tokenizer()
{
    file.close();
}

Token *tokenizer::get_next_token()
{
    Token *t = new Token();
    bool string_escaping;
    if (pos >= cur_line.size())
        readTokens();
    if(state == end_of_file) {
        end_token(t);
        return t;
    }
    for (; pos < cur_line.size(); pos++) {
        switch (state)
        {
        case s0:
            if (isalpha(cur_line[pos])) {
                state = word;
                t->value += cur_line[pos];
            } 
            else if (isdigit(cur_line[pos])) {
                state = number;
                t->value += cur_line[pos];
            }
            else if (cur_line[pos] == '"') {
                state = string;
                string_escaping = false;
            }
            else if (operator_modify.contains(cur_line[pos])) {
                state = operator_repeating;
                t->value += cur_line[pos];
            }
            else if (cur_line[pos] == '#') {
                // this is a comment, moving to next line
                readTokens();
            }
            else if (cur_line[pos] == '<' || cur_line[pos] == '>') {
                if (cur_line[pos + 1] == '=' || cur_line[pos + 1] == cur_line[pos]) {
                    t->value += cur_line[pos];
                    t->value += cur_line[pos + 1];
                    pos += 2;
                    end_token(t);
                    return t;
                } else {
                    t->value = cur_line[pos++];
                    end_token(t);
                    return t;
                }
            }
            else if (cur_line[pos] == '=' || cur_line[pos] == '!') {
                if (cur_line[pos + 1] == '=') {
                    t->value += cur_line[pos];
                    t->value += cur_line[pos + 1];
                    pos += 2;
                    end_token(t);
                    return t;
                } else {
                    t->value = cur_line[pos++];
                    end_token(t);
                    return t;
                }
            }
            else if (cur_line[pos] == ' ' || cur_line[pos] == '\t' || cur_line[pos] == '\n')
                continue;
            else {
                t->value += cur_line[pos++];
                end_token(t);
                return t;
            }
            break;
        case word:
            if (isalnum(cur_line[pos]) || cur_line[pos] == '_') 
                t->value += cur_line[pos];
            else {
                end_token(t);
                return t;
            }
            break;
        case number:
            if (isdigit(cur_line[pos]))
                t->value += cur_line[pos];
            else if (cur_line[pos] == '.' || cur_line[pos] == ',') {
                state = floating_numb;
                t->value += cur_line[pos];
            }
            else {
                end_token(t);
                return t;
            }
            break;
        case floating_numb:
            if (isdigit(cur_line[pos])) 
                t->value += cur_line[pos];
            else {
                end_token(t);
                return t;
            }
            break;
        case string:
            if (string_escaping) {
                if (cur_line[pos] == '"')
                    t->value += cur_line[pos];
                else {
                    t->value += '\\';
                    t->value += cur_line[pos];
                }
                string_escaping = false;
            } else {
                if (cur_line[pos] == '"') {
                    pos++;
                    end_token(t);
                    return t;
                } else {
                    t->value += cur_line[pos];
                }
            }
            break;
        case operator_repeating:
            if (t->value[0] == cur_line[pos])
                t->value += cur_line[pos];
            else {
                end_token(t);
                return t;
            }
        } 
    }
    end_token(t);
    return t;
}

void tokenizer::readTokens()
{
    pos = 0;
    do {
        if(file.eof())
            state = end_of_file;
        std::getline(file, cur_line);
        line++;
    } while (pos >= cur_line.length());
}

void tokenizer::set_token_type(Token* t)
{
    if (state == word) {
        if (keywords.contains(t->value))
            t->type = KEYWORD;
        else if(data_types.contains(t->value))
            t->type = DATA_TYPE;
        else if (t->value == "or" || t->value == "xor" ||
                t->value == "not" || t->value == "and") 
            t->type = OPERATOR;
        else
            t->type = VAR;
    }
    if(state == end_of_file) 
        t->type = END_OF_FILE;
    if (state == number)
        t->type = INT;
    if(state == floating_numb)
        t->type = FLOAT;
    if(state == string)
        t->type = STRING;
    if (state == operator_repeating) {
        if(t->value.length() == 1)
            t->type = Token_Type::OPERATOR;
        else
            t->type = OPERATOR_MODIFY;
    }
    if (state == s0) {
        if(operators.contains(t->value))
            t->type = OPERATOR;
        else if(t->value == "[")
            t->type = SQUARE_OPEN;
        else if(t->value == "]")
            t->type = SQUARE_CLOSE;
        else if(t->value == "{")
            t->type = CURLY_OPEN;
        else if(t->value == "}")
            t->type = CURLY_CLOSE;
        else if(t->value == "(")
            t->type = BRACKET_OPEN;
        else if(t->value == ")")
            t->type = BRACKET_CLOSE;
        else if(t->value == ";")
            t->type = SEMICOLON;
        else if(t->value == ":")
            t->type = COLON;
        else if(t->value == ",")
            t->type = COMMA;
        else if(t->value == ".")
            t->type = DOT;
        else {
            std::cerr << "Error in line: " << line << " - following character is not an operator: " << t->value;
            exit(1);
        }
    }
}

void tokenizer::end_token(Token* t)
{
    set_token_type(t);
    t->line = line;
    if(state != end_of_file)
        state = s0;
}