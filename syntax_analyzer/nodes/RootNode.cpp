class RootNode : public Node {
public:
    std::vector<ClassNode*> classNodes;

    RootNode() {
        initializePrintFunction([this](std::string tab) {
            for (auto node : classNodes) {
                node->print(tab);
            }
        });
        classNodes = std::vector<ClassNode*>();
        nodeType = "RootNode";
    }

    void addClassDeclaration(ClassNode* node) {
        classNodes.push_back(new ClassNode(*node));
    }
};