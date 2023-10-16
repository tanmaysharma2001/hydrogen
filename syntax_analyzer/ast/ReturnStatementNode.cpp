class ReturnStatementNode : public Node {
public:
    bool isVoid;
    ExpressionNode expression;

    void setExpression(const ExpressionNode& expr) {
        expression = expr;
    }
};