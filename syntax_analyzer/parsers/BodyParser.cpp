class BodyParser {
public:
    static std::vector<Node*> parse(int& tokenNumber, SyntaxToken &tokenObj);
};

class LoopParser {
public:
    static LoopNode* parse(int& tokenNumber, SyntaxToken& tokenObj);
};

class ConditionalParser {
public:
    static ConditionalNode* parse(int& tokenNumber, SyntaxToken& tokenObj);
};

class ConstructorParser {
public:
    static ConstructorNode* parse(int& tokenNumber, SyntaxToken& tokenObj);
};

class MethodParser {
public:
    static MethodNode* parse(int& tokenNumber, SyntaxToken& tokenObj);
};

std::vector<Node*> BodyParser::parse(int& tokenNumber, SyntaxToken &tokenObj) {
    std::vector<Node*> bodyNodes;
    while (true) {
        if (!(tokenObj.validToken(tokenNumber, "type", "name", 0) ||
                tokenObj.validToken(tokenNumber, "value", "while", 0) ||
                tokenObj.validToken(tokenNumber, "value", "if", 0) ||
                tokenObj.validToken(tokenNumber, "value", "return", 0) ||
                tokenObj.validToken(tokenNumber, "value", "var", 0) ||
                tokenObj.validToken(tokenNumber, "value", "this", 0))) {
            return bodyNodes;
        }
        VariableNode* variableNode = VariableParser::parse(tokenNumber, tokenObj);
        if (!variableNode->parsingError) {
            bodyNodes.push_back(variableNode);
            tokenNumber = variableNode->tokenNumber;
            continue;
        }
        AssignmentNode* assignmentNode = AssignmentParser::parse(tokenNumber, tokenObj);
        if (!assignmentNode->parsingError) {
            bodyNodes.push_back(assignmentNode);
            tokenNumber = assignmentNode->tokenNumber;
            continue;
        }
        LoopNode* loopNode = LoopParser::parse(tokenNumber, tokenObj);
        if (!loopNode->parsingError) {
            bodyNodes.push_back(loopNode);
            tokenNumber = loopNode->tokenNumber;
            continue;
        }
        ConditionalNode* conditionalNode = ConditionalParser::parse(tokenNumber, tokenObj);
        if (!conditionalNode->parsingError) {
            bodyNodes.push_back(conditionalNode);
            tokenNumber = conditionalNode->tokenNumber;
            continue;
        }
        ReturnStatementNode* returnNode = ReturnStatementParser::parse(tokenNumber, tokenObj);
        if (!returnNode->parsingError) {
            bodyNodes.push_back(returnNode);
            tokenNumber = returnNode->tokenNumber;
            continue;
        }
        CallNode* callNode = CallParser::parse(tokenNumber, tokenObj);
        if (callNode->parsingError) {
            Node* node = new Node();
            node->parsingError = true;
            node->errorLine = std::max(std::max(variableNode->errorLine, assignmentNode->errorLine),
                                      std::max(loopNode->errorLine, conditionalNode->errorLine));
            node->errorLine = std::max(std::max(returnNode->errorLine, callNode->errorLine), node->errorLine);
            bodyNodes.push_back(node);
            return bodyNodes;
        }
        bodyNodes.push_back(callNode);
        tokenNumber = callNode->tokenNumber;
        if (tokenNumber >= tokenObj.tokenCount) {
            // If tokens ended
            Node* node = new Node();
            node->parsingError = true;
            node->errorLine = tokenObj.tokenLines.back();
            bodyNodes.push_back(node);
            return bodyNodes;
        }
    }
}

