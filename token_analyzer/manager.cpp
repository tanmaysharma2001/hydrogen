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

    std::vector<Token> getTokens(std::string& source) {
        std::vector<Token> tokens;
        IReader* reader = readers[0];
        std::string buffer = "";
        int line = 1;
        int colPos = 0;

        if (source.back() != '\n')
            source += '\n';

        int i = 0;
        for (; i < source.length() - 1;) {
            if (source[i] == '\n') {
                line++;
                colPos = i;
            }

            if (reader == nullptr || !reader->read(source, i, buffer)) {
                if (!buffer.empty()) {
                    std::string type = reader->getTokenType(buffer);
                    tokens.emplace_back(buffer, type, line, i - colPos);
                    buffer.clear();
                }

                if (reader != nullptr) {
                    if (reader->errorExists())
                        error(line, i - colPos);
                    reader->reset();
                    reader = nullptr;
                }

                if (source[i] == ' ' || std::iscntrl(source[i])) {
                    i++;
                } else {
                    for (IReader* r : readers) {
                        if (r->canStartParsing(source, i)) {
                            reader = r;
                            break;
                        }
                    }
                    if (reader == nullptr)
                        error(line, i - colPos);
                }
            }
        }

        if (!buffer.empty()) {
            std::string type = reader->getTokenType(buffer);
            tokens.emplace_back(buffer, type, line, i - colPos);
            buffer.clear();
        }
        return tokens;
    }

public:
    Manager() {}

    std::vector<Token> analyze(std::string& source) {
        std::vector<Token> tokens = getTokens(source);
        freopen("tokens.txt", "w", stdout);
        for (Token token : tokens) {
            std::cout << token.value << ' ' << token.type << ' ' << token.line << ' ' << token.column << std::endl; 
        }
        return tokens;
    }
};