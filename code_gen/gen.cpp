#include <memory>
#include <unordered_map>
#include <algorithm>
#include <iostream>
#include <iterator>
#include <cassert>
#include <fstream>
#include <string>
#include <sstream>

#include "llvm/ADT/APFloat.h"
#include "llvm/ADT/STLExtras.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Type.h"
#include "llvm/IR/Verifier.h"
#include "llvm/Support/TargetSelect.h"
#include "llvm/Target/TargetMachine.h"
#include "llvm/Transforms/Scalar.h"
#include "llvm/Transforms/Scalar/GVN.h"
#include "llvm/ADT/STLExtras.h"
#include "llvm/Target/TargetOptions.h"
#include "llvm/MC/TargetRegistry.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Support/FileSystem.h"
#include "llvm/ADT/Triple.h"
#include "llvm/Support/Host.h"
#include "llvm/CodeGen/Passes.h"
#include "llvm/CodeGen/TargetPassConfig.h"

#include "../semantic_analyzer/analyzer.cpp"

using namespace llvm;

class Generator {
private:
    llvm::LLVMContext TheContext;
    llvm::IRBuilder<> Builder;
    std::unique_ptr<llvm::Module> TheModule;
    llvm::TargetMachine* targetMachine;
    llvm::raw_fd_ostream output;
    std::error_code EC;

    std::unordered_map<std::string, bool> variables;
    std::unordered_map<std::string, ExpressionNode*> variableValue;
    std::unordered_map<std::string, std::string> variableType;
public:
    Generator(const std::string& filename) : Builder{TheContext}, output{filename, EC, llvm::sys::fs::OF_None} {
        TheModule = std::make_unique<llvm::Module>("hydrogen", TheContext);
        auto TargetTriple = llvm::sys::getDefaultTargetTriple();
        llvm::InitializeAllTargetInfos();
        llvm::InitializeAllTargets();
        llvm::InitializeAllTargetMCs();
        llvm::InitializeAllAsmParsers();
        llvm::InitializeAllAsmPrinters();
        std::string Error;
        auto Target = llvm::TargetRegistry::lookupTarget(TargetTriple, Error);
        if (!Target) {
            llvm::errs() << Error;
        }
        auto CPU = "generic";
        auto Features = "";
        llvm::TargetOptions opt;
        auto RM = llvm::Optional<llvm::Reloc::Model>();
        targetMachine = Target->createTargetMachine(TargetTriple, CPU, Features, opt, RM);

        TheModule->setDataLayout(targetMachine->createDataLayout());
        TheModule->setTargetTriple(TargetTriple);
    }

    void generateIntegerStruct() {
        llvm::StructType* integerStructType = llvm::StructType::create(TheContext, "Integer");
        integerStructType->setBody(llvm::Type::getInt32Ty(TheContext), /* isPacked */ false);
        generatePlusObject(integerStructType);
        generateMinusObject(integerStructType);
        generatePlusMethod(integerStructType);
        generateMinusMethod(integerStructType);
        generateIntegerLessObject(integerStructType);
        generateIntegerLessConstant(integerStructType);
    }

    void generatePlusObject(llvm::StructType* integerStructType) {
        llvm::FunctionType* plusFunctionType = llvm::FunctionType::get(
                llvm::Type::getInt32Ty(TheContext),
        {integerStructType->getPointerTo(), integerStructType->getPointerTo()},
        false
                                               );
        llvm::Function* plusFunction = llvm::Function::Create(
                                           plusFunctionType,
                                           llvm::Function::ExternalLinkage,
                                           "Plus",
                                           TheModule.get()
                                       );
        llvm::BasicBlock* entryBlock = llvm::BasicBlock::Create(TheContext, "entry", plusFunction);
        llvm::IRBuilder<> Builder(entryBlock);
        llvm::Argument* thisParam = plusFunction->arg_begin();
        thisParam->setName("this");
        llvm::Argument* othParam = std::next(plusFunction->arg_begin());
        othParam->setName("oth");
        llvm::Value* thisValAddr = Builder.CreateStructGEP(integerStructType, thisParam, 0, "thisValAddr");
        llvm::Value* thisVal = Builder.CreateLoad(llvm::Type::getInt32Ty(TheContext), thisValAddr, "thisVal");
        llvm::Value* othValAddr = Builder.CreateStructGEP(integerStructType, othParam, 0, "othValAddr");
        llvm::Value* othVal = Builder.CreateLoad(llvm::Type::getInt32Ty(TheContext), othValAddr, "othVal");
        llvm::Value* result = Builder.CreateAdd(thisVal, othVal, "result");
        Builder.CreateRet(result);
    }

