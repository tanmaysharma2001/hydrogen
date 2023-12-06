; ModuleID = 'hydrogen'
source_filename = "hydrogen"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-linux-gnu"

%Integer = type { i32 }
%Real = type { float }

@formatStr = private unnamed_addr constant [4 x i8] c"%d\0A\00", align 1

define %Integer* @Plus(%Integer* %this, %Integer* %oth) {
entry:
  %thisValAddr = getelementptr inbounds %Integer, %Integer* %this, i32 0, i32 0
  %thisVal = load i32, i32* %thisValAddr, align 4
  %othValAddr = getelementptr inbounds %Integer, %Integer* %oth, i32 0, i32 0
  %othVal = load i32, i32* %othValAddr, align 4
  %result = add i32 %thisVal, %othVal
  %0 = alloca %Integer, align 8
  %1 = getelementptr inbounds %Integer, %Integer* %0, i32 0, i32 0
  store i32 %result, i32* %1, align 4
  ret %Integer* %0
}

define %Integer* @Minus(%Integer* %this, %Integer* %oth) {
entry:
  %thisValAddr = getelementptr inbounds %Integer, %Integer* %this, i32 0, i32 0
  %thisVal = load i32, i32* %thisValAddr, align 4
  %othValAddr = getelementptr inbounds %Integer, %Integer* %oth, i32 0, i32 0
  %othVal = load i32, i32* %othValAddr, align 4
  %result = sub i32 %thisVal, %othVal
  %0 = alloca %Integer, align 8
  %1 = getelementptr inbounds %Integer, %Integer* %0, i32 0, i32 0
  store i32 %result, i32* %1, align 4
  ret %Integer* %0
}

define %Integer* @Plus.1(%Integer* %this, i32 %a) {
entry:
  %valAddr = getelementptr inbounds %Integer, %Integer* %this, i32 0, i32 0
  %val = load i32, i32* %valAddr, align 4
  %result = add i32 %val, %a
  %0 = alloca %Integer, align 8
  %1 = getelementptr inbounds %Integer, %Integer* %0, i32 0, i32 0
  store i32 %result, i32* %1, align 4
  ret %Integer* %0
}

define %Integer* @Minus.2(%Integer* %this, i32 %b) {
entry:
  %valAddr = getelementptr inbounds %Integer, %Integer* %this, i32 0, i32 0
  %val = load i32, i32* %valAddr, align 4
  %result = sub i32 %val, %b
  %0 = alloca %Integer, align 8
  %1 = getelementptr inbounds %Integer, %Integer* %0, i32 0, i32 0
  store i32 %result, i32* %1, align 4
  ret %Integer* %0
}

define i1 @Less(%Integer* %this, %Integer* %oth) {
entry:
  %thisValAddr = getelementptr inbounds %Integer, %Integer* %this, i32 0, i32 0
  %thisVal = load i32, i32* %thisValAddr, align 4
  %othValAddr = getelementptr inbounds %Integer, %Integer* %oth, i32 0, i32 0
  %othVal = load i32, i32* %othValAddr, align 4
  %cmpResult = icmp slt i32 %thisVal, %othVal
  ret i1 %cmpResult
}

define i1 @Less.3(%Integer* %this, i32 %constant) {
entry:
  %thisValAddr = getelementptr inbounds %Integer, %Integer* %this, i32 0, i32 0
  %thisVal = load i32, i32* %thisValAddr, align 4
  %cmpResult = icmp slt i32 %thisVal, %constant
  ret i1 %cmpResult
}

define i1 @Greater(%Integer* %this, %Integer* %oth) {
entry:
  %thisValAddr = getelementptr inbounds %Integer, %Integer* %this, i32 0, i32 0
  %thisVal = load i32, i32* %thisValAddr, align 4
  %othValAddr = getelementptr inbounds %Integer, %Integer* %oth, i32 0, i32 0
  %othVal = load i32, i32* %othValAddr, align 4
  %cmpResult = icmp sgt i32 %thisVal, %othVal
  ret i1 %cmpResult
}

define i1 @Greater.4(%Integer* %this, i32 %constant) {
entry:
  %thisValAddr = getelementptr inbounds %Integer, %Integer* %this, i32 0, i32 0
  %thisVal = load i32, i32* %thisValAddr, align 4
  %cmpResult = icmp sgt i32 %thisVal, %constant
  ret i1 %cmpResult
}