LoopNode* LoopParser::parse(int& tokenNumber, SyntaxToken& tokenObj) {
    LoopNode* node = new LoopNode();
    if (!tokenObj.validToken(tokenNumber, "value", "while", 0)) {
        node->parsingError = true;
        node->errorLine = tokenObj.tokenLines[std::min(tokenNumber, tokenObj.tokenCount - 1)];
        return node;
    }
    tokenNumber++;
    ExpressionNode* exprNode = ExpressionParser::parse(tokenNumber, tokenObj);
    if (exprNode->parsingError) {
        node->parsingError = true;
        node->errorLine = exprNode->errorLine;
        return node;
    }
    node->setExpression(*exprNode);
    tokenNumber = exprNode->tokenNumber;
    if (!tokenObj.validToken(tokenNumber, "value", "loop", 0)) {
        node->parsingError = true;
        node->errorLine = tokenObj.tokenLines[std::min(tokenNumber, tokenObj.tokenCount - 1)];
        return node;
    }
    tokenNumber++;
    std::vector<Node*> bodyNodes = BodyParser::parse(tokenNumber, tokenObj);
    if (bodyNodes.empty()) {
        node->parsingError = true;
        node->errorLine = tokenObj.tokenLines[std::min(tokenNumber, tokenObj.tokenCount - 1)];
        return node;
    }
    Node* lastNode = bodyNodes.back();
    if (lastNode->parsingError) {
        node->parsingError = true;
        node->errorLine = lastNode->errorLine;
        return node;
    }
    for (Node* bodyNode : bodyNodes) {
        node->addBodyNode(bodyNode);
    }
    tokenNumber = lastNode->tokenNumber;
    if (!tokenObj.validToken(tokenNumber, "value", "end", 0)) {
        node->parsingError = true;
        node->errorLine = tokenObj.tokenLines[std::min(tokenNumber, tokenObj.tokenCount - 1)];
        return node;
    }
    tokenNumber++;
    node->tokenNumber = tokenNumber;
    return node;
}

ConditionalNode* ConditionalParser::parse(int& tokenNumber, SyntaxToken& tokenObj) {
    ConditionalNode* node = new ConditionalNode();
    if (!tokenObj.validToken(tokenNumber, "value", "if", 0)) {
        node->parsingError = true;
        node->errorLine = tokenObj.tokenLines[std::min(tokenNumber, tokenObj.tokenCount - 1)];
        return node;
    }
    tokenNumber++;
    ExpressionNode* exprNode = ExpressionParser::parse(tokenNumber, tokenObj);
    if (exprNode->parsingError) {
        node->parsingError = true;
        node->errorLine = exprNode->errorLine;
        return node;
    }
    node->setExpression(*exprNode);
    tokenNumber = exprNode->tokenNumber;
    if (!tokenObj.validToken(tokenNumber, "value", "then", 0)) {
        node->parsingError = true;
        node->errorLine = tokenObj.tokenLines[std::min(tokenNumber, tokenObj.tokenCount - 1)];
        return node;
    }
    tokenNumber++;
    std::vector<Node*> ifBodyNodes = BodyParser::parse(tokenNumber, tokenObj);
    if (ifBodyNodes.empty()) {
        node->parsingError = true;
        node->errorLine = tokenObj.tokenLines[std::min(tokenNumber, tokenObj.tokenCount - 1)];
        return node;
    }
    Node* lastNode = ifBodyNodes.back();
    if (lastNode->parsingError) {
        node->parsingError = true;
        node->errorLine = lastNode->errorLine;
        return node;
    }
    for (Node* bodyNode : ifBodyNodes) {
        node->addIfBodyNode(bodyNode);
    }
    tokenNumber = lastNode->tokenNumber;
    if (tokenObj.validToken(tokenNumber, "value", "else", 0)) {
        tokenNumber++;
        std::vector<Node*> elseBodyNodes = BodyParser::parse(tokenNumber, tokenObj);
        if (elseBodyNodes.empty()) {
            node->parsingError = true;
            node->errorLine = tokenObj.tokenLines[std::min(tokenNumber, tokenObj.tokenCount - 1)];
            return node;
        }
        Node* lastElseNode = elseBodyNodes.back();
        if (lastElseNode->parsingError) {
            node->parsingError = true;
            node->errorLine = lastElseNode->errorLine;
            return node;
        }
        for (Node* elseNode : elseBodyNodes) {
            node->addElseBodyNode(elseNode);
        }
        tokenNumber = lastElseNode->tokenNumber;
    }
    if (!tokenObj.validToken(tokenNumber, "value", "end", 0)) {
        node->parsingError = true;
        node->errorLine = tokenObj.tokenLines[std::min(tokenNumber, tokenObj.tokenCount - 1)];
        return node;
    }
    tokenNumber++;
    node->tokenNumber = tokenNumber;
    return node;
}

