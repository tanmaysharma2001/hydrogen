class LoopNode : public Node {
public:
    ExpressionNode expression;
    std::vector<Node*> bodyNodes;

    LoopNode() {
        bodyNodes = std::vector<Node*>();
    }

    void setExpression(const ExpressionNode& expr) {
        expression = expr;
    }

    void addBodyNode(Node* node) {
        bodyNodes.push_back(node);
    }
};