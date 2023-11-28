#include <vector>
#include <string>
#include <iostream>
#include <algorithm>
#include <cassert>

#include "../syntax_analyzer/parser.cpp"

class Analyzer {
private:
    RootNode* rootNode;
    std::vector<Token> tokens;
    std::vector<bool> varUsed;
    int varIndex;
    std::vector<std::string> validReturnTypes {
        "Integer",
        "Real",
        "Boolean",
        "Array",
        "List",
        "Void"
    };
public:
    std::vector<std::vector<std::string>> getVariables(ClassNode* node) {
        std::vector<std::string> variableNames;
        std::vector<std::string> variableTypes;
        std::vector<std::string> variableIndices;

        for (auto variable : node->variables) {
            std::string name = variable->name;
            std::string type = variable->expression.call->parentNames[0];
            variableNames.push_back(name);
            variableTypes.push_back(type);
            ++varIndex;
            variableIndices.push_back(std::to_string(varIndex - 1));
            varUsed.resize(varIndex);
        }

        for (const auto& name : getSuperClasses(node)) {
            ClassNode* superClass = findClass(name);
            if (!superClass->variables.empty()) {
                for (const auto& variable : superClass->variables) {
                    variableNames.push_back(variable->name);
                    variableTypes.push_back(variable->expression.call->parentNames[0]);
                    ++varIndex;
                    variableIndices.push_back(std::to_string(varIndex - 1));
                    varUsed.resize(varIndex);
                }
            }
        }

        return {variableNames, variableTypes, variableIndices};
    }

    ClassNode* findClass(const std::string& name) {
        for (auto declaration : rootNode->classNodes)
            if (name == declaration->name)
                return declaration;

        return nullptr;
    }

    void error(const Node* node, std::string message = "") {
        std::cout << "\033[31mError\n\n";
        Token token = this->tokens[node->tokenNumber - 1];
        const auto approxErrorVar = [&]() {
            std::string ret = "\'";
            if (node->tokenNumber - 2 >= 0)
                ret += this->tokens[node->tokenNumber - 2].value;
            ret += " ";
            ret += this->tokens[node->tokenNumber - 1].value;
            ret += "\'";
            return ret;
        };
        const auto prettify = [&](std::string str) {
            for (char& ch : str)
                ch = tolower(ch);
            int len = (int) str.length();
            std::string prefix = str.substr(0, len - 4);
            std::string ans = "";
            ans += prefix;
            return ans;
        };
        std::cout << "Semantic error at " << token.line << ":" << token.column << ".\n";
        std::cout << "Invalid token at " << token.line << ":" << token.column << ".\n";
        if (!message.empty())
            std::cout << "Error: " << message << std::endl;
        std::cout << "Error at " << prettify(node->nodeType) << " near " << approxErrorVar() << ".\n";
        std::exit(0);
    }

    std::vector<std::string> getSuperClasses(ClassNode* node, std::string callee = "") {
        std::vector<std::string> result;
        ClassNode* tmp = node;
        while (tmp != nullptr && !tmp->superClass.empty()) {
            result.push_back(tmp->superClass);
            if (tmp->superClass == callee) {
                std::cout << "\033[31mSubclass " << node->name << " cannot inherit super class" << std::endl;
                std::exit(0);
            }
            tmp = findClass(tmp->superClass);
            if (tmp == nullptr) {
                error(node);
                break;
            }
        }
        return result;
    }

    std::vector<std::vector<std::string>> getMethods(ClassNode* node) {
        std::vector<std::string> methodNames;
        std::vector<std::string> methodTypes;
        for (const auto& method : node->methods) {
            methodNames.push_back(method->name);
            methodTypes.push_back(method->returnType);
        }

        for (const auto& name : getSuperClasses(node)) {
            ClassNode* superClass = findClass(name);
            if (!superClass->methods.empty()) {
                for (const auto& method : superClass->methods) {
                    methodNames.push_back(method->name);
                    methodTypes.push_back(method->returnType);
                }
            }
        }

        return {methodNames, methodTypes};
    }

    std::vector<std::string> concat(std::vector<std::string> v1,
                                    std::vector<std::string> v2) {
        std::vector<std::string> ret{v1};
        for (const auto& str : v2)
            ret.push_back(str);
        return ret;
    }

