class ReturnStatementParser {
public:
    static ReturnStatementNode parse(int& tokenNumber, SyntaxToken& tokenObj) {
        ReturnStatementNode node;
        if (!tokenObj.validToken(tokenNumber, "value", "return", 0)) {
            node.parsingError = true;
            node.errorLine = tokenObj.tokenLines[std::min(tokenNumber, tokenObj.tokenCount - 1)];
            return node;
        }
        tokenNumber++;
        ExpressionNode exprNode = ExpressionParser::parse(tokenNumber, tokenObj);
        if (exprNode.parsingError) {
            node.isVoid = true;
        } else {
            node.setExpression(exprNode);
            tokenNumber = exprNode.tokenNumber;
        }
        node.tokenNumber = tokenNumber;
        return node;
    }
};