ConstructorNode* ConstructorParser::parse(int& tokenNumber, SyntaxToken& tokenObj) {
    ConstructorNode* node = new ConstructorNode();
    if (!tokenObj.validToken(tokenNumber, "value", "this", 0)) {
        node->parsingError = true;
        node->errorLine = tokenObj.tokenLines[std::min(tokenNumber, tokenObj.tokenCount - 1)];
        return node;
    }
    tokenNumber++;
    if (tokenObj.validToken(tokenNumber, "value", "(", 0)) {
        tokenNumber++;
        while (true) {
            if (tokenObj.validToken(tokenNumber, "value", ")", 0)) {
                tokenNumber++;
                break;
            }
            if (tokenObj.validToken(tokenNumber, "type", "name", 0) &&
                tokenObj.validToken(tokenNumber + 1, "value", ":", 0) &&
                tokenObj.validToken(tokenNumber + 2, "type", "name", 0) &&
                tokenObj.validToken(tokenNumber + 3, "value", "[", 0) &&
                tokenObj.validToken(tokenNumber + 4, "type", "name", 0) &&
                tokenObj.validToken(tokenNumber + 5, "value", "]", 0)
            ) {
                std::string paramName = tokenObj.tokenValues[tokenNumber + 2] + "[" + tokenObj.tokenValues[tokenNumber + 4] + "]";
                node->addParameter(tokenObj.tokenValues[tokenNumber], paramName);
                tokenNumber += 6;
                if (tokenObj.validToken(tokenNumber, "value", ",", 0)) {
                    tokenNumber++;
                    continue;
                }
                continue;
            }
            if (!(tokenObj.validToken(tokenNumber, "type", "name", 0) &&
                tokenObj.validToken(tokenNumber + 1, "value", ":", 0) &&
                tokenObj.validToken(tokenNumber + 2, "type", "name", 0))) {
                node->parsingError = true;
                node->errorLine = tokenObj.tokenLines[std::min(tokenNumber, tokenObj.tokenCount - 1)];
                return node;
            }
            node->addParameter(tokenObj.tokenValues[tokenNumber], tokenObj.tokenValues[tokenNumber + 2]);
            tokenNumber += 3;
            if (tokenObj.validToken(tokenNumber, "value", ",", 0)) {
                tokenNumber++;
                continue;
            }
            if (tokenObj.validToken(tokenNumber, "value", ")", 0)) {
                tokenNumber++;
                break;
            }
            node->parsingError = true;
            node->errorLine = tokenObj.tokenLines[std::min(tokenNumber, tokenObj.tokenCount - 1)];
            return node;
        }
    }
    if (!tokenObj.validToken(tokenNumber, "value", "is", 0)) {
        node->parsingError = true;
        node->errorLine = tokenObj.tokenLines[std::min(tokenNumber, tokenObj.tokenCount - 1)];
        return node;
    }
    tokenNumber++;
    std::vector<Node*> bodyNodes = BodyParser::parse(tokenNumber, tokenObj);
    if (bodyNodes.empty()) {
        node->parsingError = true;
        node->errorLine = tokenObj.tokenLines[std::min(tokenNumber, tokenObj.tokenCount - 1)];
        return node;
    }
    Node* lastNode = bodyNodes.back();
    if (lastNode->parsingError) {
        node->parsingError = true;
        node->errorLine = lastNode->errorLine;
        return node;
    }
    for (Node* bodyNode : bodyNodes) {
        node->addBodyNode(bodyNode);
    }
    tokenNumber = lastNode->tokenNumber;
    if (!tokenObj.validToken(tokenNumber, "value", "end", 0)) {
        node->parsingError = true;
        node->errorLine = tokenObj.tokenLines[std::min(tokenNumber, tokenObj.tokenCount - 1)];
        return node;
    }
    tokenNumber++;
    node->tokenNumber = tokenNumber;
    return node;
}