define i1 @IntegerEqual(%Integer* %this, %Integer* %other) {
entry:
  %thisValAddr = getelementptr inbounds %Integer, %Integer* %this, i32 0, i32 0
  %thisVal = load i32, i32* %thisValAddr, align 4
  %otherValAddr = getelementptr inbounds %Integer, %Integer* %other, i32 0, i32 0
  %otherVal = load i32, i32* %otherValAddr, align 4
  %cmpResult = icmp eq i32 %thisVal, %otherVal
  ret i1 %cmpResult
}

define i1 @IntegerEqual.5(%Integer* %this, i32 %constant) {
entry:
  %thisValAddr = getelementptr inbounds %Integer, %Integer* %this, i32 0, i32 0
  %thisVal = load i32, i32* %thisValAddr, align 4
  %cmpResult = icmp eq i32 %thisVal, %constant
  ret i1 %cmpResult
}

define %Integer* @Mult(%Integer* %this, %Integer* %oth) {
entry:
  %thisValAddr = getelementptr inbounds %Integer, %Integer* %this, i32 0, i32 0
  %thisVal = load i32, i32* %thisValAddr, align 4
  %othValAddr = getelementptr inbounds %Integer, %Integer* %oth, i32 0, i32 0
  %othVal = load i32, i32* %othValAddr, align 4
  %result = mul i32 %thisVal, %othVal
  %0 = alloca %Integer, align 8
  %1 = getelementptr inbounds %Integer, %Integer* %0, i32 0, i32 0
  store i32 %result, i32* %1, align 4
  ret %Integer* %0
}

define %Integer* @Mult.6(%Integer* %this, i32 %a) {
entry:
  %valAddr = getelementptr inbounds %Integer, %Integer* %this, i32 0, i32 0
  %val = load i32, i32* %valAddr, align 4
  %result = mul i32 %val, %a
  %0 = alloca %Integer, align 8
  %1 = getelementptr inbounds %Integer, %Integer* %0, i32 0, i32 0
  store i32 %result, i32* %1, align 4
  ret %Integer* %0
}

define %Integer* @Div(%Integer* %this, %Integer* %oth) {
entry:
  %thisValAddr = getelementptr inbounds %Integer, %Integer* %this, i32 0, i32 0
  %thisVal = load i32, i32* %thisValAddr, align 4
  %othValAddr = getelementptr inbounds %Integer, %Integer* %oth, i32 0, i32 0
  %othVal = load i32, i32* %othValAddr, align 4
  %result = sdiv i32 %thisVal, %othVal
  %0 = alloca %Integer, align 8
  %1 = getelementptr inbounds %Integer, %Integer* %0, i32 0, i32 0
  store i32 %result, i32* %1, align 4
  ret %Integer* %0
}

define %Integer* @Rem(%Integer* %this, %Integer* %oth) {
entry:
  %thisValAddr = getelementptr inbounds %Integer, %Integer* %this, i32 0, i32 0
  %thisVal = load i32, i32* %thisValAddr, align 4
  %othValAddr = getelementptr inbounds %Integer, %Integer* %oth, i32 0, i32 0
  %othVal = load i32, i32* %othValAddr, align 4
  %result = srem i32 %thisVal, %othVal
  %0 = alloca %Integer, align 8
  %1 = getelementptr inbounds %Integer, %Integer* %0, i32 0, i32 0
  store i32 %result, i32* %1, align 4
  ret %Integer* %0
}

define %Real* @RealPlus(%Real* %this, %Real* %oth) {
entry:
  %thisValAddr = getelementptr inbounds %Real, %Real* %this, i32 0, i32 0
  %thisVal = load float, float* %thisValAddr, align 4
  %othValAddr = getelementptr inbounds %Real, %Real* %oth, i32 0, i32 0
  %othVal = load float, float* %othValAddr, align 4
  %result = fadd float %thisVal, %othVal
  %0 = alloca %Real, align 8
  %1 = getelementptr inbounds %Real, %Real* %0, i32 0, i32 0
  store float %result, float* %1, align 4
  ret %Real* %0
}

define %Real* @RealMinus(%Real* %this, %Real* %oth) {
entry:
  %thisValAddr = getelementptr inbounds %Real, %Real* %this, i32 0, i32 0
  %thisVal = load float, float* %thisValAddr, align 4
  %othValAddr = getelementptr inbounds %Real, %Real* %oth, i32 0, i32 0
  %othVal = load float, float* %othValAddr, align 4
  %result = fsub float %thisVal, %othVal
  %0 = alloca %Real, align 8
  %1 = getelementptr inbounds %Real, %Real* %0, i32 0, i32 0
  store float %result, float* %1, align 4
  ret %Real* %0
}

