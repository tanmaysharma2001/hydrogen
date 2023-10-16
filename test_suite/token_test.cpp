#include <iostream>
#include <vector>
#include <string>

#include "../token_analyzer/manager.cpp"

int main(int argc, char** argv) {
    std::string file = argv[1];

    Manager manager;
    std::string source;

    freopen(file.c_str(), "r", stdin);

    char ch;
    while (EOF != (ch = getchar())) {
        source += ch;
    }

    std::vector<Token> tokens = manager.analyze(source);

    freopen("tokens.txt", "w", stdout);
    for (Token token : tokens) {
        std::cout << token.value << ' ' << token.type << ' ' << token.line << ' ' << token.column << std::endl;
    }
}