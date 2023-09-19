#include <string>

class CommentBlockReader : public IReader {
public:
    bool errorExists() override { return false; }
    void reset() override {}

    bool canStartParsing(std::string source, int& idx) override {
        if (source[idx] == '/' && source[idx + 1] == '*') { // start of comment block
            idx += 2;
            return true;
        }
        return false;
    }

    bool read(std::string source, int& idx, std::string& buffer) override {
        return (source[idx++] == '*' && source[idx++] == '/'); // end of comment block
    }

    std::string getTokenType(std::string value) override { return ""; }
};