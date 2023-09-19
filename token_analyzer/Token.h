#include <string>

class Token {

public:
    std::string value;
    std::string type;
    int line;
    int column;

    Token(std::string value, std::string type, int line, int column) {
        this->value = value;
        this->type = type;
        this->line = line;
        this->column = column;
    }
};
