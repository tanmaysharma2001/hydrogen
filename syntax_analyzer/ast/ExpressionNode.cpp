class ExpressionNode : public Node {
public:
    std::string expressionType; 
    std::string integerValue; 
    std::string booleanValue; 
    std::string floatValue; 
    CallNode call;

    ExpressionNode() : integerValue(""), booleanValue(""), floatValue("") {
        call = CallNode();
    }

    void setExpressionType(const std::string& expressionType) {
        this->expressionType = expressionType;
    }

    void setCall(const CallNode& callNode) {
        this->call = callNode;
    }
};