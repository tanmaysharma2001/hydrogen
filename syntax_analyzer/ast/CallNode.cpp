class ExpressionNode;

class CallNode : public Node {
public:
    std::string parentName; // If exists, this or ClassName structure
    std::vector<std::string> parentNames; // If exists
    std::vector<std::vector<ExpressionNode>> arguments;

    CallNode() {
        arguments = std::vector<std::vector<ExpressionNode>>();
        parentNames = std::vector<std::string>();
    }

    CallNode(const CallNode& other) {
        this->parentName = other.parentName;
        this->parentNames = other.parentNames;
        // Copy the arguments vector
        this->arguments = other.arguments;
    }

    void setCallerName(const std::string& parentName) {
        this->parentName = parentName;
    }

    void addCallee(const std::string& calleeName, const std::vector<ExpressionNode>& args) {
        parentNames.push_back(calleeName);
        arguments.push_back(args);
    }
};