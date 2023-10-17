class ConditionalNode : public Node {
public:
    ExpressionNode expression;
    std::vector<Node*> ifBodyNodes;
    std::vector<Node*> elseBodyNodes;

    ConditionalNode() {
        initializePrintFunction([&](std::string tab) {
            std::cout << tab << "If Statement" << std::endl;
            expression.print(tab + "\t");
            for (auto node : ifBodyNodes) {
                node->print(tab + "\t");
            }
            for (auto node : elseBodyNodes) {
                node->print(tab + "\t");
            }
        });
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