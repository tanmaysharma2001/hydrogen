class MethodNode : public Node {
public:
    std::string name;
    std::string returnType;
    std::vector<std::string> parameterNames;
    std::vector<std::string> parameterTypes;
    std::vector<Node*> bodyNodes;
    bool isDestructor;

    MethodNode() : isDestructor(false) {
        parameterNames = std::vector<std::string>();
        parameterTypes = std::vector<std::string>();
        bodyNodes = std::vector<Node*>();
    }

    MethodNode(bool isDestructor) : isDestructor(isDestructor) {
        parameterNames = std::vector<std::string>();
        parameterTypes = std::vector<std::string>();
        bodyNodes = std::vector<Node*>();
        name = "Destruct";
    }

    MethodNode(const std::string& name, const std::string& returnType) :
        name(name) {
        parameterNames = std::vector<std::string>();
        parameterTypes = std::vector<std::string>();
        bodyNodes = std::vector<Node*>();
    }

    MethodNode(const std::string& name, const std::vector<std::string>& paramNames, const std::vector<std::string>& paramTypes) :
        name(name),
        parameterNames(paramNames),
        parameterTypes(paramTypes) {
        bodyNodes = std::vector<Node*>();
    }

    MethodNode(const std::string& name, const std::string& returnType, const std::vector<std::string>& paramNames, const std::vector<std::string>& paramTypes) :
        name(name),
        returnType(returnType),
        parameterNames(paramNames),
        parameterTypes(paramTypes) {
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
