void generateMainMethod() {
    llvm::FunctionType* printfType = llvm::FunctionType::get(
                                         llvm::IntegerType::getInt32Ty(TheContext),
                                         llvm::PointerType::get(llvm::Type::getInt8Ty(TheContext), 0),
                                         true /* This is a vararg function */
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

    llvm::StructType* realStructType = llvm::StructType::getTypeByName(TheContext, "Real");

    llvm::Value* argA = Builder.CreateAlloca(realStructType, nullptr, "a");
    llvm::Value* argAValAddr = Builder.CreateStructGEP(realStructType, argA, 0);
    Builder.CreateStore(llvm::ConstantFP::get(TheContext, llvm::APFloat(3.0f)), argAValAddr);

    llvm::Type* integerStructType = getType("Integer");

    llvm::Value* argB = Builder.CreateAlloca(integerStructType, nullptr, "b");
    llvm::Value* argBValAddr = Builder.CreateStructGEP(integerStructType, argB, 0);
    Builder.CreateStore(llvm::ConstantInt::get(TheContext, llvm::APInt(32, 3)), argBValAddr);

    llvm::Function* returnRealFunction = TheModule->getFunction("BinaryExponentiation");
    if (!returnRealFunction) {
        llvm::errs() << "BinaryExponentiation function not found\n";
        Builder.CreateRet(llvm::ConstantInt::get(TheContext, llvm::APInt(32, 0)));
        return;
    }

    std::vector<llvm::Value*> args = {argA, argB};
    llvm::Value* returnRealResultObject = Builder.CreateCall(returnRealFunction, args, "returnRealResultObject");

    llvm::Value* returnRealResultValueAddr = Builder.CreateStructGEP(realStructType, returnRealResultObject, 0, "returnRealResultValueAddr");
    llvm::Value* returnRealResultValue = Builder.CreateLoad(llvm::Type::getFloatTy(TheContext), returnRealResultValueAddr, "returnRealResultValue");

    llvm::Value* formatStr = Builder.CreateGlobalStringPtr("%f\n", "formatStr");
    llvm::Value* doubleVal = Builder.CreateFPExt(returnRealResultValue, llvm::Type::getDoubleTy(TheContext)); // Extend float to double
    std::vector<llvm::Value*> printfArgs = {formatStr, doubleVal};
    Builder.CreateCall(printfFunc, printfArgs);

    Builder.CreateRet(llvm::ConstantInt::get(TheContext, llvm::APInt(32, 0)));
}