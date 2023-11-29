; ModuleID = 'hydrogen'
source_filename = "hydrogen"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-linux-gnu"

%Integer = type { i32 }

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

define i1 @IntegerEqual(%Integer* %this, %Integer* %other) {
entry:
  %thisValAddr = getelementptr inbounds %Integer, %Integer* %this, i32 0, i32 0
  %thisVal = load i32, i32* %thisValAddr, align 4
  %otherValAddr = getelementptr inbounds %Integer, %Integer* %other, i32 0, i32 0
  %otherVal = load i32, i32* %otherValAddr, align 4
  %cmpResult = icmp eq i32 %thisVal, %otherVal
  ret i1 %cmpResult
}

define i1 @IntegerEqual.4(%Integer* %this, i32 %constant) {
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

define %Integer* @Mult.5(%Integer* %this, i32 %a) {
entry:
  %valAddr = getelementptr inbounds %Integer, %Integer* %this, i32 0, i32 0
  %val = load i32, i32* %valAddr, align 4
  %result = mul i32 %val, %a
  %0 = alloca %Integer, align 8
  %1 = getelementptr inbounds %Integer, %Integer* %0, i32 0, i32 0
  store i32 %result, i32* %1, align 4
  ret %Integer* %0
}

define %Integer* @eval(%Integer* %a, %Integer* %b, %Integer* %c) {
entry:
  ret %Integer* %c
}

define %Integer* @solve(%Integer* %a, %Integer* %b, %Integer* %c) {
entry:
  %0 = alloca %Integer, align 8
  %valPtr = getelementptr inbounds %Integer, %Integer* %0, i32 0, i32 0
  store i32 1, i32* %valPtr, align 4
  %1 = call %Integer* @Minus(%Integer* %a, %Integer* %0)
  %2 = call %Integer* @eval(%Integer* %1, %Integer* %b, %Integer* %c)
  ret %Integer* %2
}

declare i32 @printf(i8*, ...)

define i32 @main() {
entry:
  %a = alloca %Integer, align 8
  %0 = getelementptr inbounds %Integer, %Integer* %a, i32 0, i32 0
  store i32 2, i32* %0, align 4
  %b = alloca %Integer, align 8
  %1 = getelementptr inbounds %Integer, %Integer* %b, i32 0, i32 0
  store i32 3, i32* %1, align 4
  %c = alloca %Integer, align 8
  %2 = getelementptr inbounds %Integer, %Integer* %c, i32 0, i32 0
  store i32 4, i32* %2, align 4
  %solveResultObject = call %Integer* @solve(%Integer* %a, %Integer* %b, %Integer* %c)
  %solveResultValueAddr = getelementptr inbounds %Integer, %Integer* %solveResultObject, i32 0, i32 0
  %solveResultValue = load i32, i32* %solveResultValueAddr, align 4
  %3 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([4 x i8], [4 x i8]* @formatStr, i32 0, i32 0), i32 %solveResultValue)
  ret i32 0
}
