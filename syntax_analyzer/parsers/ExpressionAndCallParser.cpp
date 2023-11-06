class CallParser {
public:
    static CallNode* parse(int& tokenNumber, SyntaxToken& tokenObj);
};

class ExpressionParser {
public:
    static ExpressionNode* parse(int& tokenNumber, SyntaxToken& tokenObj);
};

CallNode* CallParser::parse(int& tokenNumber, SyntaxToken& tokenObj) {
    CallNode* node = new CallNode();
    if ((tokenObj.validToken(tokenNumber, "type", "name", 0) || tokenObj.validToken(tokenNumber, "value", "this", 0)) &&
        tokenObj.validToken(tokenNumber + 1, "value", ".", 0)) {
        node->setCallerName(tokenObj.tokenValues[tokenNumber]);
        tokenNumber += 2;
    }
    while (true) {
        std::string currentCallee = "";
        std::vector<ExpressionNode> arguments;
        if (!tokenObj.validToken(tokenNumber, "type", "name", 0)) {
            node->parsingError = true;
            node->errorLine = tokenObj.tokenLines[std::min(static_cast<int>(tokenNumber), tokenObj.tokenCount - 1)];
            return node;
        }
        currentCallee = tokenObj.tokenValues[tokenNumber];
        ++tokenNumber;
        if (tokenObj.validToken(tokenNumber, "value", "(", 0) || tokenObj.validToken(tokenNumber, "value", "[", 0)) {
            ++tokenNumber;
            while (true) {
                ExpressionNode* enode = ExpressionParser::parse(tokenNumber, tokenObj);
                if (enode->parsingError) {
                    node->parsingError = true;
                    node->errorLine = enode->errorLine;
                    return node;
                }
                tokenNumber = enode->tokenNumber;
                arguments.push_back(*enode);
                if (tokenObj.validToken(tokenNumber, "value", ",", 0)) {
                    ++tokenNumber;
                    continue;
                }
                if (tokenObj.validToken(tokenNumber, "value", ")", 0)) {
                    ++tokenNumber;
                    break;
                }
                if (tokenObj.validToken(tokenNumber, "value", "]", 0)) {
                    ++tokenNumber;
                    break;
                }
                node->parsingError = true;
                node->errorLine = tokenObj.tokenLines[std::min(static_cast<int>(tokenNumber), tokenObj.tokenCount - 1)];
                return node;
            }
        }
        node->addCallee(currentCallee, arguments);
        if (tokenObj.validToken(tokenNumber, "value", ".", 0)) {
            ++tokenNumber;
            continue;
        }
        break;
    }
    node->tokenNumber = tokenNumber;
    return node;
}

ExpressionNode* ExpressionParser::parse(int& tokenNumber, SyntaxToken& tokenObj) {
    ExpressionNode* node = new ExpressionNode();
    if (tokenObj.validToken(tokenNumber, "type", "int", 0)) {
        node->expressionType = "literal";
        node->integerValue = tokenObj.tokenValues[tokenNumber];
        ++tokenNumber;
        node->tokenNumber = tokenNumber;
        return node;
    }
    if (tokenObj.validToken(tokenNumber, "type", "bool", 0)) {
        node->expressionType = "literal";
        node->booleanValue = tokenObj.tokenValues[tokenNumber];
        ++tokenNumber;
        node->tokenNumber = tokenNumber;
        return node;
    }
    if (tokenObj.validToken(tokenNumber, "type", "float", 0)) {
        node->expressionType = "literal";
        node->floatValue = tokenObj.tokenValues[tokenNumber];
        ++tokenNumber;
        node->tokenNumber = tokenNumber;
        return node;
    }
    CallNode* cNode = CallParser::parse(tokenNumber, tokenObj);
    if (cNode->parsingError) {
        node->parsingError = true;
        node->errorLine = cNode->errorLine;
        return node;
    }
    node->expressionType = "call";
    node->tokenNumber = cNode->tokenNumber;
    node->setCall(cNode);
    return node;
}
