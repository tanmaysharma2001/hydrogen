class ConstructorNode : public Node {
public:
    std::vector<std::string> parameterNames;
    std::vector<std::string> parameterTypes;
    std::vector<Node*> bodyNodes;

    ConstructorNode() {
        initializePrintFunction([&](std::string tab) {
            std::cout << tab << "Constructor" << std::endl;
            std::cout << tab << "Parameters" << std::endl;
            int paramCount = (int) parameterNames.size();
            for (int i = 0; i < paramCount; i++) {
                std::cout << (tab + "\t") << "param_name: " << parameterNames[i] << " ";
                std::cout << "| param_type: " << parameterTypes[i] << std::endl;
            }
            for (auto node : bodyNodes) {
                node->print(tab + "\t");
            }
        });
        nodeType = "ConstructorNode";
        parameterNames = std::vector<std::string>();
        parameterTypes = std::vector<std::string>();
        bodyNodes = std::vector<Node*>();
    }

    ConstructorNode(const std::vector<std::string>& paramNames, const std::vector<std::string>& paramTypes) :
        parameterNames(paramNames),
        parameterTypes(paramTypes) {
        bodyNodes = std::vector<Node*>();
    }

    void addParameter(const std::string& paramName, const std::string& paramType) {
        parameterNames.push_back(paramName);
        parameterTypes.push_back(paramType);
    }

    void addBodyNode(Node* node) {
        bodyNodes.push_back(node);
    }
};