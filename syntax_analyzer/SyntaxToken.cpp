struct SyntaxToken {
    std::vector<std::string> tokenValues;
    std::vector<std::string> tokenTypes;
    std::vector<int> tokenLines;
    std::vector<int> tokenColumns;
    int tokenCount;

    SyntaxToken(std::vector<std::string>& tokenValues, std::vector<std::string>& tokenTypes,
                std::vector<int>& tokenLines, std::vector<int>& tokenColumns)
        : tokenValues(tokenValues), tokenTypes(tokenTypes), tokenLines(tokenLines), tokenColumns(tokenColumns) {
            this->tokenCount = (int) tokenValues.size();
        }

    bool validToken(int tokenNumber, const std::string& attributeType,
                      const std::string& valueOrType, int lineOrColumn) {
        if (tokenNumber >= tokenValues.size()) {
            return false;
        } else {
            if (attributeType == "value") {
                return tokenValues[tokenNumber] == valueOrType;
            } else if (attributeType == "type") {
                return tokenTypes[tokenNumber] == valueOrType;
            } else if (attributeType == "line") {
                return tokenLines[tokenNumber] == lineOrColumn;
            } else if (attributeType == "column") {
                return tokenColumns[tokenNumber] == lineOrColumn;
            }
            return false;
        }
    }
};