    void generateMinusObject(llvm::StructType* integerStructType) {
        llvm::FunctionType* minusFunctionType = llvm::FunctionType::get(
                llvm::Type::getInt32Ty(TheContext),
        {integerStructType->getPointerTo(), integerStructType->getPointerTo()},
        false
                                                );

        llvm::Function* minusFunction = llvm::Function::Create(
                                            minusFunctionType,
                                            llvm::Function::ExternalLinkage,
                                            "Minus",
                                            TheModule.get()
                                        );
        llvm::BasicBlock* entryBlock = llvm::BasicBlock::Create(TheContext, "entry", minusFunction);
        llvm::IRBuilder<> Builder(entryBlock);
        llvm::Argument* thisParam = minusFunction->arg_begin();
        thisParam->setName("this");
        llvm::Argument* othParam = std::next(minusFunction->arg_begin());
        othParam->setName("oth");
        llvm::Value* thisValAddr = Builder.CreateStructGEP(integerStructType, thisParam, 0, "thisValAddr");
        llvm::Value* thisVal = Builder.CreateLoad(llvm::Type::getInt32Ty(TheContext), thisValAddr, "thisVal");
        llvm::Value* othValAddr = Builder.CreateStructGEP(integerStructType, othParam, 0, "othValAddr");
        llvm::Value* othVal = Builder.CreateLoad(llvm::Type::getInt32Ty(TheContext), othValAddr, "othVal");
        llvm::Value* result = Builder.CreateSub(thisVal, othVal, "result");
        Builder.CreateRet(result);
    }

    void generatePlusMethod(llvm::StructType* integerStructType) {
        llvm::FunctionType* plusFunctionType = llvm::FunctionType::get(
                llvm::Type::getInt32Ty(TheContext),
        {integerStructType->getPointerTo(), llvm::Type::getInt32Ty(TheContext)},
        false
                                               );
        llvm::Function* plusFunction = llvm::Function::Create(
                                           plusFunctionType,
                                           llvm::Function::ExternalLinkage,
                                           "Plus",
                                           TheModule.get()
                                       );
        llvm::BasicBlock* entryBlock = llvm::BasicBlock::Create(TheContext, "entry", plusFunction);
        llvm::IRBuilder<> Builder(entryBlock);
        llvm::Argument* thisParam = plusFunction->arg_begin();
        thisParam->setName("this");
        llvm::Argument* aParam = std::next(plusFunction->arg_begin());
        aParam->setName("a");
        llvm::Value* valAddr = Builder.CreateStructGEP(integerStructType, thisParam, 0, "valAddr");
        llvm::Value* val = Builder.CreateLoad(llvm::Type::getInt32Ty(TheContext), valAddr, "val");
        llvm::Value* result = Builder.CreateAdd(val, aParam, "result");
        Builder.CreateRet(result);
    }

    void generateMinusMethod(llvm::StructType* integerStructType) {
        llvm::FunctionType* minusFunctionType = llvm::FunctionType::get(
                llvm::Type::getInt32Ty(TheContext),
        {integerStructType->getPointerTo(), llvm::Type::getInt32Ty(TheContext)},
        false
                                                );
        llvm::Function* minusFunction = llvm::Function::Create(
                                            minusFunctionType,
                                            llvm::Function::ExternalLinkage,
                                            "Minus",
                                            TheModule.get()
                                        );
        llvm::BasicBlock* entryBlock = llvm::BasicBlock::Create(TheContext, "entry", minusFunction);
        llvm::IRBuilder<> Builder(entryBlock);
        llvm::Argument* thisParam = minusFunction->arg_begin();
        thisParam->setName("this");
        llvm::Argument* bParam = std::next(minusFunction->arg_begin());
        bParam->setName("b");
        llvm::Value* valAddr = Builder.CreateStructGEP(integerStructType, thisParam, 0, "valAddr");
        llvm::Value* val = Builder.CreateLoad(llvm::Type::getInt32Ty(TheContext), valAddr, "val");
        llvm::Value* result = Builder.CreateSub(val, bParam, "result");
        Builder.CreateRet(result);
    }

