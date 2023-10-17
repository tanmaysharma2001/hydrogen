class ClassParser {
public:
    static ClassNode* parse(int& tokenNumber, SyntaxToken& tokenObj);
};

ClassNode* ClassParser::parse(int& tokenNumber, SyntaxToken& tokenObj) {
    ClassNode* node = new ClassNode(); 
    if (!(tokenObj.validToken(tokenNumber, "value", "class", 0) && tokenObj.validToken(tokenNumber + 1, "type", "name", 0))) {
        node->parsingError = true;
        node->errorLine = tokenObj.tokenLines[std::min(tokenNumber, tokenObj.tokenCount - 1)];
        return node;
    }
    node->setName(tokenObj.tokenValues[tokenNumber + 1]);
    tokenNumber += 2;
    if (tokenObj.validToken(tokenNumber, "value", "extends", 0) && tokenObj.validToken(tokenNumber + 1, "type", "name", 0)) {
        node->setSuperClass(tokenObj.tokenValues[tokenNumber + 1]);
        tokenNumber += 2;
    }
    if (!tokenObj.validToken(tokenNumber, "value", "is", 0)) {
        node->parsingError = true;
        node->errorLine = tokenObj.tokenLines[std::min(tokenNumber, tokenObj.tokenCount - 1)];
        return node;
    }
    tokenNumber++;
    while (true) {
        if (tokenObj.validToken(tokenNumber, "value", "end", 0)) {
            tokenNumber++;
            node->tokenNumber = tokenNumber;
            return node;
        }
        VariableNode* variableNode = VariableParser::parse(tokenNumber, tokenObj);
        if (!variableNode->parsingError) {
            tokenNumber = variableNode->tokenNumber;
            node->addVariableDeclaration(variableNode);
            continue;
        }
        MethodNode* methodNode = MethodParser::parse(tokenNumber, tokenObj);
        if (!methodNode->parsingError) {
            tokenNumber = methodNode->tokenNumber;
            node->addMethodDeclaration(methodNode);
            continue;
        }
        ConstructorNode* constructorNode = ConstructorParser::parse(tokenNumber, tokenObj);
        if (constructorNode->parsingError) {
            node->parsingError = true;
            node->errorLine = std::max(variableNode->errorLine, std::max(methodNode->errorLine, constructorNode->errorLine));
            return node;
        }
        tokenNumber = constructorNode->tokenNumber;
        node->addConstructorDeclaration(constructorNode);
        if (tokenNumber >= tokenObj.tokenCount) {
            node->parsingError = true;
            node->errorLine = tokenObj.tokenLines.back();
            return node;
        }
    }
}
