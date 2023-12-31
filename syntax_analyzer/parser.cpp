#include <vector>
#include <string>
#include <iostream>

// include order is important
// they are ordered by dependency

#include "Node.h"
#include "SyntaxToken.cpp"

#include "nodes/CallAndExpressionNode.cpp"
#include "nodes/LoopNode.cpp"
#include "nodes/ConstructorNode.cpp"
#include "nodes/MethodNode.cpp"
#include "nodes/AssignmentNode.cpp"
#include "nodes/VariableNode.cpp"
#include "nodes/ClassNode.cpp"
#include "nodes/ConditionalNode.cpp"
#include "nodes/ReturnStatementNode.cpp"
#include "nodes/RootNode.cpp"

#include "parsers/ExpressionAndCallParser.cpp"
#include "parsers/AssignmentParser.cpp"
#include "parsers/VariableParser.cpp"
#include "parsers/ReturnStatementParser.cpp"
#include "parsers/BodyParser.cpp"
#include "parsers/ClassParser.cpp"

#include "../token_analyzer/manager.cpp"

class Parser {
public:

    Parser() {}

    RootNode* startParsing(std::vector<Token> tokens) {
        std::vector<std::string> tokenValues;
        std::vector<std::string> tokenTypes;
        std::vector<int> tokenLines;
        std::vector<int> tokenColumns;

        for (Token token : tokens) {
            tokenValues.push_back(token.value);
            tokenTypes.push_back(token.type);
            tokenLines.push_back(token.line);
            tokenColumns.push_back(token.column);
        }

        SyntaxToken tokenObj(tokenValues, tokenTypes, tokenLines, tokenColumns);

        const auto exitWithError = [&](int line) {
            std::cout << "Syntax Error on line " << line << "." << std::endl;
            std::exit(0);
        };

        int tokenNumber = 0;
        RootNode* node = new RootNode();

        while (true) {
            ClassNode* classNode = ClassParser::parse(tokenNumber, tokenObj);
            if (classNode->parsingError) {
                exitWithError(classNode->errorLine);
            } else {
                node->addClassDeclaration(classNode);
                tokenNumber = classNode->tokenNumber;
            }
            if (tokenNumber >= (int) tokens.size()) {
                break;
            }
        }

        // std::cout << "No Syntax Error Found" << std::endl << std::endl;
        // std::cout << "Abstract Syntax Tree (children nested with tabs)" << std::endl << std::endl;
        node->print("");

        return node;
    }
};