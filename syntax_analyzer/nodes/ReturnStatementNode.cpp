class ReturnStatementNode : public Node {
public:
    bool isVoid;
    ExpressionNode expression;

    ReturnStatementNode() {
        initializePrintFunction([&](std::string tab) {
            std::cout << tab << "Return Statement" << std::endl;
            expression.print(tab + "\t");
        });
    }

    void setExpression(const ExpressionNode& expr) {
        expression = expr;
    }
};