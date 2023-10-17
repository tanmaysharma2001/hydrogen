class ClassNode : public Node {
public:
    std::string name;
    std::string superClass;
    std::string genericClass;
    std::vector<VariableNode*> variables;
    std::vector<MethodNode*> methods;
    std::vector<ConstructorNode*> constructors;

    ClassNode() {
        initializePrintFunction([this](std::string tab) {
            std::cout << tab << "Class name: " << name << std::endl;
            std::cout << tab << "Super class: " << (superClass.empty() ? "NONE" : superClass) << std::endl;
            std::cout << tab << "Generic class: " << (genericClass.empty() ? "NONE" : genericClass) << std::endl;
            for (auto node : variables) {
                node->print(tab + "\t");
            }
            for (auto node : methods) {
                node->print(tab + "\t");
            }
        });
        variables = std::vector<VariableNode*>();
        methods = std::vector<MethodNode*>();
        constructors = std::vector<ConstructorNode*>();
    }

    ClassNode(const std::string& name, const std::string& superClass, const std::vector<MethodNode*>& methodDeclarationNodes, const std::vector<ConstructorNode*>& constructorDeclarationNodes, const std::vector<VariableNode*>& variables) :
        name(name),
        superClass(superClass),
        variables(variables),
        methods(methodDeclarationNodes),
        constructors(constructorDeclarationNodes) {}

    ClassNode(const std::string& name, const std::string& superClass, const std::string& genericClass, const std::vector<MethodNode*>& methodDeclarationNodes, const std::vector<ConstructorNode*>& constructorDeclarationNodes, const std::vector<VariableNode*>& variables) :
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
        variables(std::vector<VariableNode * >()),
        methods(std::vector<MethodNode * >()),
        constructors(std::vector<ConstructorNode * >()) {}

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
        variables.push_back(new VariableNode(*node));
    }

    void addMethodDeclaration(MethodNode* node) {
        methods.push_back(new MethodNode(*node));
    }

    void addConstructorDeclaration(ConstructorNode* node) {
        constructors.push_back(new ConstructorNode(*node));
    }
};