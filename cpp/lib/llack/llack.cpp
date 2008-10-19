/*
 * File: llack.c
 *
 * Description:
 *  This is a llack source file for a library.  It helps to demonstrate
 *  how to setup a project that uses the LLVM build system, header files,
 *  and libraries.
 */

#include <stdio.h>
#include <stdlib.h>

/* LLVM Header File
#include "llvm/Support/DataTypes.h"
*/

/* Header file global to this project */
#include "llack.h"

/*int
compute_llack (int a)
{
  return a;
  }*/


//#include "llvm/BasicBlock.h"
#include "llvm/DerivedTypes.h"
#include "llvm/GlobalValue.h"
#include "llvm/Module.h"
#include "llvm/Analysis/Verifier.h"
#include "llvm/Support/IRBuilder.h"
#include "llvm/Target/TargetMachine.h"
#include "llvm/Target/TargetData.h"

//#include "llack.h"

using namespace llvm;

Word* LlackModule::getWord(const std::string& name) {
  for (std::vector<Word*>::iterator iter = words.begin(); iter < words.end(); ++iter) {
    Word* word = *iter;
    if (word->name == name) return word;
  }
  return NULL;
}

LlackInstruction::~LlackInstruction() {}

Word::Word() {}

Word* Word::Create(const std::string& name, LlackModule* mod) {
  Word* word = new Word();
  word->name = name;
  mod->words.push_back(word);
  return word;
}

VMCodeGenInterface::~VMCodeGenInterface() {}

SimpleVMCodeGenInterface::~SimpleVMCodeGenInterface() {}
const TargetData* SimpleVMCodeGenInterface::getTargetData() {
  return td;
}
Type* SimpleVMCodeGenInterface::getContType() {
  // XXX: could use actual function type; would make it
  // easier to do tail call...
  return PointerType::getUnqual(Type::Int8Ty);
}
Value* SimpleVMCodeGenInterface::getWordCont(Word* word) {
  ConstantInt* intValue = ConstantInt::get(Type::Int32Ty, (int) word, false);
  Value* ptrValue = builder->CreateIntToPtr(intValue, getContType());
  return ptrValue;
}
void SimpleVMCodeGenInterface::pushData(Value *v) {
  push(getDataStack(), v);
}
Value* SimpleVMCodeGenInterface::popData(const Type *t) {
  return pop(getDataStack(), t);
}
void SimpleVMCodeGenInterface::pushRetain(Value *v) {
  push(getRetainStack(), v);
}
Value* SimpleVMCodeGenInterface::popRetain(Type *t) {
  return pop(getRetainStack(), t);
}
void SimpleVMCodeGenInterface::pushCont(Value *v) {
  push(getContStack(), v);
}
Value* SimpleVMCodeGenInterface::popCont() {
  return pop(getContStack(), getContType());
}
Instruction* SimpleVMCodeGenInterface::addInstruction(Instruction* inst) {
  return builder->Insert(inst);
}
Type* SimpleVMCodeGenInterface::getStackType() {
  return PointerType::getUnqual(Type::Int8Ty);
}
Type* SimpleVMCodeGenInterface::getVMStateType() {
  return StructType::get(
			 getStackType(), // data
			 getStackType(), // retain
			 getStackType(), // cont
			 NULL
			 );
}
Value* SimpleVMCodeGenInterface::getDataStack() {
  return builder->CreateStructGEP(vmStatePtr, 0);
}
Value* SimpleVMCodeGenInterface::getRetainStack() {
  return builder->CreateStructGEP(vmStatePtr, 1);
}
Value* SimpleVMCodeGenInterface::getContStack() {
  return builder->CreateStructGEP(vmStatePtr, 2);
}
void SimpleVMCodeGenInterface::push(Value* stack, Value* v) {
  const Type *t = v->getType();

  Value *top = builder->CreateLoad(stack);
  Value *castTop = builder->CreateBitCast(top, PointerType::getUnqual(t));
  builder->CreateStore(v, castTop);

  // XXX: align stack
  int elementSize = td->getTypeStoreSize(t);
  ConstantInt *topDelta = ConstantInt::get(Type::Int32Ty, elementSize, true);
  Value *newTop = builder->CreateGEP(top, topDelta);
  builder->CreateStore(newTop, stack);
}
Value* SimpleVMCodeGenInterface::pop(Value* stack, const Type* t) {
  int elementSize = td->getTypeStoreSize(t);
  ConstantInt *topDelta = ConstantInt::get(Type::Int32Ty, -1 * elementSize, true);
  // move top down
  Value *top = builder->CreateLoad(stack);
  Value *newTop = builder->CreateGEP(top, topDelta);
  builder->CreateStore(newTop, stack);
  // load value
  Value *castTop = builder->CreateBitCast(newTop, PointerType::getUnqual(t));
  Value *topValue = builder->CreateLoad(castTop);
  return topValue;
}


