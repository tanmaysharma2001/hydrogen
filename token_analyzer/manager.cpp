#include <iostream>
#include <vector>
#include <string>

#include "Token.h"
#include "IReader.h"
#include "readers/CommentBlockReader.cpp"
#include "readers/LineCommentReader.cpp"
#include "readers/IdentifierReader.cpp"
#include "readers/NumericReader.cpp"
#include "readers/OperandReader.cpp"

class Manager {

private:
    const std::vector<IReader*> readers = {
        new IdentifierReader(),
        new NumericReader(),
        new OperandReader(),
        new CommentBlockReader(),
        new LineCommentReader()
    };

    void error(int line, int column) {
        std::cout << "Invalid token at line " << line << " column " << column << "." << std::endl;
        std::exit(0);
    }

    std::vector<Token> GetTokens(const std::string& source) {
        std::vector<Token> tokens;
        IReader* reader = readers[0];
        std::string buffer = "";
        int line = 1;
        int lineI = 0;

        std::string modifiedSource = source;
        if (modifiedSource.back() != '\n')
            modifiedSource += '\n';

        for (int i = 0; i < modifiedSource.length() - 1;) {
            if (modifiedSource[i] == '\n') {
                line++;
                lineI = i;
            }

            if (reader == nullptr || !reader->read(modifiedSource, i, buffer)) {
                if (!buffer.empty()) {
                    std::string type = reader->getTokenType(buffer);
                    tokens.emplace_back(buffer, type, line, i - lineI);
                    buffer.clear();
                }

                if (reader != nullptr) {
                    if (reader->errorExists())
                        error(line, i - lineI);
                    reader->reset();
                    reader = nullptr;
                }

                if (modifiedSource[i] == ' ' || std::iscntrl(modifiedSource[i]))
                    i++;
                else {
                    for (IReader* r : readers) {
                        if (r->canStartParsing(modifiedSource, i)) {
                            reader = r;
                            break;
                        }
                    }
                    if (reader == nullptr)
                        error(line, i - lineI);
                }
            }
        }
        return tokens;
    }

public:
    Manager() {}

    std::vector<Token> Analyze(const std::string& source) {
        std::vector<Token> tokens = GetTokens(source);
        freopen("tokens.txt", "w", stdout);
        for (Token token : tokens) {
            std::cout << token.value << ' ' << token.type << ' ' << token.line << ' ' << token.column << std::endl; 
        }
        return tokens;
    }
};