#include <string>
#include <cstdio>

class OperandReader : public IReader {
public:
    const std::string operands = ".,:=[]()";

    bool isOperand(char ch) {
        if (ch == '=') return false;
        for (const char& op : operands)
            if (op == ch)
                return true;
        return false;
    }

    bool errorExists() override { return false; }
    void reset() override { }

    bool canStartParsing(std::string source, int& idx) override {
        return isOperand(source[idx]);
    }

    bool read(std::string source, int& idx, std::string& buffer) override {
        char ch = source[idx];
        if (ch == ':' && source[idx + 1] == '=') {
            buffer = ":=";
            idx += 2;
        } else if (isOperand(ch)) {
            buffer += ch;
            ++idx;
        }
        return false; // maybe return true? and then return false?
    }

    std::string getTokenType(std::string value) override {
        return "operand";
    }
};