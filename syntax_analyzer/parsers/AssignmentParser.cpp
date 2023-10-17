class AssignmentParser {
public:
    static AssignmentNode* parse(int& tokenNumber, SyntaxToken &tokenObj) {
        AssignmentNode* node = new AssignmentNode();
        if (!((tokenObj.validToken(tokenNumber, "type", "name", 0) && tokenObj.validToken(tokenNumber + 1, "value", ":=", 0)) ||
            (tokenObj.validToken(tokenNumber, "value", "this", 0) && tokenObj.validToken(tokenNumber + 1, "value", ".", 0) &&
             tokenObj.validToken(tokenNumber + 2, "type", "name", 0) && tokenObj.validToken(tokenNumber + 3, "value", ":=", 0))
        )) {
            node->parsingError = true;
            node->errorLine = tokenObj.tokenLines[std::min(tokenNumber, (int) tokenObj.tokenCount - 1)];
            return node;
        }
        std::string prefix = "";
        if (tokenObj.validToken(tokenNumber, "value", "this", 0) && tokenObj.validToken(tokenNumber + 1, "value", ".", 0)) {
            prefix += tokenObj.tokenValues[tokenNumber];
            prefix += tokenObj.tokenValues[tokenNumber + 1];
            tokenNumber += 2;
        }
        node->setName(prefix + tokenObj.tokenValues[tokenNumber]);
        tokenNumber += 2;
        ExpressionNode* exprNode = ExpressionParser::parse(tokenNumber, tokenObj);
        if (exprNode->parsingError) {
            node->parsingError = true;
            node->errorLine = exprNode->errorLine;
            return node;
        }
        node->setExpression(exprNode);
        node->tokenNumber = exprNode->tokenNumber;
        return node;
    }
};
