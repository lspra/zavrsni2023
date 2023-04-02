#include <unordered_map>
#include <unordered_set>

#define TOTAL_TOKENIZER_STATES 67
#define OP_LETTERS 78
#define OP_END 80
#define DOUBLE_OP 81
#define COMMENT 82
#define STRING_END 83
#define INT 58
#define FLOATING_POINT 59
#define FLOAT 60
#define STRING 61

std::unordered_map<char, int> const transitions[TOTAL_TOKENIZER_STATES] = {
    {{'i', 1}, {'u', 9}, {'b', 12}, {'f', 15}, {'s', 24}, {'w', 29}, {'n', 33}, {'a', 35}, {'o', 38}, {'x', 39}, {'c', 40}, {'p', 44}, {'e', 55}, 
    {'"', STRING}, {'_', 62}, {'<', 64}, {'>', 65}, {'=', 66}, {'!', 66}, {'~', OP_END}, {':', OP_END}, {'+', DOUBLE_OP}, {'-', DOUBLE_OP}, 
    {'*', DOUBLE_OP}, {'/', DOUBLE_OP}, {'%', DOUBLE_OP}, {'&', DOUBLE_OP}, {'|', DOUBLE_OP}, {'^', DOUBLE_OP}, {'#', COMMENT}, {':', COLON},
    {'[', SQUARE_OPEN}, {']', SQUARE_CLOSE}, {'{', CURLY_OPEN}, {'}', CURLY_CLOSE}, {'(', BRACKET_OPEN}, {')', BRACKET_CLOSE}, {';', SEMICOLON}},
    {{'n', 2}, {'f', IF}},
    {{'t', 3}, {'p', 7}},
    {{'8', DATA_TYPE}, {'1', 4}, {'3', 5}, {'6', 6}},
    {{'6', DATA_TYPE}},
    {{'2', DATA_TYPE}},
    {{'4', DATA_TYPE}},
    {{'u', 8}},
    {{'t', INPUT}},
    {{'i', 10}},
    {{'n', 11}},
    {{'t', 3}},
    {{'o', 13}},
    {{'o', 14}},
    {{'l', DATA_TYPE}},
    {{'l', 16}, {'o', 23}},
    {{'o', 17}},
    {{'a', 18}},
    {{'t', 19}},
    {{'3', 20}, {'6', 21}, {'9', 22}},
    {{'2', DATA_TYPE}},
    {{'4', DATA_TYPE}},
    {{'6', DATA_TYPE}},
    {{'r', FOR}},
    {{'t', 25}},
    {{'r', 26}},
    {{'i', 27}},
    {{'n', 28}},
    {{'g', DATA_TYPE}},
    {{'h', 30}},
    {{'i', 31}},
    {{'l', 32}},
    {{'e', WHILE}},
    {{'o', 34}},
    {{'t', OP_LETTERS}},
    {{'n', 36}},
    {{'d', OP_LETTERS}},
    {},
    {{'r', OP_LETTERS}},
    {{'o', 38}},
    {{'l', 41}},
    {{'a', 42}},
    {{'s', 43}},
    {{'s', CLASS}},
    {{'u', 45}, {'r', 49}},
    {{'b', 46}},
    {{'l', 47}},
    {{'i', 48}},
    {{'c', PUBLIC}},
    {{'i', 50}},
    {{'v', 51}, {'n', 54}},
    {{'a', 52}},
    {{'t', 53}},
    {{'e', PRIVATE}},
    {{'t', PRINT}},
    {{'l', 56}},
    {{'s', 57}},
    {{'e', ELSE}},
    {{'.', FLOATING_POINT}},
    {},
    {},
    {{'"', STRING_END}},
    {},
    {},
    {{'<', OP_END}, {'=', OP_END}},
    {{'>', OP_END}, {'=', OP_END}},
    {{'=', OP_END}}
    };

std::unordered_set<char> const numbers = {'1', '2', '3', '4', '5', '6', '7', '8', '9', '0'};
std::unordered_set<char> const letters = {'q', 'w', 'e', 'r', 't', 'z', 'u', 'i', 'o', 'p', 'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', 'y', 'x', 'c', 'v', 'b', 'n', 'm', 'Q', 'W', 'E', 'R', 'T', 'Z', 'U', 'I', 'O', 'P', 'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', 'Y', 'X', 'C', 'V', 'B', 'N', 'M'};
std::unordered_set<char> const double_operators = {'+', '-', '*', '/', '%', '&', '|', '^'};
std::unordered_set<char> const white_space = {' ', '\t', '\n'};
std::unordered_set<char> const op = {'<', '>', '='};
std::unordered_set<char> const single_op = {';', '(', ')', '[', ']', '{', '}', ':', '#', '"'};
std::unordered_set<int> const op_states = {64, 65, 66};
std::unordered_set<char> const variable_char = {'q', 'w', 'e', 'r', 't', 'z', 'u', 'i', 'o', 'p', 'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', 'y', 'x', 'c', 'v', 'b', 'n', 'm', 'Q', 'W', 'E', 'R', 'T', 'Z', 'U', 'I', 'O', 'P', 'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', 'Y', 'X', 'C', 'V', 'B', 'N', 'M', '_', '1', '2', '3', '4', '5', '6', '7', '8', '9', '0'};