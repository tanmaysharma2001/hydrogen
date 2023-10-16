class VariableNode : public Node {
public:
    std::string name;
    ExpressionNode expression;

    VariableNode(const std::string& name, const std::string& type) :
        name(name), expression() {
        expression.setExpressionType("Call");    
    }

    VariableNode() : expression(ExpressionNode()) {}

    void setName(const std::string& name) {
        this->name = name;
    }

    void setExpression(const ExpressionNode& expr) {
        expression = expr;
    }
};