    void checkClass(ClassNode* classNode) {
        getSuperClasses(classNode, classNode->name);
        std::vector<std::vector<std::string>> variablesTypesIndices = getVariables(classNode);
        std::vector<std::vector<std::string>> methodsAndTypes = getMethods(classNode);
        std::vector<std::string> validVariables = variablesTypesIndices[0];
        std::vector<std::string> validMethods = methodsAndTypes[0];
        std::vector<std::string> variableTypes = variablesTypesIndices[1];
        std::vector<std::string> variableIndices = variablesTypesIndices[2];
        std::vector<std::string> methodTypes = methodsAndTypes[1];
        for (const auto& constructor : classNode->constructors) {
            checkBody(constructor->bodyNodes, validVariables,
                      validMethods, variableTypes, variableIndices, classNode, "-1");
        }
        for (const auto& method : classNode->methods) {
            if (std::find(begin(validReturnTypes), end(validReturnTypes), method->returnType) ==
                    end(validReturnTypes) && !findClass(method->returnType)) {
                error(method, ("Invalid return type \'" + method->returnType + "\'"));
            }
            std::vector<std::string> nVariableIndices = variableIndices;
            int methodParamSize = (int) method->parameterNames.size();
            for (int i = 0; i < methodParamSize; i++) {
                ++varIndex;
                nVariableIndices.push_back(std::to_string(varIndex - 1));
                varUsed.resize(varIndex);
            }
            checkBody(method->bodyNodes, concat(validVariables, method->parameterNames),
                      validMethods, concat(variableTypes, method->parameterTypes),
                      nVariableIndices,
                      classNode, method->returnType);
        }
        std::set<std::string> varNameSet;
        for (const auto& variable : classNode->variables) {
            if (!varNameSet.empty() && varNameSet.count(variable->name)) {
                std::string errorMessage = "duplicate variable name [" + variable->name + "] in class body";
                error(variable, errorMessage);
            }
            varNameSet.insert(variable->name);
            std::string type = variable->expression.call->parentNames[0];
            if (type != "Array") {
                if (variable->expression.expressionType == "call") {
                    CallNode* callNode = variable->expression.call;
                    if (!checkCall(callNode, validVariables,
                                   validMethods, variableTypes,
                                   variableIndices,
                                   classNode, "-1")) {
                        error(variable, ("invalid expression assigned to variable \'" + variable->name + "\'"));
                    }
                }
            }
        }
    }

    bool checkExpression(ExpressionNode* node,
                         const std::vector<std::string>& validVariables,
                         const std::vector<std::string>& validMethods,
                         const std::vector<std::string>& variableTypes,
                         const std::vector<std::string>& variableIndices,
                         ClassNode* currentClass,
                         const std::string& targetReturnType) {
        if (node->expressionType == "call") {
            return checkCall(node->call, validVariables, validMethods, variableTypes,
                             variableIndices, currentClass, targetReturnType);
        } else {
            if ((node->booleanValue.empty() && targetReturnType == "Boolean") ||
                    (node->floatValue.empty() && targetReturnType == "Real") ||
                    (node->integerValue.empty() && targetReturnType == "Integer")) {
                error(node);
                return false;
            }
        }

        return true;
    }

    MethodNode* getMethod(ClassNode* classNode, const std::string& methodName) {
        for (auto method : classNode->methods) {
            if (method->name == methodName) {
                return method;
            }
        }

        for (auto& name : getSuperClasses(classNode)) {
            ClassNode* superClass = findClass(name);
            if (!superClass->methods.empty()) {
                for (const auto& method : superClass->methods) {
                    if (method->name == methodName) {
                        return method;
                    }
                }
            }
        }

        return nullptr;
    }

