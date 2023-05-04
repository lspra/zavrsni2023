#include<fstream>
#include<unordered_set>
#include<string>

enum Token_Type
{
    KEYWORD,
    DATA_TYPE,
    VAR,
    INT,
    FLOAT,
    STRING,
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
    COMMA,
    DOT
};

struct Token
{
    int line;
    Token_Type type;
    std::string value;
};

enum State {
    s0,
    word,
    number,
    floating_numb,
    string,
    operator_repeating,
    end_of_file
};

class tokenizer
{
private:
    int line = 0;
    int pos = 0;
    State state;
    std::fstream file;
    std::string cur_line;
    std::string filename;
    void readTokens();
    void end_token(Token* t);
    void set_token_type(Token* t);
public:
    tokenizer(std::string filename_) : filename(filename_)
    {
        state = s0;
        file.open(filename);
    }
    ~tokenizer();
    Token* get_next_token();
};