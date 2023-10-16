class ClassNode : public Node {
public:
    std::string name;
    std::string superClass;
    std::string genericClass;
    std::vector<Node*> variables;
    std::vector<Node*> methods;
    std::vector<Node*> constructors;

    ClassNode() {
        variables = std::vector<Node*>();
        methods = std::vector<Node*>();
        constructors = std::vector<Node*>();
    }

    ClassNode(const std::string& name, const std::string& superClass, const std::vector<Node*>& methodDeclarationNodes, const std::vector<Node*>& constructorDeclarationNodes, const std::vector<Node*>& variables) :
        name(name),
        superClass(superClass),
        variables(variables),
        methods(methodDeclarationNodes),
        constructors(constructorDeclarationNodes) {}

    ClassNode(const std::string& name, const std::string& superClass, const std::string& genericClass, const std::vector<Node*>& methodDeclarationNodes, const std::vector<Node*>& constructorDeclarationNodes, const std::vector<Node*>& variables) :
        name(name),
        superClass(superClass),
        genericClass(genericClass),
        variables(variables),
        methods(methodDeclarationNodes),
        constructors(constructorDeclarationNodes) {}

    ClassNode(const std::string& name, const std::string& superClass, const std::string& genericClass) :
        name(name),
        superClass(superClass),
        genericClass(genericClass),
        variables(std::vector<Node*>()),
        methods(std::vector<Node*>()),
        constructors(std::vector<Node*>()) {}

    void setName(const std::string& name) {
        this->name = name;
    }

    void setSuperClass(const std::string& superClass) {
        this->superClass = superClass;
    }

    void setGenericClass(const std::string& genericClass) {
        this->genericClass = genericClass;
    }

    void addVariableDeclaration(VariableNode* node) {
        variables.push_back(node);
    }

    void addMethodDeclaration(MethodNode* node) {
        methods.push_back(node);
    }

    void addConstructorDeclaration(ConstructorNode* node) {
        constructors.push_back(node);
    }
};