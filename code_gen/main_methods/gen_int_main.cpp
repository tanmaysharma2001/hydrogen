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
    llvm::Value* argA = Builder.CreateAlloca(integerStructType, nullptr, "a");
    llvm::Value* argAValAddr = Builder.CreateStructGEP(integerStructType, argA, 0);
    Builder.CreateStore(llvm::ConstantInt::get(TheContext, llvm::APInt(32, 10)), argAValAddr);

    llvm::Value* argB = Builder.CreateAlloca(integerStructType, nullptr, "b");
    llvm::Value* argBValAddr = Builder.CreateStructGEP(integerStructType, argB, 0);
    Builder.CreateStore(llvm::ConstantInt::get(TheContext, llvm::APInt(32, 20)), argBValAddr);

    // llvm::Value* argC = Builder.CreateAlloca(integerStructType, nullptr, "n");
    // llvm::Value* argCValAddr = Builder.CreateStructGEP(integerStructType, argC, 0);
    // Builder.CreateStore(llvm::ConstantInt::get(TheContext, llvm::APInt(32, 10)), argCValAddr);

    // Call solve
    llvm::Function* solveFunction = TheModule->getFunction("returnReal");
    if (!solveFunction) {
        llvm::errs() << "Solve function not found\n";
        Builder.CreateRet(llvm::ConstantInt::get(TheContext, llvm::APInt(32, 0)));
        return;
    }

    std::vector<llvm::Value*> args;
    args.push_back(argA);
    args.push_back(argB);
    // args.push_back(argC);

    llvm::Value* solveResultObject = Builder.CreateCall(solveFunction, args, "solveResultObject");

    // Extract the integer value from the returned object
    llvm::Value* solveResultValueAddr = Builder.CreateStructGEP(integerStructType, solveResultObject, 0, "solveResultValueAddr");
    llvm::Value* solveResultValue = Builder.CreateLoad(llvm::Type::getInt32Ty(TheContext), solveResultValueAddr, "solveResultValue");

    llvm::Value* formatStr = Builder.CreateGlobalStringPtr("%d\n", "formatStr");
    std::vector<llvm::Value*> printfArgs = {formatStr, solveResultValue};
    Builder.CreateCall(printfFunc, printfArgs);

    Builder.CreateRet(llvm::ConstantInt::get(TheContext, llvm::APInt(32, 0)));
}