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

    std::unordered_map<std::string, llvm::Value*> variables;
    std::unordered_map<std::string, llvm::Value*> variableValue;
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
        generateIntegerGreaterObject(integerStructType);
        generateIntegerGreaterConstant(integerStructType);
        generateIntegerEqualObject(integerStructType);
        generateIntegerEqualConstant(integerStructType);
        generateMultObject(integerStructType);
        generateMultMethod(integerStructType);
    }

    void generatePlusObject(llvm::StructType* integerStructType) {
        llvm::FunctionType* plusFunctionType = llvm::FunctionType::get(
                integerStructType->getPointerTo(),
        {integerStructType->getPointerTo(), integerStructType->getPointerTo()},
        false
                                               );
        llvm::Function* plusFunction = llvm::Function::Create(
                                           plusFunctionType, llvm::Function::ExternalLinkage, "Plus", TheModule.get()
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

        llvm::Value* resultObject = Builder.CreateAlloca(integerStructType);
        llvm::Value* resultValAddr = Builder.CreateStructGEP(integerStructType, resultObject, 0);
        Builder.CreateStore(result, resultValAddr);

        Builder.CreateRet(resultObject);
    }


    void generateMinusObject(llvm::StructType* integerStructType) {
        llvm::FunctionType* minusFunctionType = llvm::FunctionType::get(
                integerStructType->getPointerTo(),
        {integerStructType->getPointerTo(), integerStructType->getPointerTo()},
        false
                                                );
        llvm::Function* minusFunction = llvm::Function::Create(
                                            minusFunctionType, llvm::Function::ExternalLinkage, "Minus", TheModule.get()
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

        llvm::Value* resultObject = Builder.CreateAlloca(integerStructType);
        llvm::Value* resultValAddr = Builder.CreateStructGEP(integerStructType, resultObject, 0);
        Builder.CreateStore(result, resultValAddr);

        Builder.CreateRet(resultObject);
    }

    void generatePlusMethod(llvm::StructType* integerStructType) {
        llvm::FunctionType* plusFunctionType = llvm::FunctionType::get(
                integerStructType->getPointerTo(),
        {integerStructType->getPointerTo(), llvm::Type::getInt32Ty(TheContext)},
        false
                                               );
        llvm::Function* plusFunction = llvm::Function::Create(
                                           plusFunctionType, llvm::Function::ExternalLinkage, "Plus", TheModule.get()
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

        llvm::Value* resultObject = Builder.CreateAlloca(integerStructType);
        llvm::Value* resultValAddr = Builder.CreateStructGEP(integerStructType, resultObject, 0);
        Builder.CreateStore(result, resultValAddr);

        Builder.CreateRet(resultObject);
    }

    void generateMultObject(llvm::StructType* integerStructType) {
        llvm::FunctionType* multFunctionType = llvm::FunctionType::get(
                integerStructType->getPointerTo(),
        {integerStructType->getPointerTo(), integerStructType->getPointerTo()},
        false
                                               );
        llvm::Function* multFunction = llvm::Function::Create(
                                           multFunctionType, llvm::Function::ExternalLinkage, "Mult", TheModule.get()
                                       );
        llvm::BasicBlock* entryBlock = llvm::BasicBlock::Create(TheContext, "entry", multFunction);
        llvm::IRBuilder<> Builder(entryBlock);

        llvm::Argument* thisParam = multFunction->arg_begin();
        thisParam->setName("this");
        llvm::Argument* othParam = std::next(multFunction->arg_begin());
        othParam->setName("oth");

        llvm::Value* thisValAddr = Builder.CreateStructGEP(integerStructType, thisParam, 0, "thisValAddr");
        llvm::Value* thisVal = Builder.CreateLoad(llvm::Type::getInt32Ty(TheContext), thisValAddr, "thisVal");
        llvm::Value* othValAddr = Builder.CreateStructGEP(integerStructType, othParam, 0, "othValAddr");
        llvm::Value* othVal = Builder.CreateLoad(llvm::Type::getInt32Ty(TheContext), othValAddr, "othVal");

        llvm::Value* result = Builder.CreateMul(thisVal, othVal, "result");

        llvm::Value* resultObject = Builder.CreateAlloca(integerStructType);
        llvm::Value* resultValAddr = Builder.CreateStructGEP(integerStructType, resultObject, 0);
        Builder.CreateStore(result, resultValAddr);

        Builder.CreateRet(resultObject);
    }

    void generateMultMethod(llvm::StructType* integerStructType) {
        llvm::FunctionType* multFunctionType = llvm::FunctionType::get(
                integerStructType->getPointerTo(),
        {integerStructType->getPointerTo(), llvm::Type::getInt32Ty(TheContext)},
        false
                                               );
        llvm::Function* multFunction = llvm::Function::Create(
                                           multFunctionType, llvm::Function::ExternalLinkage, "Mult", TheModule.get()
                                       );
        llvm::BasicBlock* entryBlock = llvm::BasicBlock::Create(TheContext, "entry", multFunction);
        llvm::IRBuilder<> Builder(entryBlock);

        llvm::Argument* thisParam = multFunction->arg_begin();
        thisParam->setName("this");
        llvm::Argument* aParam = std::next(multFunction->arg_begin());
        aParam->setName("a");

        llvm::Value* valAddr = Builder.CreateStructGEP(integerStructType, thisParam, 0, "valAddr");
        llvm::Value* val = Builder.CreateLoad(llvm::Type::getInt32Ty(TheContext), valAddr, "val");

        llvm::Value* result = Builder.CreateMul(val, aParam, "result");

        llvm::Value* resultObject = Builder.CreateAlloca(integerStructType);
        llvm::Value* resultValAddr = Builder.CreateStructGEP(integerStructType, resultObject, 0);
        Builder.CreateStore(result, resultValAddr);

        Builder.CreateRet(resultObject);
    }

    void generateMinusMethod(llvm::StructType* integerStructType) {
        llvm::FunctionType* minusFunctionType = llvm::FunctionType::get(
                integerStructType->getPointerTo(),
        {integerStructType->getPointerTo(), llvm::Type::getInt32Ty(TheContext)},
        false
                                                );
        llvm::Function* minusFunction = llvm::Function::Create(
                                            minusFunctionType, llvm::Function::ExternalLinkage, "Minus", TheModule.get()
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

        llvm::Value* resultObject = Builder.CreateAlloca(integerStructType);
        llvm::Value* resultValAddr = Builder.CreateStructGEP(integerStructType, resultObject, 0);
        Builder.CreateStore(result, resultValAddr);

        Builder.CreateRet(resultObject);
    }

    void generateIntegerLessObject(llvm::StructType* integerStructType) {
        llvm::FunctionType* lessFunctionType = llvm::FunctionType::get(
                llvm::Type::getInt1Ty(TheContext),
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
        Builder.CreateRet(cmpResult);
    }

    void generateIntegerLessConstant(llvm::StructType* integerStructType) {
        // Define the function type for the LessConstant method
        llvm::FunctionType* lessConstFunctionType = llvm::FunctionType::get(
                    llvm::Type::getInt1Ty(TheContext),
        {integerStructType->getPointerTo(), llvm::Type::getInt32Ty(TheContext)},
        false
                );
        llvm::Function* lessConstFunction = llvm::Function::Create(
                                                lessConstFunctionType,
                                                llvm::Function::ExternalLinkage,
                                                "Less",
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
        Builder.CreateRet(cmpResult);
    }

    void generateIntegerGreaterObject(llvm::StructType* integerStructType) {
        llvm::FunctionType* greaterFunctionType = llvm::FunctionType::get(
                    llvm::Type::getInt1Ty(TheContext),
        {integerStructType->getPointerTo(), integerStructType->getPointerTo()},
        false
                );

        llvm::Function* greaterFunction = llvm::Function::Create(
                                              greaterFunctionType,
                                              llvm::Function::ExternalLinkage,
                                              "Greater",
                                              TheModule.get()
                                          );

        llvm::BasicBlock* entryBlock = llvm::BasicBlock::Create(TheContext, "entry", greaterFunction);
        llvm::IRBuilder<> Builder(entryBlock);
        llvm::Argument* thisParam = greaterFunction->arg_begin();
        thisParam->setName("this");
        llvm::Argument* othParam = std::next(greaterFunction->arg_begin());
        othParam->setName("oth");

        llvm::Value* thisValAddr = Builder.CreateStructGEP(integerStructType, thisParam, 0, "thisValAddr");
        llvm::Value* thisVal = Builder.CreateLoad(llvm::Type::getInt32Ty(TheContext), thisValAddr, "thisVal");
        llvm::Value* othValAddr = Builder.CreateStructGEP(integerStructType, othParam, 0, "othValAddr");
        llvm::Value* othVal = Builder.CreateLoad(llvm::Type::getInt32Ty(TheContext), othValAddr, "othVal");

        llvm::Value* cmpResult = Builder.CreateICmpSGT(thisVal, othVal, "cmpResult");
        Builder.CreateRet(cmpResult);
    }

    void generateIntegerGreaterConstant(llvm::StructType* integerStructType) {
        // Define the function type for the GreaterConstant method
        llvm::FunctionType* greaterConstFunctionType = llvm::FunctionType::get(
                    llvm::Type::getInt1Ty(TheContext),
        {integerStructType->getPointerTo(), llvm::Type::getInt32Ty(TheContext)},
        false
                );
        llvm::Function* greaterConstFunction = llvm::Function::Create(
                greaterConstFunctionType,
                llvm::Function::ExternalLinkage,
                "Greater",
                TheModule.get()
                                               );
        llvm::BasicBlock* entryBlock = llvm::BasicBlock::Create(TheContext, "entry", greaterConstFunction);
        llvm::IRBuilder<> Builder(entryBlock);
        llvm::Argument* thisParam = greaterConstFunction->arg_begin();
        thisParam->setName("this");
        llvm::Argument* constParam = std::next(greaterConstFunction->arg_begin());
        constParam->setName("constant");
        llvm::Value* thisValAddr = Builder.CreateStructGEP(integerStructType, thisParam, 0, "thisValAddr");
        llvm::Value* thisVal = Builder.CreateLoad(llvm::Type::getInt32Ty(TheContext), thisValAddr, "thisVal");
        llvm::Value* cmpResult = Builder.CreateICmpSGT(thisVal, constParam, "cmpResult");
        Builder.CreateRet(cmpResult);
    }


    void generateIntegerEqualObject(llvm::StructType* integerStructType) {
        llvm::FunctionType* equalObjectFunctionType = llvm::FunctionType::get(
                    llvm::Type::getInt1Ty(TheContext),
        {integerStructType->getPointerTo(), integerStructType->getPointerTo()},
        false
                );
        llvm::Function* equalObjectFunction = llvm::Function::Create(
                equalObjectFunctionType,
                llvm::Function::ExternalLinkage,
                "IntegerEqual",
                TheModule.get()
                                              );
        llvm::BasicBlock* entryBlock = llvm::BasicBlock::Create(TheContext, "entry", equalObjectFunction);
        llvm::IRBuilder<> Builder(entryBlock);
        llvm::Argument* thisParam = equalObjectFunction->arg_begin();
        thisParam->setName("this");
        llvm::Argument* otherParam = std::next(equalObjectFunction->arg_begin());
        otherParam->setName("other");
        llvm::Value* thisValAddr = Builder.CreateStructGEP(integerStructType, thisParam, 0, "thisValAddr");
        llvm::Value* thisVal = Builder.CreateLoad(llvm::Type::getInt32Ty(TheContext), thisValAddr, "thisVal");
        llvm::Value* otherValAddr = Builder.CreateStructGEP(integerStructType, otherParam, 0, "otherValAddr");
        llvm::Value* otherVal = Builder.CreateLoad(llvm::Type::getInt32Ty(TheContext), otherValAddr, "otherVal");
        llvm::Value* cmpResult = Builder.CreateICmpEQ(thisVal, otherVal, "cmpResult");
        Builder.CreateRet(cmpResult);
    }

    void generateIntegerEqualConstant(llvm::StructType* integerStructType) {
        llvm::FunctionType* equalConstFunctionType = llvm::FunctionType::get(
                    llvm::Type::getInt1Ty(TheContext),
        {integerStructType->getPointerTo(), llvm::Type::getInt32Ty(TheContext)},
        false
                );
        llvm::Function* equalConstFunction = llvm::Function::Create(
                equalConstFunctionType,
                llvm::Function::ExternalLinkage,
                "IntegerEqual",
                TheModule.get()
                                             );
        llvm::BasicBlock* entryBlock = llvm::BasicBlock::Create(TheContext, "entry", equalConstFunction);
        llvm::IRBuilder<> Builder(entryBlock);
        llvm::Argument* thisParam = equalConstFunction->arg_begin();
        thisParam->setName("this");
        llvm::Argument* constParam = std::next(equalConstFunction->arg_begin());
        constParam->setName("constant");
        llvm::Value* thisValAddr = Builder.CreateStructGEP(integerStructType, thisParam, 0, "thisValAddr");
        llvm::Value* thisVal = Builder.CreateLoad(llvm::Type::getInt32Ty(TheContext), thisValAddr, "thisVal");
        llvm::Value* cmpResult = Builder.CreateICmpEQ(thisVal, constParam, "cmpResult");
        Builder.CreateRet(cmpResult);
    }

    llvm::Type* getType(const std::string& type) {
        if (type == "Integer") {
            return llvm::StructType::getTypeByName(TheContext, "Integer");
        } else {
            // handle other types
            return nullptr;
        }
    }

    void evaluateAssignment(AssignmentNode* node) {
        // Check if the variable exists in the symbol table
        if (variables.count(node->name)) {
            llvm::Value* alloca = variables[node->name]; // Retrieve the existing variable's memory address

            // Evaluate the new value for the variable
            llvm::Value* newValuePtr = evaluateExpression(node->expression);

            // Check if the alloca and newValuePtr are valid
            if (!alloca || !newValuePtr) {
                std::cerr << "Error: variable or new value is null" << std::endl;
                return;
            }

            // Load the value from newValuePtr if it's a pointer
            llvm::Value* newValue = Builder.CreateLoad(newValuePtr->getType()->getPointerElementType(), newValuePtr);

            // Store the loaded value into the allocated memory
            Builder.CreateStore(newValue, alloca);
        } else {
            std::cerr << "Error: variable " << node->name << " not declared." << std::endl;
        }
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
            } else if (type == "AssignmentNode") {
                AssignmentNode* node = (AssignmentNode*) bodyNode;
                evaluateAssignment(node);
            } else {
                // handle other nodes
            }
        }
        // Jump back to the condition check
        Builder.CreateBr(loopCondBB);
        currentFunction->getBasicBlockList().push_back(afterLoopBB);
        Builder.SetInsertPoint(afterLoopBB);
    }

    void generateReturnStatementNode(ReturnStatementNode* node) {
        if (node->isVoid) {
            Builder.CreateRetVoid();
        } else {
            std::cout << "creating return value" << std::endl;
            llvm::Value* returnValue = evaluateExpression(&node->expression);
            if (returnValue != nullptr) {
                Builder.CreateRet(returnValue);
            } else {
                llvm::errs() << "Error: Return expression evaluation failed.\n";
            }
        }
    }

    void generateConditionalNode(ConditionalNode* node) {
        llvm::Function* currentFunction = Builder.GetInsertBlock()->getParent();
        llvm::BasicBlock* ifBB = llvm::BasicBlock::Create(TheContext, "if", currentFunction);
        llvm::BasicBlock* elseBB = llvm::BasicBlock::Create(TheContext, "else");
        llvm::BasicBlock* continueBB = llvm::BasicBlock::Create(TheContext, "ifcont");

        llvm::Value* condition = evaluateExpression(&node->expression); // Evaluate the conditional expression
        Builder.CreateCondBr(condition, ifBB, elseBB);

        Builder.SetInsertPoint(ifBB);
        bool ifReturns = false;
        for (Node* bodyNode : node->ifBodyNodes) {
            std::string type = bodyNode->nodeType;
            std::cout << "type: " << type << std::endl;
            if (type == "LoopNode") {
                LoopNode* node = (LoopNode*) bodyNode;
                generateLoopNode(node);
            } else if (type == "ConditionalNode") {
                ConditionalNode* node = (ConditionalNode*) bodyNode;
                generateConditionalNode(node);
            } else if (type == "ReturnStatementNode") {
                ReturnStatementNode* node = (ReturnStatementNode*) bodyNode;
                generateReturnStatementNode(node);
                ifReturns = true;
                break;
            } else if (type == "AssignmentNode") {
                AssignmentNode* node = (AssignmentNode*) bodyNode;
                evaluateAssignment(node);
            } else {
                // handle other nodes
            }
        }

        if (!Builder.GetInsertBlock()->getTerminator()) {
            Builder.CreateBr(continueBB);
        }

        currentFunction->getBasicBlockList().push_back(elseBB);
        Builder.SetInsertPoint(elseBB);

        bool elseReturns = false;

        for (Node* bodyNode : node->elseBodyNodes) {
            std::string type = bodyNode->nodeType;
            if (type == "LoopNode") {
                LoopNode* node = (LoopNode*) bodyNode;
                generateLoopNode(node);
            } else if (type == "ConditionalNode") {
                ConditionalNode* node = (ConditionalNode*) bodyNode;
                generateConditionalNode(node);
            } else if (type == "ReturnStatementNode") {
                ReturnStatementNode* node = (ReturnStatementNode*) bodyNode;
                generateReturnStatementNode(node);
                elseReturns = true;
                break;
            } else {
                // handle other nodes
            }
        }

        if (!Builder.GetInsertBlock()->getTerminator()) {
            Builder.CreateBr(continueBB);
        }

        // Add 'continue' block only if needed
        if (!continueBB->getTerminator()) {
            currentFunction->getBasicBlockList().push_back(continueBB);
            Builder.SetInsertPoint(continueBB);
        } else {
            delete continueBB; // Remove unneeded 'continue' block
        }
    }

    llvm::Value* evaluateVariable(ExpressionNode* node, std::string type) {
        if (type == "Integer") {
            CallNode* call = (CallNode*) node->call;
            node = &call->arguments[0][0];
            int val = std::stoi(node->integerValue);
            std::cerr << "found value: " << val << std::endl;
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
                objectInstance = variableValue[call->parentName];
            } else if (!call->parentNames.empty() == 1 && variables.count(call->parentNames[0])) {
                objectInstance = variableValue[call->parentNames[0]];
                ++i;
            }
            int callsCount = int(call->parentNames.size());
            for (; i < callsCount; i++) {
                std::string methodString = call->parentNames[i];
                if (methodString == "Equal")
                    methodString = "IntegerEqual";
                llvm::Function* method = TheModule->getFunction(methodString);
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
                std::string str;
                llvm::raw_string_ostream rso(str);
                objectInstance->print(rso);

                // Now 'str' contains the string representation of 'value'
                std::cout << "object instance val: " << str << std::endl;
            }

            return objectInstance;
        } else if (!node->integerValue.empty()) {
            int val = std::stoi(node->integerValue);
            std::cerr << "found value: " << val << std::endl;
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

    void generateVariableNode(VariableNode* node) {
        //         std::string name;
        // ExpressionNode expression;

    }

    void generateMethod(MethodNode * node) {
        std::vector<Type*> paramTypes;
        for (const auto& paramType : node->parameterTypes) {
            Type* llvmType = getType(paramType)->getPointerTo();
            paramTypes.push_back(llvmType);
        }

        Type* returnType = getType(node->returnType)->getPointerTo();
        FunctionType* funcType = FunctionType::get(returnType, paramTypes, false);
        Function* func = Function::Create(funcType, Function::ExternalLinkage, node->name, TheModule.get());
        auto argIt = func->arg_begin();

        BasicBlock* entryBlock = BasicBlock::Create(TheContext, "entry", func);
        Builder.SetInsertPoint(entryBlock);

        int idx = 0;
        for (const auto& paramName : node->parameterNames) {
            argIt->setName(paramName);
            variables[paramName] = argIt;
            std::cout << "variable paramName: " << paramName << " has value: " << std::endl;
            variableValue[paramName] = argIt;
            ++argIt;
            ++idx;
        }

        for (Node* bodyNode : node->bodyNodes) {
            std::string type = bodyNode->nodeType;
            if (type == "ReturnStatementNode") {
                ReturnStatementNode* node = (ReturnStatementNode*) bodyNode;
                generateReturnStatementNode(node);
            } else if (type == "LoopNode") {
                LoopNode* node = (LoopNode*) bodyNode;
                generateLoopNode(node);
            } else if (type == "ConditionalNode") {
                ConditionalNode* node = (ConditionalNode*) bodyNode;
                generateConditionalNode(node);
            } else if (type == "AssignmentNode") {
                std::cerr << "found assignment" << std::endl;
                AssignmentNode* node = (AssignmentNode*) bodyNode;
                evaluateAssignment(node);
            } else if (type == "VariableNode") {
                VariableNode* variableNode = (VariableNode*) bodyNode;
                std::string varTypeStr = variableNode->expression.call->parentNames[0];
                llvm::Type* varType = getType(varTypeStr); // Convert high-level type to LLVM type
                std::cout << "var: " << variableNode->name << std::endl;
                std::cout << "type: " << varTypeStr << std::endl;
                // Allocate memory for the variable
                llvm::AllocaInst* alloca = Builder.CreateAlloca(varType, 0, variableNode->name.c_str());
                variables[variableNode->name] = alloca; // Store the variable's address in the symbol table
                // If the variable is initialized, evaluate the expression and store the result
                llvm::Value* initVal = evaluateVariable(&variableNode->expression, varTypeStr);
                // Assuming alloca is the memory allocated for %d and %0
                llvm::Value* initValLoad = Builder.CreateLoad(initVal->getType()->getPointerElementType(), initVal);
                variableValue[variableNode->name] = alloca;
                Builder.CreateStore(initValLoad, alloca);
            } else {
                // handle other nodes
            }
        }

        // get the actual object passed to the method
    }

    void generateMainMethod() {
        llvm::FunctionType* printfType = llvm::FunctionType::get(
                                             llvm::IntegerType::getInt32Ty(TheContext),
                                             llvm::PointerType::get(llvm::Type::getInt8Ty(TheContext), 0),
                                             true
                                         );
        llvm::FunctionCallee printfFunc = TheModule->getOrInsertFunction("printf", printfType);

        llvm::FunctionType* mainFunctionType = llvm::FunctionType::get(
                llvm::Type::getInt32Ty(TheContext), false
                                               );
        llvm::Function* mainFunction = llvm::Function::Create(
                                           mainFunctionType, llvm::Function::ExternalLinkage, "main", TheModule.get()
                                       );
        llvm::BasicBlock* entryBlock = llvm::BasicBlock::Create(TheContext, "entry", mainFunction);
        Builder.SetInsertPoint(entryBlock);

        llvm::Type* integerStructType = getType("Integer");

        // Prepare the arguments for 'solve': solve(2, 3, 4)
        // llvm::Value* argA = Builder.CreateAlloca(integerStructType, nullptr, "a");
        // llvm::Value* argAValAddr = Builder.CreateStructGEP(integerStructType, argA, 0);
        // Builder.CreateStore(llvm::ConstantInt::get(TheContext, llvm::APInt(32, 4)), argAValAddr);

        // llvm::Value* argB = Builder.CreateAlloca(integerStructType, nullptr, "b");
        // llvm::Value* argBValAddr = Builder.CreateStructGEP(integerStructType, argB, 0);
        // Builder.CreateStore(llvm::ConstantInt::get(TheContext, llvm::APInt(32, 3)), argBValAddr);

        llvm::Value* argC = Builder.CreateAlloca(integerStructType, nullptr, "n");
        llvm::Value* argCValAddr = Builder.CreateStructGEP(integerStructType, argC, 0);
        Builder.CreateStore(llvm::ConstantInt::get(TheContext, llvm::APInt(32, 10)), argCValAddr);

        // Call solve
        llvm::Function* solveFunction = TheModule->getFunction("solve");
        if (!solveFunction) {
            llvm::errs() << "Solve function not found\n";
            Builder.CreateRet(llvm::ConstantInt::get(TheContext, llvm::APInt(32, 0)));
            return;
        }

        std::vector<llvm::Value*> args;
        // args.push_back(argA);
        // args.push_back(argB);
        args.push_back(argC);

        llvm::Value* solveResultObject = Builder.CreateCall(solveFunction, args, "solveResultObject");

        // Extract the integer value from the returned object
        llvm::Value* solveResultValueAddr = Builder.CreateStructGEP(integerStructType, solveResultObject, 0, "solveResultValueAddr");
        llvm::Value* solveResultValue = Builder.CreateLoad(llvm::Type::getInt32Ty(TheContext), solveResultValueAddr, "solveResultValue");

        llvm::Value* formatStr = Builder.CreateGlobalStringPtr("%d\n", "formatStr");
        std::vector<llvm::Value*> printfArgs = {formatStr, solveResultValue};
        Builder.CreateCall(printfFunc, printfArgs);

        Builder.CreateRet(llvm::ConstantInt::get(TheContext, llvm::APInt(32, 0)));
    }

    void generateClass(ClassNode * node) {
        for (VariableNode* variableNode : node->variables) {
            std::string varTypeStr = variableNode->expression.call->parentNames[0];
            if (varTypeStr == "Integer") {
                llvm::Type* varType = getType(varTypeStr); // Convert high-level type to LLVM type
                std::cout << "var: " << variableNode->name << std::endl;
                std::cout << "type: " << varTypeStr << std::endl;
                // Allocate memory for the variable
                llvm::AllocaInst* alloca = Builder.CreateAlloca(varType, 0, variableNode->name.c_str());
                variables[variableNode->name] = alloca; // Store the variable's address in the symbol table
                // If the variable is initialized, evaluate the expression and store the result
                llvm::Value* initVal = evaluateVariable(&variableNode->expression, varTypeStr);
                variableValue[variableNode->name] = initVal;
                Builder.CreateStore(initVal, alloca);
                // variables[variable->name] = true;
                // variableValue[variable->name] = evaluateExpression(&variable->expression);
                // variableType[variable->name] = type;
            } else {
                // handle other types
            }
        }
        for (auto methodNode : node->methods) {
            generateMethod(methodNode);
        }
        generateMainMethod();
    }

    void generate(RootNode *node) {
        generateIntegerStruct();
        for (auto classNode : node->classNodes) {
            generateClass(classNode);
        }

        std::error_code EC;
        llvm::raw_fd_ostream fileStream("hydrogen.ll", EC);

        if (EC) {
            std::cerr << "Error opening file hydrogen.ll: " << EC.message() << '\n';
            return;
        }

        std::cerr << "Generating Object Code\n";
        llvm::legacy::PassManager pass;
        auto FileType = CGFT_ObjectFile;
        targetMachine->addPassesToEmitFile(pass, output, nullptr, FileType);

        std::cerr << "Printing IR to hydrogen.ll\n";
        TheModule->print(fileStream, nullptr);
        fileStream.flush();
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