    void generateIntegerLessObject(llvm::StructType* integerStructType) {
        llvm::FunctionType* lessFunctionType = llvm::FunctionType::get(
                llvm::Type::getInt32Ty(TheContext),
        {integerStructType->getPointerTo(), integerStructType->getPointerTo()},
        false
                                               );
        llvm::Function* lessFunction = llvm::Function::Create(
                                           lessFunctionType,
                                           llvm::Function::ExternalLinkage,
                                           "Less",
                                           TheModule.get()
                                       );
        llvm::BasicBlock* entryBlock = llvm::BasicBlock::Create(TheContext, "entry", lessFunction);
        llvm::IRBuilder<> Builder(entryBlock);
        llvm::Argument* thisParam = lessFunction->arg_begin();
        thisParam->setName("this");
        llvm::Argument* othParam = std::next(lessFunction->arg_begin());
        othParam->setName("oth");
        llvm::Value* thisValAddr = Builder.CreateStructGEP(integerStructType, thisParam, 0, "thisValAddr");
        llvm::Value* thisVal = Builder.CreateLoad(llvm::Type::getInt32Ty(TheContext), thisValAddr, "thisVal");
        llvm::Value* othValAddr = Builder.CreateStructGEP(integerStructType, othParam, 0, "othValAddr");
        llvm::Value* othVal = Builder.CreateLoad(llvm::Type::getInt32Ty(TheContext), othValAddr, "othVal");
        llvm::Value* cmpResult = Builder.CreateICmpSLT(thisVal, othVal, "cmpResult");
        llvm::Value* result = Builder.CreateZExt(cmpResult, llvm::Type::getInt32Ty(TheContext), "boolToInt");
        Builder.CreateRet(result);
    }

    void generateIntegerLessConstant(llvm::StructType* integerStructType) {
        // Define the function type for the LessConstant method
        llvm::FunctionType* lessConstFunctionType = llvm::FunctionType::get(
                    llvm::Type::getInt32Ty(TheContext),
        {integerStructType->getPointerTo(), llvm::Type::getInt32Ty(TheContext)},
        false
                );
        llvm::Function* lessConstFunction = llvm::Function::Create(
                                                lessConstFunctionType,
                                                llvm::Function::ExternalLinkage,
                                                "LessConstant",
                                                TheModule.get()
                                            );
        llvm::BasicBlock* entryBlock = llvm::BasicBlock::Create(TheContext, "entry", lessConstFunction);
        llvm::IRBuilder<> Builder(entryBlock);
        llvm::Argument* thisParam = lessConstFunction->arg_begin();
        thisParam->setName("this");
        llvm::Argument* constParam = std::next(lessConstFunction->arg_begin());
        constParam->setName("constant");
        llvm::Value* thisValAddr = Builder.CreateStructGEP(integerStructType, thisParam, 0, "thisValAddr");
        llvm::Value* thisVal = Builder.CreateLoad(llvm::Type::getInt32Ty(TheContext), thisValAddr, "thisVal");
        llvm::Value* cmpResult = Builder.CreateICmpSLT(thisVal, constParam, "cmpResult");
        llvm::Value* result = Builder.CreateZExt(cmpResult, llvm::Type::getInt32Ty(TheContext), "boolToInt");
        Builder.CreateRet(result);
    }

    llvm::StructType* getType(const std::string& type) {
        if (type == "Integer") {
            return llvm::StructType::getTypeByName(TheContext, "Integer");
        } else {
            // handle other types
            return nullptr;
        }
    }

    void generateAssignmentNode(AssignmentNode* node) {
        // std::string name;
        // ExpressionNode* expression;
        // if (variables.count(node->name)) {
        //     std::cout << "re-assigning: " << node->name << std::endl;
        //     std::string type = node->expression.call->parentNames[0];
        //     variableValue[variable->name] = &expression;
        //     variableType[variable->name] = type;
        // }
    }

    void generateLoopNode(LoopNode* node) {
        llvm::Function* currentFunction = Builder.GetInsertBlock()->getParent();
        llvm::BasicBlock* loopCondBB = llvm::BasicBlock::Create(TheContext, "loopCond", currentFunction);
        llvm::BasicBlock* loopBodyBB = llvm::BasicBlock::Create(TheContext, "loopBody");
        llvm::BasicBlock* afterLoopBB = llvm::BasicBlock::Create(TheContext, "afterLoop");

        Builder.CreateBr(loopCondBB);

        Builder.SetInsertPoint(loopCondBB);
        llvm::Value* condition = evaluateExpression(&node->expression); // Evaluate the loop condition
        Builder.CreateCondBr(condition, loopBodyBB, afterLoopBB);

        currentFunction->getBasicBlockList().push_back(loopBodyBB);
        Builder.SetInsertPoint(loopBodyBB);
        for (Node* bodyNode : node->bodyNodes) {
            std::string type = bodyNode->nodeType;
            if (type == "LoopNode") {
                LoopNode* node = (LoopNode*) bodyNode;
                generateLoopNode(node);
            } else if (type == "ConditionalNode") {
                ConditionalNode* node = (ConditionalNode*) bodyNode;
                generateConditionalNode(node);
            } else {
                // handle other nodes
            }
        }
        // Jump back to the condition check
        Builder.CreateBr(loopCondBB);
        currentFunction->getBasicBlockList().push_back(afterLoopBB);
        Builder.SetInsertPoint(afterLoopBB);
    }