define %Real* @RealMult(%Real* %this, %Real* %oth) {
entry:
  %thisValAddr = getelementptr inbounds %Real, %Real* %this, i32 0, i32 0
  %thisVal = load float, float* %thisValAddr, align 4
  %othValAddr = getelementptr inbounds %Real, %Real* %oth, i32 0, i32 0
  %othVal = load float, float* %othValAddr, align 4
  %result = fmul float %thisVal, %othVal
  %0 = alloca %Real, align 8
  %1 = getelementptr inbounds %Real, %Real* %0, i32 0, i32 0
  store float %result, float* %1, align 4
  ret %Real* %0
}

define %Real* @RealDiv(%Real* %this, %Real* %oth) {
entry:
  %thisValAddr = getelementptr inbounds %Real, %Real* %this, i32 0, i32 0
  %thisVal = load float, float* %thisValAddr, align 4
  %othValAddr = getelementptr inbounds %Real, %Real* %oth, i32 0, i32 0
  %othVal = load float, float* %othValAddr, align 4
  %result = fdiv float %thisVal, %othVal
  %0 = alloca %Real, align 8
  %1 = getelementptr inbounds %Real, %Real* %0, i32 0, i32 0
  store float %result, float* %1, align 4
  ret %Real* %0
}

define %Integer* @solve(%Integer* %n) {
entry:
  %a = alloca %Integer, align 8
  %0 = alloca %Integer, align 8
  %valPtr = getelementptr inbounds %Integer, %Integer* %0, i32 0, i32 0
  store i32 0, i32* %valPtr, align 4
  %1 = load %Integer, %Integer* %0, align 4
  store %Integer %1, %Integer* %a, align 4
  %b = alloca %Integer, align 8
  %2 = alloca %Integer, align 8
  %valPtr1 = getelementptr inbounds %Integer, %Integer* %2, i32 0, i32 0
  store i32 1, i32* %valPtr1, align 4
  %3 = load %Integer, %Integer* %2, align 4
  store %Integer %3, %Integer* %b, align 4
  %tmp = alloca %Integer, align 8
  %4 = alloca %Integer, align 8
  %valPtr2 = getelementptr inbounds %Integer, %Integer* %4, i32 0, i32 0
  store i32 0, i32* %valPtr2, align 4
  %5 = load %Integer, %Integer* %4, align 4
  store %Integer %5, %Integer* %tmp, align 4
  %i = alloca %Integer, align 8
  %6 = alloca %Integer, align 8
  %valPtr3 = getelementptr inbounds %Integer, %Integer* %6, i32 0, i32 0
  store i32 0, i32* %valPtr3, align 4
  %7 = load %Integer, %Integer* %6, align 4
  store %Integer %7, %Integer* %i, align 4
  br label %loopCond

loopCond:                                         ; preds = %loopBody, %entry
  %8 = call i1 @Less(%Integer* %i, %Integer* %n)
  br i1 %8, label %loopBody, label %afterLoop

loopBody:                                         ; preds = %loopCond
  %9 = load %Integer, %Integer* %b, align 4
  store %Integer %9, %Integer* %tmp, align 4
  %10 = call %Integer* @Plus(%Integer* %a, %Integer* %b)
  %11 = load %Integer, %Integer* %10, align 4
  store %Integer %11, %Integer* %b, align 4
  %12 = load %Integer, %Integer* %tmp, align 4
  store %Integer %12, %Integer* %a, align 4
  %13 = alloca %Integer, align 8
  %valPtr4 = getelementptr inbounds %Integer, %Integer* %13, i32 0, i32 0
  store i32 1, i32* %valPtr4, align 4
  %14 = call %Integer* @Plus(%Integer* %i, %Integer* %13)
  %15 = load %Integer, %Integer* %14, align 4
  store %Integer %15, %Integer* %i, align 4
  br label %loopCond

afterLoop:                                        ; preds = %loopCond
  ret %Integer* %b
}

declare i32 @printf(i8*, ...)

define i32 @main() {
entry:
  %n = alloca %Integer, align 8
  %0 = getelementptr inbounds %Integer, %Integer* %n, i32 0, i32 0
  store i32 10, i32* %0, align 4
  %solveResultObject = call %Integer* @solve(%Integer* %n)
  %solveResultValueAddr = getelementptr inbounds %Integer, %Integer* %solveResultObject, i32 0, i32 0
  %solveResultValue = load i32, i32* %solveResultValueAddr, align 4
  %1 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([4 x i8], [4 x i8]* @formatStr, i32 0, i32 0), i32 %solveResultValue)
  ret i32 0
}
