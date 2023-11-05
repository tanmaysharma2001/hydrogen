class LoopNode : public Node {
public:
    ExpressionNode expression;
    std::vector<Node*> bodyNodes;

    LoopNode() {
        initializePrintFunction([&](std::string tab) {
            std::cout << tab << "Loop" << std::endl;
            expression.print(tab + "\t");
            for (auto node : bodyNodes) {
                node->print(tab + "\t");
            }
        });
        nodeType = "LoopNode";
        bodyNodes = std::vector<Node*>();
    }

    void setExpression(const ExpressionNode& expr) {
        expression = expr;
    }

    void addBodyNode(Node* node) {
        bodyNodes.push_back(node);
    }
};