    ClassNode* getReturnType(ExpressionNode* node,
                             std::vector<std::string> validVariables,
                             std::vector<std::string> validMethods,
                             std::vector<std::string> variableTypes,
                             std::vector<std::string> variableIndices,
                             ClassNode* currentClass) {
        if (node->expressionType == "call") {
            ClassNode* returnTypeClass = nullptr;
            CallNode* callNode = node->call;
            int argCount = 0;
            if (callNode->parentName == "this") {
                returnTypeClass = currentClass;
            } else {
                if (!callNode->parentName.empty()) {
                    auto it = std::find(validVariables.begin(), validVariables.end(), callNode->parentName);
                    if (it != validVariables.end()) {
                        returnTypeClass = findClass(variableTypes[std::distance(validVariables.begin(), it)]);
                    } else {
                        error(callNode);
                    }
                }
            }

            if (returnTypeClass == nullptr && callNode->parentNames.empty()) {
                error(callNode);
            }

            for (const std::string& calleeName : callNode->parentNames) {
                MethodNode* method = nullptr;
                if (returnTypeClass == nullptr) {
                    if (calleeName != callNode->parentNames.front()) {
                        error(callNode);
                    }
                    if (std::find(validMethods.begin(), validMethods.end(), calleeName) != validMethods.end()) {
                        method = getMethod(currentClass, calleeName);
                    } else if (std::find(validVariables.begin(), validVariables.end(), calleeName) != validVariables.end()) {
                        method = nullptr;
                        returnTypeClass = findClass(variableTypes[std::distance(validVariables.begin(),
                                                    std::find(validVariables.begin(), validVariables.end(), calleeName))]);
                    } else if (findClass(calleeName) != nullptr) {
                        method = nullptr;
                        ClassNode* constructorClass = findClass(calleeName);
                        bool validArgs = false;
                        std::string argumentSignature;
                        for (auto argument : callNode->arguments[argCount]) {
                            ExpressionNode expNode = (ExpressionNode) argument;
                            argumentSignature += getReturnType(&expNode,
                                                               validVariables,
                                                               validMethods,
                                                               variableTypes,
                                                               variableIndices,
                                                               currentClass)->name;
                        }
                        for (auto constructor : constructorClass->constructors) {
                            if (constructor->parameterTypes.size() != callNode->arguments[argCount].size()) {
                                std::string parameterSignature;
                                for (const std::string& parameterType : constructor->parameterTypes) {
                                    parameterSignature += parameterType;
                                }
                                if (parameterSignature == argumentSignature) {
                                    validArgs = true;
                                    break;
                                }
                            }
                        }

                        if (!validArgs) {
                            error(callNode, ("Invalid arguments to " + constructorClass->name));
                        }
                        returnTypeClass = constructorClass;
                    } else {
                        error(callNode);
                    }
                } else {
                    auto tmpMethods = getMethods(returnTypeClass)[0];
                    auto tmpVariables = getVariables(returnTypeClass)[0];
                    if (std::find(begin(tmpMethods), end(tmpMethods), calleeName) != end(tmpMethods)) {
                        method = getMethod(returnTypeClass, calleeName);
                    } else if (std::find(begin(tmpVariables), end(tmpVariables), calleeName) != end(tmpVariables)) {
                        auto variables = getVariables(returnTypeClass);
                        int idx = std::find(variables[0].begin(), variables[0].end(), calleeName) - begin(variables[0]);
                        returnTypeClass = findClass(variables[1][idx]);
                    } else {
                        error(callNode);
                    }
                }

                if (method != nullptr) {
                    if (!method->parameterTypes.empty()) {
                        if (method->parameterTypes.size() != callNode->arguments[argCount].size()) {
                            error(callNode, "invalid method arguments");
                        }
                        for (size_t i = 0; i < callNode->arguments[argCount].size(); ++i) {
                            ExpressionNode expNode = callNode->arguments[argCount][i];
                            if (!checkExpression(&expNode, validVariables, validMethods,
                                                 variableTypes, variableIndices, currentClass, method->parameterTypes[i])) {
                                error(&expNode);
                            }
                        }
                    }
                    returnTypeClass = method->returnType.empty() ? nullptr : findClass(method->returnType);
                }
                argCount++;
            }

            return returnTypeClass;
        } else {
            if (!node->booleanValue.empty()) {
                return findClass("Boolean");
            }
            if (!node->integerValue.empty()) {
                return findClass("Integer");
            }
            if (!node->floatValue.empty()) {
                return findClass("Real");
            }
        }

        return nullptr;
    }

