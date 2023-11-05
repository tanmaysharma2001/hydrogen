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
public:
    std::vector<std::vector<std::string>> getVariables(const ClassNode* node) {
        std::vector<std::string> variableNames;
        std::vector<std::string> variableTypes;

        for (const auto& variable : node->variables) {
            std::string name = variable->name;
            std::string type = variable->expression.call->parentNames[0];
            variableNames.push_back(name);
            variableTypes.push_back(type);
        }

        return {variableNames, variableTypes};
    }

    ClassNode* findClass(const std::string& name) {
        for (auto declaration : rootNode->classNodes)
            if (name == declaration->name)
                return declaration;

        return nullptr;
    }

    void error(const Node* node) {
        std::cout << "ERROR\n\n";
        Token token = this->tokens[node->tokenNumber - 1];
        std::cout << "Invalid semantics at " << token.line << ":" << token.column << ".\n";
        std::cout << "Invalid token at " << token.line << ":" << token.column << ".\n";
        std::exit(0);
    }

    std::vector<std::string> getSuperClasses(ClassNode* node) {
        std::vector<std::string> result;
        ClassNode* tmp = node;
        while (tmp != nullptr && !tmp->superClass.empty()) {
            result.push_back(tmp->superClass);
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
        std::vector<std::vector<std::string>> variablesAndTypes = getVariables(classNode);
        std::vector<std::vector<std::string>> methodsAndTypes = getMethods(classNode);
        std::vector<std::string> validVariables = variablesAndTypes[0];
        std::vector<std::string> validMethods = methodsAndTypes[0];
        std::vector<std::string> variableTypes = variablesAndTypes[1];
        std::vector<std::string> methodTypes = methodsAndTypes[1];
        for (const auto& method : classNode->methods) {
            checkBody(method->bodyNodes, concat(validVariables, method->parameterNames),
                      validMethods, concat(variableTypes, method->parameterTypes), classNode, method->returnType);
        }
    }

    bool checkExpression(ExpressionNode* node,
                         const std::vector<std::string>& validVariables,
                         const std::vector<std::string>& validMethods,
                         const std::vector<std::string>& variableTypes,
                         ClassNode* currentClass,
                         const std::string& targetReturnType) {
        if (node->expressionType == "call") {
            return checkCall(node->call, validVariables, validMethods, variableTypes, currentClass, targetReturnType);
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

    MethodNode* getMethod(const ClassNode* classNode, const std::string& methodName) {
        for (auto method : classNode->methods) {
            if (method->name == methodName) {
                return method;
            }
        }
        return nullptr;
    }

    ClassNode* getReturnType(ExpressionNode* node,
                             std::vector<std::string> validVariables,
                             std::vector<std::string> validMethods,
                             std::vector<std::string> variableTypes,
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
                        bool isArgumentsOK = false;
                        std::string argumentSignature;
                        for (auto argument : callNode->arguments[argCount]) {
                            ExpressionNode expNode = (ExpressionNode) argument;
                            argumentSignature += getReturnType(&expNode,
                                                               validVariables,
                                                               validMethods,
                                                               variableTypes,
                                                               currentClass)->name;
                        }
                        for (auto constructor : constructorClass->constructors) {
                            if (constructor->parameterTypes.size() != callNode->arguments[argCount].size()) {
                                std::string parameterSignature;
                                for (const std::string& parameterType : constructor->parameterTypes) {
                                    parameterSignature += parameterType;
                                }
                                if (parameterSignature == argumentSignature) {
                                    isArgumentsOK = true;
                                    break;
                                }
                            }
                        }

                        if (!isArgumentsOK) {
                            error(callNode);
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
                            error(callNode);
                        }
                        for (size_t i = 0; i < callNode->arguments[argCount].size(); ++i) {
                            ExpressionNode expNode = callNode->arguments[argCount][i];
                            if (!checkExpression(&expNode, validVariables, validMethods,
                                                 variableTypes, currentClass, method->parameterTypes[i])) {
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
                    returnTypeClass = findClass(variableTypes[std::distance(validVariables.begin(), it)]);
                } else {
                    error(callNode);
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
                    error(callNode);
                    return false;
                }
                if (std::find(begin(validMethods), end(validMethods), calleeName) != validMethods.end()) {
                    method = getMethod(currentClass, calleeName);
                } else if (std::find(begin(validVariables), end(validVariables), calleeName) != validVariables.end()) {
                    method = nullptr;
                    int idx = std::find(begin(validVariables), end(validVariables), calleeName) - begin(validVariables);
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
                        error(callNode);
                        return false;
                    }
                    returnTypeClass = constructorClass;
                } else {
                    error(callNode);
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
                        error(callNode);
                        return false;
                    }

                    for (size_t i = 0; i < callNode->arguments[argCount].size(); ++i) {
                        ExpressionNode expressionNode = (ExpressionNode) (callNode->arguments[argCount][i]);
                        if (!checkExpression(&expressionNode, validVariables,
                                             validMethods, variableTypes, currentClass, method->parameterTypes[i])) {
                            error(&callNode->arguments[argCount][i]);
                            return false;
                        }
                    }
                }
                returnTypeClass = (method->returnType.empty()) ? nullptr : findClass(method->returnType);
            }
            argCount++;
        }

        if (targetReturnType != "-1" && returnTypeClass == nullptr && !targetReturnType.empty()) {
            error(callNode);
            return false;
        }

        if (returnTypeClass == nullptr && targetReturnType.empty()) {
            return true;
        }

        std::vector<std::string> superClasses = getSuperClasses(returnTypeClass);
        if (targetReturnType != "-1" && returnTypeClass->name != targetReturnType
                // && std::find(superClasses.begin(), superClasses.end(), targetReturnType) == superClasses.end())
           ) {
            error(callNode);
            return false;
        }


        return true;
    }

    bool checkReturnStatement(ReturnStatementNode* node,
                              const std::vector<std::string> validVariables,
                              const std::vector<std::string> validMethods,
                              const std::vector<std::string> variableTypes,
                              ClassNode* currentClass,
                              const std::string targetReturnType) {
        ExpressionNode expressionNode = (ExpressionNode) node->expression;
        return checkExpression(&expressionNode, validVariables, validMethods, variableTypes, currentClass, targetReturnType);
    }

    bool checkNode(Node* node,
                   const std::vector<std::string> validVariables,
                   const std::vector<std::string> validMethods,
                   const std::vector<std::string> variableTypes,
                   ClassNode* currentClass,
                   const std::string targetReturnType) {
        std::string type = node->nodeType;
        if (type == "VariableNode") {
            auto varDeclNode = (VariableNode*) node;
            if (varDeclNode->expression.expressionType == "call") {
                return checkNode(varDeclNode->expression.call,
                                 validVariables,
                                 validMethods,
                                 variableTypes,
                                 currentClass,
                                 targetReturnType);
            }
        }
        if (type == "ConditionalNode") {
            auto ifStmtNode = (ConditionalNode*) node;
            if (ifStmtNode->expression.expressionType == "call") {
                if (!checkCall(ifStmtNode->expression.call, validVariables, validMethods, variableTypes, currentClass, "Boolean"))
                    return false;
            }
            if (!checkBody(ifStmtNode->ifBodyNodes, validVariables, validMethods, variableTypes, currentClass, targetReturnType)) {
                error(node);
                return false;
            }
            if (!checkBody(ifStmtNode->elseBodyNodes, validVariables, validMethods, variableTypes, currentClass, targetReturnType)) {
                error(node);
                return false;
            }
        }
        if (type == "LoopNode") {
            auto loopNode = (LoopNode*) node;
            if (loopNode->expression.expressionType == "call") {
                if (!checkCall(loopNode->expression.call, validVariables, validMethods, variableTypes, currentClass, "Boolean"))
                    return false;
            }
            if (!checkBody(loopNode->bodyNodes, validVariables, validMethods, variableTypes, currentClass, targetReturnType)) {
                error(node);
                return false;
            }
        }
        if (type == "AssignmentNode") {
            auto assignNode = (AssignmentNode*) node;
            if (std::find(validVariables.begin(), validVariables.end(), assignNode->name) == validVariables.end()) {
                error(node);
                return false;
            }
            if (assignNode->expression->expressionType == "call") {
                auto index = std::distance(validVariables.begin(), std::find(validVariables.begin(), validVariables.end(), assignNode->name));
                std::string targetType = variableTypes[index];
                return checkCall(assignNode->expression->call, validVariables, validMethods, variableTypes, currentClass, targetType);
            }
        }
        return true;
    }

    bool checkBody(const std::vector<Node*> body,
                   const std::vector<std::string> validVariables,
                   const std::vector<std::string> validMethods,
                   const std::vector<std::string> variableTypes,
                   ClassNode* currentClass,
                   const std::string& targetReturnType) {
        std::vector<std::string> newVariables;
        std::vector<std::string> newTypes;
        for (const auto& node : body) {
            std::string type = node->nodeType;
            if (type == "VariableNode") {
                VariableNode* varNode = (VariableNode*) node;
                newVariables.push_back(varNode->name);
                newTypes.push_back(varNode->expression.call->parentNames[0]);
            } else if (type == "CallNode") {
                if (!checkCall((CallNode*) node, concat(validVariables, newVariables),
                               validMethods, concat(variableTypes, newTypes),
                               currentClass, "-1")) {
                    return false;
                }
            } else if (type == "ReturnStatementNode") {
                if (!checkReturnStatement((ReturnStatementNode*) node,
                                          concat(validVariables, newVariables),
                                          validMethods,
                                          concat(variableTypes, newTypes),
                                          currentClass,
                                          targetReturnType)) {
                    error(node);
                    return false;
                }
            } else if (!checkNode(node,
                                  concat(validVariables, newVariables),
                                  validMethods,
                                  concat(variableTypes, newTypes),
                                  currentClass,
                                  targetReturnType)) {
                error(node);
                return false;
            }
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
            new ConstructorNode(std::vector<std::string>{"p"}, std::vector<std::string>{"Real"})
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
        rootNode->classNodes.push_back(classArray);
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
        addInBuiltClasses();
        for (int i = 0; i < int(rootNode->classNodes.size()) - 7; i++) {
            checkClass(rootNode->classNodes[i]);
        }
        addDestructors();
        std::cout << "No Semantic Error Found" << std::endl;
    }
};


int main(int argc, char** argv) {
    std::string file = argv[1];

    Manager manager;
    std::string source;

    freopen(file.c_str(), "r", stdin);

    char ch;
    while (EOF != (ch = getchar())) {
        source += ch;
    }

    std::vector<Token> tokens = manager.analyze(source);

    Parser parser;
    RootNode* node = parser.startParsing(tokens);

    Analyzer analyzer;
    analyzer.analyze(node, tokens);
}