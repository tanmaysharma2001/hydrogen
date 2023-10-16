class RootNode : public Node {
public:
    std::vector<Node*> classNodes;

    RootNode() {
        classNodes = std::vector<Node*>();
    }

    void addClassDeclaration(ClassNode* node) {
        classNodes.push_back(node);
    }
};