    void generateConditionalNode(ConditionalNode* node) {
        // Create basic blocks for the 'if' part, 'else' part, and continuation after the 'if-else'
        llvm::Function* currentFunction = Builder.GetInsertBlock()->getParent();
        llvm::BasicBlock* ifBB = llvm::BasicBlock::Create(TheContext, "if", currentFunction);
        llvm::BasicBlock* elseBB = llvm::BasicBlock::Create(TheContext, "else");
        llvm::BasicBlock* continueBB = llvm::BasicBlock::Create(TheContext, "ifcont");

        // Generate the condition and create a conditional branch
        llvm::Value* condition = evaluateExpression(&node->expression); // Evaluate the conditional expression
        Builder.CreateCondBr(condition, ifBB, elseBB);

        // Generate 'if' body
        Builder.SetInsertPoint(ifBB);
        // for (Node* bodyNode : node->ifBodyNodes) {
        //     generate(bodyNode); // Generate IR for each node in the 'if' body
        // }
        Builder.CreateBr(continueBB); // Jump to the continuation after the 'if' body

        // Generate 'else' body
        currentFunction->getBasicBlockList().push_back(elseBB);
        Builder.SetInsertPoint(elseBB);
        // for (Node* bodyNode : node->elseBodyNodes) {
        //     generate(bodyNode); // Generate IR for each node in the 'else' body
        // }
        Builder.CreateBr(continueBB); // Jump to the continuation after the 'else' body

        // Insert the continuation block
        currentFunction->getBasicBlockList().push_back(continueBB);
        Builder.SetInsertPoint(continueBB);

        // Continue with the rest of the code after the 'if-else'
    }


    llvm::Value* evaluateVariable(ExpressionNode* node, std::string type) {
        if (type == "Integer") {
            CallNode* call = (CallNode*) node->call;
            node = &call->arguments[0][0];
            // Builder.SetInsertPoint(entryBlock);
            int val = std::stoi(node->integerValue);
            std::cout << "found value: " << val << std::endl;
            llvm::Type* integerStructType = llvm::StructType::getTypeByName(TheContext, "Integer");
            if (!integerStructType) {
                std::cerr << "Integer struct type not found in context." << std::endl;
                return nullptr;
            }
            llvm::Value* intObject = Builder.CreateAlloca(integerStructType);
            llvm::Value* valPtr = Builder.CreateStructGEP(integerStructType, intObject, 0, "valPtr");
            llvm::Value* llvmVal = llvm::ConstantInt::get(TheContext, llvm::APInt(32, val, true));
            Builder.CreateStore(llvmVal, valPtr);
            return intObject;
        } else {
            return nullptr;
        }
    }

    llvm::Value* evaluateExpression(ExpressionNode* node) {
        if (node->expressionType == "call") {
            CallNode* call = (CallNode*) node->call;
            llvm::Value* objectInstance = nullptr;
            int i = 0;
            if (variables.count(call->parentName)) {
                int callsCount = call->parentNames.size();
                objectInstance = evaluateVariable(variableValue[call->parentName],
                                                  variableType[call->parentName]);
            } else if (!call->parentNames.empty() == 1 && variables.count(call->parentNames[0])) {
                objectInstance = evaluateVariable(variableValue[call->parentNames[0]],
                                                  variableType[call->parentNames[0]]);
                ++i;
            }
            int callsCount = int(call->parentNames.size());
            for (; i < callsCount; i++) {
                llvm::Function* method = TheModule->getFunction(call->parentNames[i]);
                if (!method) {
                    std::cerr << "couldn't find method: " << call->parentNames[i] << std::endl;
                    return nullptr;
                }

                std::vector<llvm::Value*> args;
                if (objectInstance != nullptr)
                    args.push_back(objectInstance);

                for (auto argument : call->arguments[i]) {
                    llvm::Value* argValue = nullptr;
                    argValue = evaluateExpression(&argument);
                    if (!argValue) {
                        std::cerr << "Invalid argument for method " << call->parentNames[i] << std::endl;
                        return nullptr;
                    }
                    args.push_back(argValue);
                }

                objectInstance = Builder.CreateCall(method, args);
            }

            return objectInstance;
        } else if (!node->integerValue.empty()) {
            int val = std::stoi(node->integerValue);
            std::cout << "found value: " << val << std::endl;
            llvm::Type* integerStructType = llvm::StructType::getTypeByName(TheContext, "Integer");
            if (!integerStructType) {
                std::cerr << "Integer struct type not found in context." << std::endl;
                return nullptr;
            }
            llvm::Value* intObject = Builder.CreateAlloca(integerStructType);
            llvm::Value* valPtr = Builder.CreateStructGEP(integerStructType, intObject, 0, "valPtr");
            llvm::Value* llvmVal = llvm::ConstantInt::get(TheContext, llvm::APInt(32, val, true));
            Builder.CreateStore(llvmVal, valPtr);
            return intObject;
        } else {
            // handle the type of literals
            return nullptr;
        }
    }

