class ExpressionNode; // Forward declaration

class CallNode; // Forward declaration

class CallNode : public Node {
public:
    std::string parentName; 
    std::vector<std::string> parentNames; 
    std::vector<std::vector<ExpressionNode>> arguments;

    CallNode();

    void setCallerName(const std::string& parentName);

    void addCallee(const std::string& calleeName, const std::vector<ExpressionNode>& args);
};

class ExpressionNode : public Node {
public:
    std::string expressionType; 
    std::string integerValue; 
    std::string booleanValue; 
    std::string floatValue; 
    CallNode* call = nullptr; 

    ExpressionNode();

    void setExpressionType(const std::string& expressionType);

    void setCall(CallNode* callNode);
};

CallNode::CallNode() {
    initializePrintFunction([&](std::string tab) {
        if (!parentName.empty())
            std::cout << tab << "parent: " << parentName << std::endl;
        for (const auto& parent : parentNames) {
            std::cout << tab << "parent: " << parent << std::endl;
        }
        for (auto& argument : arguments) {
            for (auto& expression : argument) {
                expression.print(tab + "\t");
            }
        }
    });
    arguments = std::vector<std::vector<ExpressionNode>>();
    parentNames = std::vector<std::string>();
}

void CallNode::setCallerName(const std::string& parentName) {
    this->parentName = parentName;
}

void CallNode::addCallee(const std::string& calleeName, const std::vector<ExpressionNode>& args) {
    parentNames.push_back(calleeName);
    arguments.push_back(args);
}

ExpressionNode::ExpressionNode() : integerValue(""), booleanValue(""), floatValue(""), call(nullptr) { 
    initializePrintFunction([&](std::string tab) {
        std::cout << tab << "integerValue: " << (integerValue.empty() ? "NONE" : integerValue) << std::endl;
        std::cout << tab << "booleanValue: " << (booleanValue.empty() ? "NONE" : booleanValue) << std::endl;
        std::cout << tab << "floatValue: " << (floatValue.empty() ? "NONE" : floatValue) << std::endl;    
        std::cout << tab << "Expression Type: " << expressionType << std::endl;
        if (call != nullptr) 
            call->print(tab + "\t");
    });
}

void ExpressionNode::setExpressionType(const std::string& expressionType) {
    this->expressionType = expressionType;
}

void ExpressionNode::setCall(CallNode* callNode) {
    call = new CallNode(*callNode);
}