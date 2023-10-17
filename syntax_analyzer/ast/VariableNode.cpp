class VariableNode : public Node {
public:
    std::string name;
    ExpressionNode expression;

    VariableNode(const std::string& name, const std::string& type) :
        name(name), expression() {
        expression.setExpressionType("Call");    
        initializePrintFunction([&](std::string tab) {
            std::cout << tab << "Variable name: " << name << std::endl;
            expression.print(tab);
        });
    }

    VariableNode() : expression(ExpressionNode()) {    
        initializePrintFunction([&](std::string tab) {
            std::cout << tab << "Variable name: " << name << std::endl;
            expression.print(tab);
        });
    }

    void setName(const std::string& name) {
        this->name = name;
    }

    void setExpression(const ExpressionNode& expr) {
        expression = expr;
    }
};