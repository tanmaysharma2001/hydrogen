class AssignmentNode : public Node {
public:
    std::string name;
    ExpressionNode expression;

    void setName(const std::string& name) {
        this->name = name;
    }

    void setExpression(const ExpressionNode& expression) {
        this->expression = expression;
    }
};