    bool checkCall(const CallNode* callNode,
                   const std::vector<std::string> validVariables,
                   const std::vector<std::string> validMethods,
                   const std::vector<std::string> variableTypes,
                   const std::vector<std::string> variableIndices,
                   ClassNode* currentClass,
                   const std::string targetReturnType) {
        ClassNode* returnTypeClass = nullptr;
        size_t argCount = 0;

        if (callNode->parentName == "this") {
            returnTypeClass = currentClass;
        } else {
            if (!callNode->parentName.empty()) {
                auto it = std::find(validVariables.begin(), validVariables.end(), callNode->parentName);
                if (it != validVariables.end()) {
                    std::string type = variableTypes[std::distance(validVariables.begin(), it)];
                    if (std::distance(validVariables.begin(), it) < (int) varUsed.size())
                        varUsed[stoi(variableIndices[std::distance(validVariables.begin(), it)])] = true;
                    if (type.length() >= 5 && type.substr(0, 5) == "Array")
                        type = "Array";
                    returnTypeClass = findClass(type);
                } else if (findClass(callNode->parentName)) {
                    returnTypeClass = findClass(callNode->parentName);
                } else {
                    return false;
                }
            }
        }


        if (returnTypeClass == nullptr && callNode->parentNames.empty()) {
            error(callNode);
            return false;
        }

        int itr = 0;

        for (const auto& calleeName : callNode->parentNames) {
            MethodNode* method = nullptr;
            if (returnTypeClass == nullptr) {
                if (calleeName != callNode->parentNames.front()) {
                    return false;
                }
                if (std::find(begin(validMethods), end(validMethods), calleeName) != validMethods.end()) {
                    method = getMethod(currentClass, calleeName);
                } else if (std::find(begin(validVariables), end(validVariables), calleeName) != validVariables.end()) {
                    method = nullptr;
                    int idx = std::find(begin(validVariables), end(validVariables), calleeName) - begin(validVariables);
                    if (idx < (int) varUsed.size())
                        varUsed[stoi(variableIndices[idx])] = true;
                    returnTypeClass = findClass(variableTypes[idx]);
                } else if (findClass(calleeName) != nullptr) {
                    method = nullptr;
                    ClassNode* constructorClass = findClass(calleeName);
                    bool validArgs = false;
                    std::string argumentSignature;
                    for (auto argument : callNode->arguments[argCount]) {
                        ExpressionNode expNode = (ExpressionNode) argument;
                        argumentSignature += getReturnType(&expNode,
                                                           validVariables,
                                                           validMethods,
                                                           variableTypes,
                                                           variableIndices,
                                                           currentClass)->name;
                    }
                    for (const auto& constructor : constructorClass->constructors) {
                        if (constructor->parameterTypes.size() == callNode->arguments[argCount].size()) {
                            std::string parameterSignature;
                            for (const auto& parameterType : constructor->parameterTypes) {
                                parameterSignature += parameterType;
                            }
                            if (parameterSignature == argumentSignature) {
                                validArgs = true;
                                break;
                            }
                        }
                    }
                    if (!validArgs) {
                        error(callNode, ("Invalid arguments to " + constructorClass->name));
                        return false;
                    }
                    returnTypeClass = constructorClass;
                } else {
                    return false;
                }
            } else {
                std::vector<std::string> tmpMethods = getMethods(returnTypeClass)[0];
                std::vector<std::string> tmpVariables = getVariables(returnTypeClass)[0];
                if (std::find(tmpMethods.begin(), tmpMethods.end(), calleeName) != tmpMethods.end()) {
                    method = getMethod(returnTypeClass, calleeName);
                } else if (std::find(tmpVariables.begin(), tmpVariables.end(), calleeName) != tmpVariables.end()) {
                    auto variables = getVariables(returnTypeClass);
                    int idx = std::find(variables[0].begin(), variables[0].end(), calleeName) - begin(variables[0]);
                    returnTypeClass = findClass(variables[1][idx]);
                } else {
                    error(callNode);
                    return false;
                }
            }

            if (method != nullptr) {
                if (!method->parameterTypes.empty()) {
                    if (method->parameterTypes.size() != callNode->arguments[argCount].size()) {
                        error(callNode, " method arguments mismatch");
                        return false;
                    }

                    for (size_t i = 0; i < callNode->arguments[argCount].size(); ++i) {
                        ExpressionNode expressionNode = (ExpressionNode) (callNode->arguments[argCount][i]);
                        if (!checkExpression(&expressionNode, validVariables,
                                             validMethods, variableTypes, variableIndices,
                                             currentClass, method->parameterTypes[i])) {
                            error(&callNode->arguments[argCount][i]);
                            return false;
                        }
                    }
                }
                if (method->returnType != "T") {
                    returnTypeClass = (method->returnType.empty()) ? nullptr : findClass(method->returnType);
                } else {
                    returnTypeClass = findClass(targetReturnType);
                }
            }
            argCount++;
        }

        if (targetReturnType != "-1" && returnTypeClass == nullptr && !targetReturnType.empty()) {
            error(callNode, (" expected type " + targetReturnType));
            return false;
        }

        if (returnTypeClass == nullptr && targetReturnType.empty()) {
            return true;
        }

        std::vector<std::string> superClasses = getSuperClasses(returnTypeClass);
        if (targetReturnType != "-1" && returnTypeClass->name != targetReturnType
                && std::find(superClasses.begin(), superClasses.end(), targetReturnType) == superClasses.end()) {
            error(callNode);
            return false;
        }

        return true;
    }

