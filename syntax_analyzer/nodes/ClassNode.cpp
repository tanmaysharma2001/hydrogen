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
            for (auto node : constructors) {
                node->print(tab + "\t");
            }
            for (auto node : methods) {
                node->print(tab + "\t");
            }
        });
        nodeType = "ClassNode";
        variables = std::vector<VariableNode*>();
        methods = std::vector<MethodNode*>();
        constructors = std::vector<ConstructorNode*>();
    }

    ClassNode(std::string name, std::string superClass,
              std::vector<MethodNode*> methodDeclarationNodes,
              std::vector<ConstructorNode*> constructorDeclarationNodes,
              std::vector<VariableNode*> variables) :
        name(name),
        superClass(superClass),
        variables(variables),
        methods(methodDeclarationNodes),
        constructors(constructorDeclarationNodes) {}

    ClassNode(std::string name, std::string superClass, std::string genericClass,
              std::vector<MethodNode*> methodDeclarationNodes,
              std::vector<ConstructorNode*> constructorDeclarationNodes,
              std::vector<VariableNode*> variables) :
        name(name),
        superClass(superClass),
        genericClass(genericClass),
        variables(variables),
        methods(methodDeclarationNodes),
        constructors(constructorDeclarationNodes) {}

    ClassNode(std::string name, std::string superClass, std::string genericClass) :
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