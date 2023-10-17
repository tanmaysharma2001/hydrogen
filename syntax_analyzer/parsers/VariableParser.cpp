class VariableParser {
public:
    static VariableNode* parse(int& tokenNumber, SyntaxToken& tokenObj);
};

VariableNode* VariableParser::parse(int& tokenNumber, SyntaxToken& tokenObj) {
    VariableNode* node = new VariableNode();
    
    if (tokenObj.validToken(tokenNumber, "value", "var", 0) &&
        tokenObj.validToken(tokenNumber + 1, "type", "name", 0) &&
        tokenObj.validToken(tokenNumber + 2, "value", ":", 0) &&
        tokenObj.validToken(tokenNumber + 3, "type", "name", 0) &&
        tokenObj.validToken(tokenNumber + 4, "value", "[", 0) &&
        tokenObj.validToken(tokenNumber + 5, "type", "name", 0) &&
        tokenObj.validToken(tokenNumber + 6, "value", "]", 0)
    ) {
        std::string paramName = tokenObj.tokenValues[tokenNumber + 3] + "[" + tokenObj.tokenValues[tokenNumber + 5] + "]";
        node->setName(paramName);
        tokenNumber += 7;
    } else {
        if (!(tokenObj.validToken(tokenNumber, "value", "var", 0) &&
                tokenObj.validToken(tokenNumber + 1, "type", "name", 0) &&
                tokenObj.validToken(tokenNumber + 2, "value", ":", 0))) {
            node->parsingError = true;
            node->errorLine = tokenObj.tokenLines[std::min(tokenNumber, tokenObj.tokenCount - 1)];
            return node;
        }
        node->setName(tokenObj.tokenValues[tokenNumber + 1]);
        tokenNumber += 3;
    }
    ExpressionNode* exprNode = ExpressionParser::parse(tokenNumber, tokenObj);
    if (exprNode->parsingError) {
        node->parsingError = true;
        node->errorLine = exprNode->errorLine;
        return node;
    }
    node->setExpression(*exprNode);
    node->tokenNumber = exprNode->tokenNumber;
    return node;
}
