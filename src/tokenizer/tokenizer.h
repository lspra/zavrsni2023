#include"token.h"

class tokenizer
{
private:
    int line = 1;
    int pos;
    int state = 0;
    std::ifstream file;
    std::string cur_line;
    std::string filename;
    void readTokens();
    void change_state(char input, Token* t);
    void end_token(Token* t);
    Token_Type get_token_type();
public:
    tokenizer(std::string filename_) : filename(filename_)
    {
        file.open(filename);
        readTokens();
    }
    ~tokenizer();
    Token* get_next_token();
};