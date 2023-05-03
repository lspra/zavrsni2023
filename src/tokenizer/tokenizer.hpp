#include"token.hpp"

enum State {
    s0,
    word,
    number,
    floating_numb,
    string,
    operator_repeating
};

class tokenizer
{
private:
    int line = 1;
    int pos;
    State state;
    std::ifstream file;
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