PushLlackInst::~PushLlackInst()  {}
void PushLlackInst::codeGen(VMCodeGenInterface* cgi)  {
  cgi->pushData(v);
}

PushWordLlackInst::~PushWordLlackInst()  {}
void PushWordLlackInst::codeGen(VMCodeGenInterface* cgi)  {
  Value* v = cgi->getWordCont(word);
  cgi->pushData(v);
}

ToContLlackInst::~ToContLlackInst()  {}
void ToContLlackInst::codeGen(VMCodeGenInterface* cgi)  {
  Value* v = cgi->popData(cgi->getContType());
  cgi->pushCont(v);
}

FromContLlackInst::~FromContLlackInst()  {}
void FromContLlackInst::codeGen(VMCodeGenInterface* cgi)  {
  Value* v = cgi->popCont();
  cgi->pushData(v);
}

SubLlackInst::~SubLlackInst()  {}
void SubLlackInst::codeGen(VMCodeGenInterface* cgi)  {
  Value* b = cgi->popData(t);
  Value* a = cgi->popData(t);
  Value* c = cgi->addInstruction(BinaryOperator::createSub(a, b));
  cgi->pushData(c);
}

MulLlackInst::~MulLlackInst()  {}
void MulLlackInst::codeGen(VMCodeGenInterface* cgi)  {
  Value* b = cgi->popData(t);
  Value* a = cgi->popData(t);
  Value* c = cgi->addInstruction(BinaryOperator::createMul(a, b));
  cgi->pushData(c);
}

SelectLlackInst::~SelectLlackInst()  {}
void SelectLlackInst::codeGen(VMCodeGenInterface* cgi)  {
  Value* falseValue = cgi->popData(t);
  Value* trueValue = cgi->popData(t);
  Value* boolean = cgi->popData(Type::Int1Ty);
  Value* result = cgi->addInstruction(SelectInst::Create(boolean, trueValue, falseValue));
  cgi->pushData(result);
}

ICmpLlackInst::~ICmpLlackInst()  {}
void ICmpLlackInst::codeGen(VMCodeGenInterface* cgi)  {
  Value* rhs = cgi->popData(t);
  Value* lhs = cgi->popData(t);
  Value* result = cgi->addInstruction(new ICmpInst(p, lhs, rhs));
  cgi->pushData(result);
}

ShuffleLlackInst::~ShuffleLlackInst()  {}
void ShuffleLlackInst::codeGen(VMCodeGenInterface* cgi)  {
  std::vector<Value*> values;
  for (std::vector<Type*>::iterator iter = consumption.begin(); iter < consumption.end(); ++iter) {
    Type* t = *iter;
    Value* v = cgi->popData(t);
    values.push_back(v);
  }
  std::reverse(values.begin(), values.end()); // XXX: Could avoid this for efficiency.
  for (std::vector<int>::iterator iter = production.begin(); iter < production.end(); ++iter) {
    int index = *iter;
    Value* v = values[index];
    cgi->pushData(v);
  }
}

VMState* newVMState() {
  VMState* vmState = new VMState();
  vmState->dataBottom = vmState->data = new char[1024];
  vmState->retainBottom = vmState->retain = new char[1024];
  vmState->contBottom = vmState->cont = new char[1024];
  return vmState;
}
void freeVMState(VMState* vmState) {
  delete[] vmState->dataBottom;
  delete[] vmState->retainBottom;
  delete[] vmState->contBottom;
}
void dumpStack(Stack bottom, Stack top) {
  printf("<");
  for (Stack i = bottom; i < top; ++i) {
    printf(" %02x", (unsigned char) *i);
  }
  printf(" >\n");
}
void dumpVMState(VMState* vmState) {
  printf("----------\n");
  printf("vmState: %p\n", (void*) vmState);
  printf("vmState->data: %p\n", vmState->data);
  dumpStack(vmState->dataBottom, vmState->data);
  printf("vmState->retain: %p\n", vmState->retain);
  dumpStack(vmState->retainBottom, vmState->retain);
  printf("vmState->cont: %p\n", vmState->cont);
  dumpStack(vmState->contBottom, vmState->cont);
  printf("----------\n");
}
