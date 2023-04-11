#include<string>
enum Token_Type
{
    OPERATOR,
    CONST,
    IF = 68,
    DATA_TYPE = 69,
    INPUT = 70,
    FOR = 71,
    WHILE = 72,
    CLASS,
    PUBLIC,
    PRIVATE,
    PRINT,
    ELSE,
    OPERATOR_MODIFY = 81,
    SQUARE_OPEN = 84,
    SQUARE_CLOSE,
    CURLY_OPEN,
    CURLY_CLOSE,
    BRACKET_OPEN,
    BRACKET_CLOSE,
    SEMICOLON,
    COLON,
    VARIABLE = 63
};

struct Token
{
    int line;
    Token_Type type;
    std::string value;
};
