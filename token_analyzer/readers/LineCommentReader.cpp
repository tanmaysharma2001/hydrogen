#include <string>

class LineCommentReader : public IReader {
public:
    bool errorExists() override { return false; }
    void reset() override {}

    bool canStartParsing(std::string source, int& idx) override {
        if (source[idx] == '/' && source[idx + 1] == '/') { // start of single-line comment
            idx += 2;
            return true;
        }
        return false;
    }

    bool read(std::string source, int& idx, std::string& buffer) override {
        return (source[idx++] != '\n');
    }

    std::string getTokenType(std::string value) override { return ""; }
};