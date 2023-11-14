class MethodNode : public Node {
public:
    std::string name;
    std::string returnType;
    std::vector<std::string> parameterNames;
    std::vector<std::string> parameterTypes;
    std::vector<Node*> bodyNodes;
    bool isDestructor;

    MethodNode() : isDestructor(false) {
        initializePrintFunction([&](std::string tab) {
            std::cout << tab << "Method name: " << name << std::endl;
            std::cout << tab << "Return type: " << (returnType.empty() ? "NONE" : returnType) << std::endl;
            int paramCount = (int) parameterNames.size();
            std::cout << tab << "Parameters" << std::endl;
            for (int i = 0; i < paramCount; i++) {
                std::cout << (tab + "\t") << "param_name: " << parameterNames[i] << " ";
                std::cout << "| param_type: " << parameterTypes[i] << std::endl;
            }
            for (auto node : bodyNodes) {
                node->print(tab + "\t");
            }
        });
        nodeType = "MethodNode";
        parameterNames = std::vector<std::string>();
        parameterTypes = std::vector<std::string>();
        bodyNodes = std::vector<Node*>();
    }

    MethodNode(bool isDestructor) : isDestructor(isDestructor) {
        initializePrintFunction([&](std::string tab) {
            std::cout << tab << "Method name: " << name << std::endl;
            std::cout << tab << "Return type: " << (returnType.empty() ? "NONE" : returnType) << std::endl;
            int paramCount = (int) parameterNames.size();
            std::cout << tab << "Parameters" << std::endl;
            for (int i = 0; i < paramCount; i++) {
                std::cout << (tab + "\t") << "param_name: " << parameterNames[i] << " ";
                std::cout << "| param_type: " << parameterTypes[i] << std::endl;
            }
            for (auto node : bodyNodes) {
                node->print(tab + "\t");
            }
        });
        nodeType = "MethodNode";
        parameterNames = std::vector<std::string>();
        parameterTypes = std::vector<std::string>();
        bodyNodes = std::vector<Node*>();
        name = "Destructor";
    }

    MethodNode(const std::string& name, const std::string& returnType) :
        name(name), returnType(returnType) {
        initializePrintFunction([&](std::string tab) {
            std::cout << tab << "Method name: " << name << std::endl;
            std::cout << tab << "Return type: " << (returnType.empty() ? "NONE" : returnType) << std::endl;
            int paramCount = (int) parameterNames.size();
            std::cout << tab << "Parameters" << std::endl;
            for (int i = 0; i < paramCount; i++) {
                std::cout << (tab + "\t") << "param_name: " << parameterNames[i] << " ";
                std::cout << "| param_type: " << parameterTypes[i] << std::endl;
            }
            for (auto node : bodyNodes) {
                node->print(tab + "\t");
            }
        });
        nodeType = "MethodNode";
        parameterNames = std::vector<std::string>();
        parameterTypes = std::vector<std::string>();
        bodyNodes = std::vector<Node*>();
    }

    MethodNode(const std::string& name, const std::vector<std::string>& paramNames, const std::vector<std::string>& paramTypes) :
        name(name),
        parameterNames(paramNames),
        parameterTypes(paramTypes) {
        initializePrintFunction([&](std::string tab) {
            std::cout << tab << "Method name: " << name << std::endl;
            std::cout << tab << "Return type: " << (returnType.empty() ? "NONE" : returnType) << std::endl;
            int paramCount = (int) parameterNames.size();
            std::cout << tab << "Parameters" << std::endl;
            for (int i = 0; i < paramCount; i++) {
                std::cout << (tab + "\t") << "param_name: " << parameterNames[i] << " ";
                std::cout << "| param_type: " << parameterTypes[i] << std::endl;
            }
            for (auto node : bodyNodes) {
                node->print(tab + "\t");
            }
        });
        nodeType = "MethodNode";
        bodyNodes = std::vector<Node*>();
    }

    MethodNode(const std::string& name, const std::string& returnType, const std::vector<std::string>& paramNames, const std::vector<std::string>& paramTypes) :
        name(name),
        returnType(returnType),
        parameterNames(paramNames),
        parameterTypes(paramTypes) {
        initializePrintFunction([&](std::string tab) {
            std::cout << tab << "Method name: " << name << std::endl;
            std::cout << tab << "Return type: " << (returnType.empty() ? "NONE" : returnType) << std::endl;
            int paramCount = (int) parameterNames.size();
            std::cout << tab << "Parameters" << std::endl;
            for (int i = 0; i < paramCount; i++) {
                std::cout << (tab + "\t") << "param_name: " << parameterNames[i] << " ";
                std::cout << "| param_type: " << parameterTypes[i] << std::endl;
            }
            for (auto node : bodyNodes) {
                node->print(tab + "\t");
            }
        });
        nodeType = "MethodNode";
        bodyNodes = std::vector<Node*>();
    }

    void setName(const std::string& name) {
        this->name = name;
    }

    void setReturnType(const std::string& returnType) {
        this->returnType = returnType;
    }

    void addParameter(const std::string& paramName, const std::string& paramType) {
        parameterNames.push_back(paramName);
        parameterTypes.push_back(paramType);
    }

    void addBodyNode(Node* node) {
        bodyNodes.push_back(node);
    }
};