    void generateMethod(MethodNode * node) {
        std::vector<Type*> paramTypes;
        for (const auto& paramType : node->parameterTypes) {
            Type* llvmType = getType(paramType);
            paramTypes.push_back(llvmType);
        }

        Type* returnType = getType(node->returnType);
        FunctionType* funcType = FunctionType::get(returnType, paramTypes, false);
        Function* func = Function::Create(funcType, Function::ExternalLinkage, node->name, TheModule.get());

        auto argIt = func->arg_begin();
        for (const auto& paramName : node->parameterNames) {
            argIt->setName(paramName);
            ++argIt;
        }

        BasicBlock* entryBlock = BasicBlock::Create(TheContext, "entry", func);
        Builder.SetInsertPoint(entryBlock);

        llvm::Value* returnValue = nullptr;  // Initialize returnValue to nullptr

        for (Node* bodyNode : node->bodyNodes) {
            std::string type = bodyNode->nodeType;
            if (type == "ReturnStatementNode") {
                ReturnStatementNode* node = (ReturnStatementNode*) bodyNode;
                returnValue = evaluateExpression(&node->expression);
            } else if (type == "LoopNode") {
                LoopNode* node = (LoopNode*) bodyNode;
                generateLoopNode(node);
            } else if (type == "ConditionalNode") {
                ConditionalNode* node = (ConditionalNode*) bodyNode;
                generateConditionalNode(node);
            } else {
                // handle other nodes
            }
        }

        // Generate a return instruction (simplified)
        if (returnType->isVoidTy()) {
            Builder.CreateRetVoid();
        } else {
            // if (!returnValue) {
            //     returnValue = get_default_value(returnType);  // You need to implement this function
            // }
            Builder.CreateRet(returnValue);
        }
    }

    void generateClass(ClassNode * node) {
        // TODO: populate method signatures
        for (VariableNode* variable : node->variables) {
            std::string type = variable->expression.call->parentNames[0];
            if (type == "Integer") {
                variables[variable->name] = true;
                variableValue[variable->name] = &variable->expression;
                variableType[variable->name] = type;
            } else {
                // handle other types
            }
        }

        for (auto methodNode : node->methods) {
            generateMethod(methodNode);
        }
    }

    void generate(RootNode * node) {
        generateIntegerStruct();
        for (auto classNode : node->classNodes) {
            generateClass(classNode);
        }

        llvm::legacy::PassManager pass;
        auto FileType = CGFT_ObjectFile;
        targetMachine->addPassesToEmitFile(pass, output, nullptr, FileType);
        std::cout << "Generating Object Code\n";
        std::cout << "Printing IR\n";
        TheModule->print(llvm::errs(), nullptr);
    }
};

int main(int argc, char** argv) {
    std::string file = argv[1];

    file = "tests/" + file;
    std::ifstream fileStream(file);

    if (!fileStream) {
        std::cerr << "Error opening file: " << file << std::endl;
    }

    std::stringstream buffer;
    buffer << fileStream.rdbuf();
    std::string source = buffer.str();

    fileStream.close();
    Manager manager;
    std::vector<Token> tokens = manager.analyze(source);

    Parser parser;

    RootNode* node = parser.startParsing(tokens);

    Analyzer analyzer;
    analyzer.analyze(node, tokens);

    std::string outFile = "out";
    Generator generator(outFile);
    generator.generate(node);

    return EXIT_SUCCESS;
}