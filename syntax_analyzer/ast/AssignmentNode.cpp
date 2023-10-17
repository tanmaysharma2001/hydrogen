class AssignmentNode : public Node {
public:
    std::string name;
    ExpressionNode* expression;

    AssignmentNode() {
        initializePrintFunction([&](std::string tab) {
            std::cout << tab << "Assignment" << std::endl;
            std::cout << tab << "Lvalue: " << name << std::endl;
            expression->print(tab + "\t");
        });
    }

    void setName(const std::string& name) {
        this->name = name;
    }

    void setExpression(ExpressionNode* expression) {
        this->expression = new ExpressionNode(*expression);
    }
};