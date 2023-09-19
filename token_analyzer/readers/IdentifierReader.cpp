#include <string>
#include <set>
#include <cstdio>

class IdentifierReader : public IReader {
public:
    std::set<std::string> keywords;
    IdentifierReader() {
        keywords.insert("class");
        keywords.insert("if");
        keywords.insert("else");
        keywords.insert("then");
        keywords.insert("return");
        keywords.insert("extends");
        keywords.insert("is");
        keywords.insert("end");
        keywords.insert("var");
        keywords.insert("method");
        keywords.insert("this");
        keywords.insert("while");
        keywords.insert("loop");
    }
    

    bool errorExists() override { return false; }
    void reset() override {}

    bool canStartParsing(std::string source, int& idx) override {
        return (source[idx] == '_' || isalpha(source[idx]));
    }

    bool read(std::string source, int& idx, std::string& buffer) override {       
        char ch = source[idx];
        if (ch == '_' || isdigit(ch) || isalpha(ch)) { // read identifier
            buffer += ch;
            ++idx;
            return true;
        } 
        return false;
    }

    std::string getTokenType(std::string value) override { 
        if (value == "true" || value == "false") return "bool";
        if (keywords.count(value))
            return "keyword";
        return "name";
    }
};