    bool checkReturnStatement(ReturnStatementNode* node,
                              const std::vector<std::string> validVariables,
                              const std::vector<std::string> validMethods,
                              const std::vector<std::string> variableTypes,
                              const std::vector<std::string> variableIndices,
                              ClassNode* currentClass,
                              const std::string targetReturnType) {
        ExpressionNode expressionNode = (ExpressionNode) node->expression;
        return checkExpression(&expressionNode, validVariables, validMethods, variableTypes, variableIndices, currentClass, targetReturnType);
    }

    bool checkNode(Node* node,
                   const std::vector<std::string> validVariables,
                   const std::vector<std::string> validMethods,
                   const std::vector<std::string> variableTypes,
                   const std::vector<std::string> variableIndices,
                   ClassNode* currentClass,
                   const std::string targetReturnType) {
        std::string type = node->nodeType;
        if (type == "VariableNode") {
            auto varDeclNode = (VariableNode*) node;
            if (varDeclNode->expression.expressionType == "call") {
                if (!checkNode(varDeclNode->expression.call,
                               validVariables,
                               validMethods,
                               variableTypes,
                               variableIndices,
                               currentClass,
                               targetReturnType)) {
                    return false;
                }
            }
        }
        if (type == "ConditionalNode") {
            auto ifStmtNode = (ConditionalNode*) node;
            if (ifStmtNode->expression.expressionType == "call") {
                if (!checkCall(ifStmtNode->expression.call, validVariables, validMethods, variableTypes, variableIndices,
                               currentClass, "Boolean"))
                    return false;
            }
            if (!checkBody(ifStmtNode->ifBodyNodes, validVariables, validMethods, variableTypes, variableIndices,
                           currentClass, targetReturnType)) {
                error(node, "in if body");
                return false;
            }
            if (!checkBody(ifStmtNode->elseBodyNodes, validVariables, validMethods, variableTypes, variableIndices,
                           currentClass, targetReturnType)) {
                error(node, "in else body");
                return false;
            }
        }
        if (type == "LoopNode") {
            auto loopNode = (LoopNode*) node;
            if (loopNode->expression.expressionType == "call") {
                if (!checkCall(loopNode->expression.call, validVariables, validMethods, variableTypes, variableIndices,
                               currentClass, "Boolean"))
                    return false;
            }
            if (!checkBody(loopNode->bodyNodes, validVariables, validMethods, variableTypes, variableIndices,
                           currentClass, targetReturnType)) {
                error(node, "in loop body");
                return false;
            }
        }
        if (type == "AssignmentNode") {
            auto assignNode = (AssignmentNode*) node;
            if (std::find(validVariables.begin(), validVariables.end(), assignNode->name) == validVariables.end()) {
                error(node, ("variable \'" + (assignNode->name) + "\' not declared"));
                return false;
            }
            if (assignNode->expression->expressionType == "call") {
                auto index = std::distance(validVariables.begin(), std::find(validVariables.begin(), validVariables.end(), assignNode->name));
                std::string targetType = variableTypes[index];
                return checkCall(assignNode->expression->call, validVariables, validMethods, variableTypes, variableIndices,
                                 currentClass, targetType);
            }
        }
        return true;
    }

