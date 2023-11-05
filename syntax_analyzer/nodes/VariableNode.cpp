class VariableNode : public Node {
public:
    std::string name;
    ExpressionNode expression;

    VariableNode(const std::string& name, const std::string& type) :
        name(name), expression() {
        expression.setExpressionType("call");    
        CallNode* callNode = new CallNode();
        callNode->parentNames = std::vector<std::string>{type};
        expression.setCall(callNode);
        initializePrintFunction([&](std::string tab) {
            std::cout << tab << "Variable name: " << name << std::endl;
            expression.print(tab);
        });
        nodeType = "VariableNode";
    }

    VariableNode() : expression(ExpressionNode()) {    
        initializePrintFunction([&](std::string tab) {
            std::cout << tab << "Variable name: " << name << std::endl;
            expression.print(tab);
        });
        nodeType = "VariableNode";
    }

    void setName(const std::string& name) {
        this->name = name;
    }

    void setExpression(const ExpressionNode& expr) {
        expression = expr;
    }
};