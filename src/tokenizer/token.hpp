#include <string>
#include <unordered_set>

enum Token_Type
{
    KEYWORD,
    VAR,
    CONST,
    OPERATOR_MODIFY,
    OPERATOR,
    SQUARE_OPEN,
    SQUARE_CLOSE,
    CURLY_OPEN,
    CURLY_CLOSE,
    BRACKET_OPEN,
    BRACKET_CLOSE,
    SEMICOLON,
    COLON,
    COMMA
};

std::unordered_set <std::string> keywords = {"class", "public", 
    "private", "main", "operator", "data_type", "return", 
    "break", "continue", "if", "else", "for", "while", "input", "print"};

std::unordered_set <char> operator_modify = {'+', '-', '*', '/', '%', '|', '&', '^'};

std::unordered_set <std::string> operators = {
    "<<", ">>", "=", "==", "!=", "<", "<=", ">", ">="
};

struct Token
{
    int line;
    Token_Type type;
    std::string value;
};