    bool checkBody(const std::vector<Node*> body,
                   const std::vector<std::string> validVariables,
                   const std::vector<std::string> validMethods,
                   const std::vector<std::string> variableTypes,
                   const std::vector<std::string> variableIndices,
                   ClassNode* currentClass,
                   const std::string& targetReturnType) {
        const auto checkDupVariableName = [&](std::string & errorMessage) {
            std::set<std::string> varNameSet;
            for (const auto& node : body) {
                std::string type = node->nodeType;
                if (type == "VariableNode") {
                    VariableNode* varNode = (VariableNode*) node;
                    std::string varName = varNode->name;
                    if (!varNameSet.empty()
                            && varNameSet.count(varName)) {
                        errorMessage = "duplicate variable name [" + varName + "] in body";
                        return false;
                    }
                    varNameSet.insert(varName);
                }
            }
            return true;
        };

        std::string errorMessage;
        if (!checkDupVariableName(errorMessage)) {
            error(currentClass, errorMessage);
        }


        std::vector<std::string> newVariables;
        std::vector<std::string> newTypes;
        std::vector<std::string> newVariableIndices;

        for (const auto& node : body) {
            std::string type = node->nodeType;
            if (type == "VariableNode") {
                VariableNode* varNode = (VariableNode*) node;
                newVariables.push_back(varNode->name);
                newTypes.push_back(varNode->expression.call->parentNames[0]);
                ++varIndex;
                newVariableIndices.push_back(std::to_string(varIndex - 1));
                int idx = varIndex - 1;
                varUsed.resize(varIndex);
                if (varNode->expression.expressionType == "call") {
                    CallNode* callNode = varNode->expression.call;
                    if (!checkCall(callNode, concat(validVariables, newVariables),
                                   validMethods, concat(variableTypes, newTypes),
                                   concat(variableIndices, newVariableIndices),
                                   currentClass, "-1")) {
                        error(node, ("invalid expression assigned to variable \'" + varNode->name + "\'"));
                        return false;
                    }
                }
            } else if (type == "CallNode") {
                if (!checkCall((CallNode*) node, concat(validVariables, newVariables),
                               validMethods, concat(variableTypes, newTypes),
                               concat(variableIndices, newVariableIndices),
                               currentClass, "-1")) {
                    error(node);
                    return false;
                }
            } else if (type == "ReturnStatementNode") {
                if (!checkReturnStatement((ReturnStatementNode*) node,
                                          concat(validVariables, newVariables),
                                          validMethods,
                                          concat(variableTypes, newTypes),
                                          concat(variableIndices, newVariableIndices),
                                          currentClass,
                                          targetReturnType)) {
                    error(node);
                    return false;
                }
            } else if (!checkNode(node,
                                  concat(validVariables, newVariables),
                                  validMethods,
                                  concat(variableTypes, newTypes),
                                  concat(variableIndices, newVariableIndices),
                                  currentClass,
                                  targetReturnType)) {
                error(node);
                return false;
            }
        }

        int sz = (int) newVariables.size();
        for (int i = 0; i < sz; i++) {
            int varIndex = stoi(newVariableIndices[i]);
#ifdef UNUSED_VAR
            if (!varUsed[varIndex]) {
                std::cout << "Error: Unused variable \"" << newVariables[i] << "\"" << std::endl;
                std::exit(0);
            }
#endif
        }
        return true;
    }

