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
#include "llvm/CallingConv.h"
#include "llvm/ExecutionEngine/GenericValue.h"

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

const Type* LlvmLlackType::getLlvmType() const {
  return llvmType;
}

Word::Word() {}

Word* Word::Create(const std::string& name, LlackModule* mod) {
  Word* word = new Word();
  word->name = name;
  mod->words.push_back(word);
  return word;
}

VMCodeGenInterface::~VMCodeGenInterface() {}

const Type* VMCodeGenInterface::getLlvmType(const LlackType* llackType) {
  if (const LlvmLlackType* llvmLlackType = dynamic_cast<const LlvmLlackType*>(llackType)) {
    return llvmLlackType->getLlvmType();
  } else if (dynamic_cast<const QuotationType*>(llackType) != NULL) {
    return getContType();
  } else {
    // XXX: More straightforward error handling?
    assert(false && "Unknown LlvmValue type.");
    return NULL;
  }
}

Value* VMCodeGenInterface::getLlvmValue(LlackValue* llackValue) {
  if (LlvmLlackValue* llvmLlackValue = dynamic_cast<LlvmLlackValue*>(llackValue)) {
    return llvmLlackValue->getLlvmValue();
  } else if (Word* word = dynamic_cast<Word*>(llackValue)) {
    return getWordCont(word);
  } else {
    // XXX: More straightforward error handling?
    assert(false && "Unknown LlvmValue type.");
    return NULL;
  }
}

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
  Value* llvmValue = cgi->getLlvmValue(v);
  cgi->pushData(llvmValue);
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
  const Type* llvmType = cgi->getLlvmType(t);
  Value* falseValue = cgi->popData(llvmType);
  Value* trueValue = cgi->popData(llvmType);
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
  for (std::vector<LlackType*>::iterator iter = consumption.begin(); iter < consumption.end(); ++iter) {
    LlackType* llackType = *iter;
    const Type* llvmType = cgi->getLlvmType(llackType);
    Value* v = cgi->popData(llvmType);
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

/// Interpretables ///

Interpretable::~Interpretable() {}

void WordInterpretable::initFunction() {

  Module* module = pw->getModule();

  Type* stackType = PointerType::getUnqual(Type::Int8Ty);
  Type* vmStateType = StructType::get(
     stackType, // data
     stackType, // retain
     stackType, // cont
     NULL
    );
  
  // Build Function for Word.

  std::vector<const Type*> argTypes;
  argTypes.push_back(PointerType::getUnqual(vmStateType));
  FunctionType *funcTy = FunctionType::get(Type::VoidTy, argTypes, false);
  
  // Build Function for Word
  
  function = Function::Create(funcTy, Function::ExternalLinkage, "", module);
  function->setCallingConv(CallingConv::Fast);
  
  Function::arg_iterator args = function->arg_begin();
  Value* vmStatePtr = args++;
  vmStatePtr->setName("vmStatePtr");

  BasicBlock *bb = BasicBlock::Create("entry", function);
  IRBuilder builder;
  builder.SetInsertPoint(bb);
  
  VMCodeGenInterface* cgi = pw->getVMCodeGenInterface(vmStatePtr, &builder);
  codeGen(NULL, cgi);
  builder.CreateRetVoid();
}

void WordInterpretable::interpret(void* vpc, VMState* vmState) {
  if (function == NULL) {
    initFunction();
  }
  ExecutionEngine* ee = pw->getExecutionEngine();
  std::vector<GenericValue> argValues(1);
  argValues[0].PointerVal = vmState;
  //printf("Running code.\n");
  ee->runFunction(function, argValues);
}

WordInterpretable::~WordInterpretable() {
  ExecutionEngine* ee = pw->getExecutionEngine();
  ee->freeMachineCodeForFunction(function);
}

void WordInterpretable::codeGen(void* vpc, VMCodeGenInterface* cgi) {
  for (std::vector<LlackInstruction*>::iterator iter = word->instructions.begin(); iter < word->instructions.end(); ++iter) {
    // Instruction generation
  
    LlackInstruction* inst = *iter;
    //printf("Generating instruction code.\n");
    //Value *dataSP = builder.CreateStructGEP(vmStatePtr, 0);
    inst->codeGen(cgi);

    // Pointer to dumping Code
    
    /*if (generateDebug) {
      ConstantInt* intValue = ConstantInt::get(Type::Int32Ty, (int) &dumpVMState, false);
      Value* dumpVMStatePtr = builder.CreateIntToPtr(intValue, PointerType::getUnqual(funcTy));
      builder.CreateCall(dumpVMStatePtr, vmStatePtr);
    }*/
  }    
}

/*
class PushInterpretable : public Interpretable {
 public:
  PushInterpretable() {}
  virtual void interpret(void* vpc, VMState* vmState);
  virtual void codeGen(void* vpc, VMCodeGenInterface* cgi);
};

class CallInterpretable : public Interpretable {
 public:
  CallInterpretable() {}
  virtual void interpret(void* vpc, VMState* vmState);
  virtual void codeGen(void* vpc, VMCodeGenInterface* cgi);
};

class JumpInterpretable : public Interpretable {
 public:
  JumpInterpretable() {}
  virtual void interpret(void* vpc, VMState* vmState);
  virtual void codeGen(void* vpc, VMCodeGenInterface* cgi);
};

class ReturnInterpretable : public Interpretable {
 public:
  ReturnInterpretable() {}
  virtual void interpret(void* vpc, VMState* vmState);
  virtual void codeGen(void* vpc, VMCodeGenInterface* cgi);
};
*/

ProgramWriter::ProgramWriter(int size, Module* module, ExecutionEngine* ee) {
  first = new char[size];
  next = first;
  last = first + size;
  this->module = module;
  this->ee = ee;
}

ProgramWriter::~ProgramWriter() {
  // XXX: Delete all interpretables.
  delete[] first;
}

ProgramWriter::Location ProgramWriter::insertWord(Word* word) {
  printf("setting location for %s to %p\n", word->name.c_str(), (void*) next);
  ProgramWriter::Location nextLocation = (ProgramWriter::Location) next;
  *nextLocation = new WordInterpretable(word, this);
  wordMap[word] = nextLocation;
  next += sizeof(ProgramWriter::Location);
  return nextLocation;
}

//Location ProgramWriter::insertPush(Constant* constant);
//Location ProgramWriter::insertCall(Location location);
//Location ProgramWriter::insertJump(Location location);
//Location ProgramWriter::insertReturn();

ProgramWriter::Location ProgramWriter::getWordLocation(Word* word) {
  printf("getting location for: %s\n", word->name.c_str());
  return wordMap[word];
}

VMCodeGenInterface* ProgramWriter::getVMCodeGenInterface(Value* vmStatePtr, IRBuilder* builder) {
  return new ProgramVMCodeGenInterface(this, vmStatePtr, builder);
}



ProgramVMCodeGenInterface::~ProgramVMCodeGenInterface() {}
const TargetData* ProgramVMCodeGenInterface::getTargetData() {
  return pw->getExecutionEngine()->getTargetData();
}
Type* ProgramVMCodeGenInterface::getContType() {
  // XXX: could use actual function type; would make it
  // easier to do tail call...
  return PointerType::getUnqual(Type::Int8Ty);
}
Value* ProgramVMCodeGenInterface::getWordCont(Word* word) {
  ProgramWriter::Location location = pw->getWordLocation(word);
  assert(location != NULL);
  ConstantInt* intValue = ConstantInt::get(Type::Int32Ty, (int) location, false);
  Value* ptrValue = builder->CreateIntToPtr(intValue, getContType());
  return ptrValue;
}
void ProgramVMCodeGenInterface::pushData(Value *v) {
  push(getDataStack(), v);
}
Value* ProgramVMCodeGenInterface::popData(const Type *t) {
  return pop(getDataStack(), t);
}
void ProgramVMCodeGenInterface::pushRetain(Value *v) {
  push(getRetainStack(), v);
}
Value* ProgramVMCodeGenInterface::popRetain(Type *t) {
  return pop(getRetainStack(), t);
}
void ProgramVMCodeGenInterface::pushCont(Value *v) {
  push(getContStack(), v);
}
Value* ProgramVMCodeGenInterface::popCont() {
  return pop(getContStack(), getContType());
}
Instruction* ProgramVMCodeGenInterface::addInstruction(Instruction* inst) {
  return builder->Insert(inst);
}
Type* ProgramVMCodeGenInterface::getStackType() {
  return PointerType::getUnqual(Type::Int8Ty);
}
Type* ProgramVMCodeGenInterface::getVMStateType() {
  return StructType::get(
			 getStackType(), // data
			 getStackType(), // retain
			 getStackType(), // cont
			 NULL
			 );
}
Value* ProgramVMCodeGenInterface::getDataStack() {
  return builder->CreateStructGEP(vmStatePtr, 0);
}
Value* ProgramVMCodeGenInterface::getRetainStack() {
  return builder->CreateStructGEP(vmStatePtr, 1);
}
Value* ProgramVMCodeGenInterface::getContStack() {
  return builder->CreateStructGEP(vmStatePtr, 2);
}
void ProgramVMCodeGenInterface::push(Value* stack, Value* v) {
  const Type *t = v->getType();

  Value *top = builder->CreateLoad(stack);
  Value *castTop = builder->CreateBitCast(top, PointerType::getUnqual(t));
  builder->CreateStore(v, castTop);

  // XXX: align stack
  const TargetData* td = pw->getExecutionEngine()->getTargetData();
  int elementSize = td->getTypeStoreSize(t);
  ConstantInt *topDelta = ConstantInt::get(Type::Int32Ty, elementSize, true);
  Value *newTop = builder->CreateGEP(top, topDelta);
  builder->CreateStore(newTop, stack);
}
Value* ProgramVMCodeGenInterface::pop(Value* stack, const Type* t) {
  const TargetData* td = pw->getExecutionEngine()->getTargetData();
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