MethodNode* MethodParser::parse(int& tokenNumber, SyntaxToken& tokenObj) {
    MethodNode* node = new MethodNode();
    if (!(tokenObj.validToken(tokenNumber, "value", "method", 0) && tokenObj.validToken(tokenNumber + 1, "type", "name", 0))) {
        node->parsingError = true;
        node->errorLine = tokenObj.tokenLines[std::min(tokenNumber, tokenObj.tokenCount - 1)];
        return node;
    }
    node->setName(tokenObj.tokenValues[tokenNumber + 1]);
    tokenNumber += 2;
    if (tokenObj.validToken(tokenNumber, "value", "(", 0)) {
        tokenNumber++;
        while (true) {
            if (tokenObj.validToken(tokenNumber, "value", ")", 0)) {
                tokenNumber++;
                break;
            }
            if (tokenObj.validToken(tokenNumber, "type", "name", 0) &&
                tokenObj.validToken(tokenNumber + 1, "value", ":", 0) &&
                tokenObj.validToken(tokenNumber + 2, "type", "name", 0) &&
                tokenObj.validToken(tokenNumber + 3, "value", "[", 0) &&
                tokenObj.validToken(tokenNumber + 4, "type", "name", 0) &&
                tokenObj.validToken(tokenNumber + 5, "value", "]", 0)
            ) {
                std::string paramName = tokenObj.tokenValues[tokenNumber + 2] + "[" + tokenObj.tokenValues[tokenNumber + 4] + "]";
                node->addParameter(tokenObj.tokenValues[tokenNumber], paramName);
                tokenNumber += 6;
                if (tokenObj.validToken(tokenNumber, "value", ",", 0)) {
                    tokenNumber++;
                    continue;
                }
                continue;
            }
            if (!(tokenObj.validToken(tokenNumber, "type", "name", 0) &&
                tokenObj.validToken(tokenNumber + 1, "value", ":", 0) &&
                tokenObj.validToken(tokenNumber + 2, "type", "name", 0))) {
                node->parsingError = true;
                node->errorLine = tokenObj.tokenLines[std::min(tokenNumber, tokenObj.tokenCount - 1)];
                return node;
            }
            node->addParameter(tokenObj.tokenValues[tokenNumber], tokenObj.tokenValues[tokenNumber + 2]);
            tokenNumber += 3;
            if (tokenObj.validToken(tokenNumber, "value", ",", 0)) {
                tokenNumber++;
                continue;
            }
            if (tokenObj.validToken(tokenNumber, "value", ")", 0)) {
                tokenNumber++;
                break;
            }
            node->parsingError = true;
            node->errorLine = tokenObj.tokenLines[std::min(tokenNumber, tokenObj.tokenCount - 1)];
            return node;
        }
    }
    if (tokenObj.validToken(tokenNumber, "value", ":", 0) && tokenObj.validToken(tokenNumber + 1, "type", "name", 0)) {
        node->setReturnType(tokenObj.tokenValues[tokenNumber + 1]);
        tokenNumber += 2;
    }
    if (!tokenObj.validToken(tokenNumber, "value", "is", 0)) {
        node->parsingError = true;
        node->errorLine = tokenObj.tokenLines[std::min(tokenNumber, tokenObj.tokenCount - 1)];
        return node;
    }
    tokenNumber++;
    std::vector<Node*> bodyNodes = BodyParser::parse(tokenNumber, tokenObj);
    if (bodyNodes.empty()) {
        node->parsingError = true;
        node->errorLine = tokenObj.tokenLines[std::min(tokenNumber, tokenObj.tokenCount - 1)];
        return node;
    }
    Node* lastNode = bodyNodes.back();
    if (lastNode->parsingError) {
        node->parsingError = true;
        node->errorLine = lastNode->errorLine;
        return node;
    }
    for (Node* bodyNode : bodyNodes) {
        node->addBodyNode(bodyNode);
    }
    tokenNumber = lastNode->tokenNumber;
    if (!tokenObj.validToken(tokenNumber, "value", "end", 0)) {
        node->parsingError = true;
        node->errorLine = tokenObj.tokenLines[std::min(tokenNumber, tokenObj.tokenCount - 1)];
        return node;
    }
    tokenNumber++;
    node->tokenNumber = tokenNumber;
    return node;
}