    void addInBuiltClasses() {
        ClassNode* classInteger = new ClassNode("Integer", "AnyValue",
        std::vector<MethodNode*> {
            new MethodNode("toReal", "Real"),
            new MethodNode("toBoolean", "Boolean"),
            new MethodNode("UnaryMinus", "Integer"),
            new MethodNode("UnaryPlus", "Integer"),
            new MethodNode("Plus", "Integer", std::vector<std::string> {"p"},
            std::vector<std::string> {"Integer"}),
            new MethodNode("Minus", "Integer", std::vector<std::string> {"p"},
            std::vector<std::string> {"Integer"}),
            new MethodNode("Mult", "Integer", std::vector<std::string> {"p"},
            std::vector<std::string> {"Integer"}),
            new MethodNode("Div", "Integer", std::vector<std::string> {"p"},
            std::vector<std::string> {"Integer"}),
            new MethodNode("Plus", "Real", std::vector<std::string> {"p"}, std::vector<std::string> {"Real"}),
            new MethodNode("Minus", "Real", std::vector<std::string> {"p"}, std::vector<std::string> {"Real"}),
            new MethodNode("Mult", "Real", std::vector<std::string> {"p"}, std::vector<std::string> {"Real"}),
            new MethodNode("Div", "Real", std::vector<std::string> {"p"}, std::vector<std::string> {"Real"}),
            new MethodNode("Rem", "Integer", std::vector<std::string> {"p"},
            std::vector<std::string> {"Integer"}),
            new MethodNode("Less", "Boolean", std::vector<std::string> {"p"},
            std::vector<std::string> {"Integer"}),
            new MethodNode("LessEqual", "Boolean", std::vector<std::string> {"p"},
            std::vector<std::string> {"Integer"}),
            new MethodNode("Greater", "Boolean", std::vector<std::string> {"p"},
            std::vector<std::string> {"Integer"}),
            new MethodNode("GreaterEqual", "Boolean", std::vector<std::string> {"p"},
            std::vector<std::string> {"Integer"}),
            new MethodNode("Equal", "Boolean", std::vector<std::string> {"p"},
            std::vector<std::string> {"Integer"}),
            new MethodNode("Less", "Boolean", std::vector<std::string> {"p"}, std::vector<std::string> {"Real"}),
            new MethodNode("LessEqual", "Boolean", std::vector<std::string> {"p"},
            std::vector<std::string> {"Real"}),
            new MethodNode("Greater", "Boolean", std::vector<std::string> {"p"},
            std::vector<std::string> {"Real"}),
            new MethodNode("GreaterEqual", "Boolean", std::vector<std::string> {"p"},
            std::vector<std::string> {"Real"}),
            new MethodNode("Equal", "Boolean", std::vector<std::string> {"p"},
            std::vector<std::string> {"Real"}),
            new MethodNode(true)
        },
        std::vector<ConstructorNode*> {
            new ConstructorNode(),
            new ConstructorNode(std::vector<std::string>{"p"}, std::vector<std::string>{"Integer"}),
            // new ConstructorNode(std::vector<std::string>{"p"}, std::vector<std::string>{"Real"})
        },
        std::vector<VariableNode*> {
            new VariableNode("Min", "Integer"),
            new VariableNode("Max", "Integer")
        });
        ClassNode* classReal = new ClassNode("Real", "AnyValue",
        std::vector<MethodNode*> {
            new MethodNode("toInteger", "Integer"),
            new MethodNode("toBoolean", "Boolean"),
            new MethodNode("UnaryMinus", "Real"),
            new MethodNode("UnaryPlus", "Real"),
            // new MethodNode("Plus", "Integer", std::vector<std::string> {"p"},
            // std::vector<std::string> {"Integer"}),
            // new MethodNode("Minus", "Integer", std::vector<std::string> {"p"},
            // std::vector<std::string> {"Integer"}),
            // new MethodNode("Mult", "Integer", std::vector<std::string> {"p"},
            // std::vector<std::string> {"Integer"}),
            // new MethodNode("Div", "Integer", std::vector<std::string> {"p"},
            // std::vector<std::string> {"Integer"}),
            new MethodNode("Plus", "Real", std::vector<std::string> {"p"}, std::vector<std::string> {"Real"}),
            new MethodNode("Minus", "Real", std::vector<std::string> {"p"}, std::vector<std::string> {"Real"}),
            new MethodNode("Mult", "Real", std::vector<std::string> {"p"}, std::vector<std::string> {"Real"}),
            new MethodNode("Div", "Real", std::vector<std::string> {"p"}, std::vector<std::string> {"Real"}),
            new MethodNode("Rem", "Real", std::vector<std::string> {"p"}, std::vector<std::string> {"Integer"}),
            new MethodNode("Less", "Boolean", std::vector<std::string> {"p"},
            std::vector<std::string> {"Integer"}),
            new MethodNode("LessEqual", "Boolean", std::vector<std::string> {"p"},
            std::vector<std::string> {"Integer"}),
            new MethodNode("Greater", "Boolean", std::vector<std::string> {"p"},
            std::vector<std::string> {"Integer"}),
            new MethodNode("GreaterEqual", "Boolean", std::vector<std::string> {"p"},
            std::vector<std::string> {"Integer"}),
            new MethodNode("Equal", "Boolean", std::vector<std::string> {"p"},
            std::vector<std::string> {"Integer"}),
            new MethodNode("Less", "Boolean", std::vector<std::string> {"p"}, std::vector<std::string> {"Real"}),
            new MethodNode("LessEqual", "Boolean", std::vector<std::string> {"p"},
            std::vector<std::string> {"Real"}),
            new MethodNode("Greater", "Boolean", std::vector<std::string> {"p"},
            std::vector<std::string> {"Real"}),
            new MethodNode("GreaterEqual", "Boolean", std::vector<std::string> {"p"},
            std::vector<std::string> {"Real"}),
            new MethodNode("Equal", "Boolean", std::vector<std::string> {"p"},
            std::vector<std::string> {"Real"}),
            new MethodNode(true)
        },
        std::vector<ConstructorNode*>
        {
            new ConstructorNode(),
            new ConstructorNode(std::vector<std::string> {"p"}, std::vector<std::string> {"Integer"}),
            new ConstructorNode(std::vector<std::string> {"p"}, std::vector<std::string> {"Real"})
        },
        std::vector<VariableNode*>
        {
            new VariableNode("Min", "Real"),
            new VariableNode("Max", "Real"),
            new VariableNode("Epsilon", "Real")
        });
        ClassNode* classBoolean = new ClassNode("Boolean", "AnyValue",
        std::vector<MethodNode*> {
            new MethodNode("toInteger", "Integer"),
            new MethodNode("Or", "Boolean", std::vector<std::string> {"p"},
            std::vector<std::string> {"Boolean"}),
            new MethodNode("And", "Boolean", std::vector<std::string> {"p"},
            std::vector<std::string> {"Boolean"}),
            new MethodNode("Xor", "Boolean", std::vector<std::string> {"p"},
            std::vector<std::string> {"Boolean"}),
            new MethodNode("Not", "Boolean"),
            new MethodNode(true)
        }, std::vector<ConstructorNode*> {
            new ConstructorNode(),
            new ConstructorNode(std::vector<std::string>{"p"}, std::vector<std::string>{"Boolean"} )
        },
        std::vector<VariableNode*> {});
        ClassNode* classArray = new ClassNode("Array", "AnyRef", "T",
        std::vector<MethodNode*> {
            new MethodNode("toList", "List"),
            new MethodNode("Length", "Integer"),
            new MethodNode("get", "T", std::vector<std::string> {"i"},
            std::vector<std::string> {"Integer"}),
            new MethodNode("set", std::vector<std::string> {"i", "v"},
            std::vector<std::string> {"Integer", "T"}),
            new MethodNode(true)
        }
        , std::vector<ConstructorNode*>
        {
            new ConstructorNode(),
            new ConstructorNode(std::vector<std::string>{"l"}, std::vector<std::string>{"Integer"} )
        },
        std::vector<VariableNode*> {});
        ClassNode* classList = new ClassNode("List", "AnyRef", "T",
                                             std::vector<MethodNode*>
        {
            new MethodNode("append", "List", std::vector<std::string> {"v"},
            std::vector<std::string> {"T"}),
            new MethodNode("head", "T"),
            new MethodNode("tail", "T"),
            new MethodNode(true)
        }
        , std::vector<ConstructorNode*>
        {
            new ConstructorNode(),
            new ConstructorNode(std::vector<std::string>{}, std::vector<std::string>{} ),
            new ConstructorNode(std::vector<std::string>{"p"}, std::vector<std::string>{"T"} ),
            new ConstructorNode(std::vector<std::string>{"p", "count"}, std::vector<std::string>{"T", "Integer"} )
        },
        std::vector<VariableNode*> {});
        ClassNode* anyValue = new ClassNode("AnyValue", "", std::vector<MethodNode*> {}, std::vector<ConstructorNode*> {},
                                            std::vector<VariableNode*> {});
        ClassNode* anyRef = new ClassNode("AnyRef", "", std::vector<MethodNode*> {}, std::vector<ConstructorNode*> {},
                                          std::vector<VariableNode*> {});
        rootNode->classNodes.push_back(classInteger);
        rootNode->classNodes.push_back(classReal);
        rootNode->classNodes.push_back(classBoolean);
        rootNode->classNodes.push_back(classArray);
        rootNode->classNodes.push_back(classList);
        rootNode->classNodes.push_back(anyValue);
        rootNode->classNodes.push_back(anyRef);
    }

