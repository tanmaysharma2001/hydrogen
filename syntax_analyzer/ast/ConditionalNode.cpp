class ConditionalNode : public Node {
public:
    ExpressionNode expression;
    std::vector<Node*> ifBodyNodes;
    std::vector<Node*> elseBodyNodes;

    ConditionalNode() {
        ifBodyNodes = std::vector<Node*>();
        elseBodyNodes = std::vector<Node*>();
    }

    void setExpression(const ExpressionNode& expr) {
        expression = expr;
    }

    void addIfBodyNode(Node* node) {
        ifBodyNodes.push_back(node);
    }

    void addElseBodyNode(Node* node) {
        elseBodyNodes.push_back(node);
    }
};