    MethodNode* generateDestructor(ClassNode* classNode) {
        for (auto method : classNode->methods)
            if (method->isDestructor)
                return method;

        MethodNode* destructor = new MethodNode(true);

        for (auto declaration : classNode->variables) {
            CallNode call;
            call.setCallerName(declaration->name);
            std::string className = declaration->expression.call->parentNames[0];
            for (auto& classNode : rootNode->classNodes) {
                if (classNode->name == className) {
                    call.addCallee(generateDestructor(classNode)->name, std::vector<ExpressionNode>());
                }
            }
            destructor->addBodyNode(&call);
        }

        return destructor;
    }

    void addDestructors() {
        for (auto classNode : rootNode->classNodes) {
            bool noDestructor = true;
            for (const auto method : classNode->methods) {
                if (method->isDestructor) {
                    noDestructor = false;
                    break;
                }
            }
            if (noDestructor) {
                classNode->addMethodDeclaration(generateDestructor(classNode));
            }
        }
    }

    void analyze(RootNode* rootNode, std::vector<Token> tokens) {
        this->rootNode = rootNode;
        this->tokens = tokens;
        this->varIndex = 0;
        addInBuiltClasses();
        for (int i = 0; i < int(rootNode->classNodes.size()) - 7; i++) {
            checkClass(rootNode->classNodes[i]);
        }
        // addDestructors();
        std::cout << "No Semantic Error Found" << std::endl;
        for (int i = 0; i < 7; i++) {
            rootNode->classNodes.pop_back();
        }
    }
};


// int main(int argc, char** argv) {
//     std::string file = argv[1];

//     Manager manager;
//     std::string source;

//     freopen(file.c_str(), "r", stdin);

//     char ch;
//     while (EOF != (ch = getchar())) {
//         source += ch;
//     }

//     std::vector<Token> tokens = manager.analyze(source);

//     Parser parser;
//     RootNode* node = parser.startParsing(tokens);

//     Analyzer analyzer;
//     analyzer.analyze(node, tokens);
// }