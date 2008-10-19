/*
 * File: llack.h
 *
 *      This is a llack header file that is global to the entire project.
 *      It is located here so that everyone will find it.
 */
 
#ifndef LLACK_LLACK_H
#define LLACK_LLACK_H

#include "llvm/DerivedTypes.h"
#include "llvm/GlobalValue.h"
#include "llvm/Module.h"
#include "llvm/Analysis/Verifier.h"
#include "llvm/Support/IRBuilder.h"
#include "llvm/Target/TargetMachine.h"
#include "llvm/ExecutionEngine/ExecutionEngine.h"
#include <cstdio>
#include <string>
#include <map>
#include <vector>

using namespace llvm;

typedef char* Stack;

struct VMState {
  Stack data;
  Stack retain;
  Stack cont;
  Stack dataBottom;
  Stack retainBottom;
  Stack contBottom;
};

extern "C" VMState* newVMState();
extern "C" void freeVMState(VMState* vmState);
extern "C" void dumpVMState(VMState* vmState);

class VMCodeGenInterface;
class Word;

class LlackModule {
 public:
  std::vector<Word*> words;
  Word* getWord(const std::string& name);
};

class LlackInstruction {
 public:
  virtual ~LlackInstruction();
  virtual void codeGen(VMCodeGenInterface* cgi) = 0;
};

class Word {
 public:
  Word();
  std::string name;
  std::vector<LlackInstruction*> instructions;
  static Word* Create(const std::string& name, LlackModule* mod);
};

class VMCodeGenInterface {
 public:
  // XXX: Needs anchor?
  virtual ~VMCodeGenInterface();
  virtual const TargetData* getTargetData() = 0;
  virtual Type* getContType() = 0;
  virtual Value* getWordCont(Word* word) = 0;
  virtual void pushData(Value *v) = 0;
  virtual Value* popData(const Type *t) = 0;
  virtual void pushRetain(Value *v) = 0;
  virtual Value* popRetain(Type *t) = 0;
  virtual void pushCont(Value *v) = 0;
  virtual Value* popCont() = 0;
  virtual Instruction* addInstruction(Instruction* inst) = 0;
};

class SimpleVMCodeGenInterface : public VMCodeGenInterface {
 private:
  Module* mod;
  const TargetData* td;
  Value* vmStatePtr;
  IRBuilder* builder;
 public:
 SimpleVMCodeGenInterface(Module* mod_, const TargetData* td_, Value* vmStatePtr_, IRBuilder* builder_):
  mod(mod_), td(td_), vmStatePtr(vmStatePtr_), builder(builder_)
  {}
  virtual ~SimpleVMCodeGenInterface();
  virtual const TargetData* getTargetData();
  virtual Type* getContType();
  virtual Value* getWordCont(Word* word);
  virtual void pushData(Value *v);
  virtual Value* popData(const Type *t);
  virtual void pushRetain(Value *v);
  virtual Value* popRetain(Type *t);
  virtual void pushCont(Value *v);
  virtual Value* popCont();
  virtual Instruction* addInstruction(Instruction* inst);
 private:
  Type* getStackType();
  Type* getVMStateType();
  Value* getDataStack();
  Value* getRetainStack();
  Value* getContStack();
  void push(Value* stack, Value* v);
  Value* pop(Value* stack, const Type* t);
};

// XXX: Need sep instruction for Words until we implement
// a LlackValue type.
class PushWordLlackInst : public LlackInstruction {
 private:
  Word* word;
 public:
 PushWordLlackInst(Word* word_)
   : word(word_)
  {}
  virtual ~PushWordLlackInst();
  virtual void codeGen(VMCodeGenInterface* cgi);
};

class PushLlackInst : public LlackInstruction {
 private:
  Value* v;
 public:
 PushLlackInst(Value* v_)
   : v(v_)
  {}
  virtual ~PushLlackInst();
  virtual void codeGen(VMCodeGenInterface* cgi);
};

class ToContLlackInst : public LlackInstruction {
 public:
  ToContLlackInst() {}
  virtual ~ToContLlackInst();
  virtual void codeGen(VMCodeGenInterface* cgi);
};

class FromContLlackInst : public LlackInstruction {
 public:
  FromContLlackInst() {}
  virtual ~FromContLlackInst();
  virtual void codeGen(VMCodeGenInterface* cgi);
};

class SubLlackInst : public LlackInstruction {
 private:
  const Type* t;
 public:
 SubLlackInst(const Type* t_) : t(t_) {}
  virtual ~SubLlackInst();
  virtual void codeGen(VMCodeGenInterface* cgi);
};

class SelectLlackInst : public LlackInstruction {
 private:
  const Type* t;
 public:
 SelectLlackInst(const Type* t_) : t(t_) {}
  virtual ~SelectLlackInst();
  virtual void codeGen(VMCodeGenInterface* cgi);
};

class MulLlackInst : public LlackInstruction {
 private:
  const Type* t;
 public:
 MulLlackInst(const Type* t_) : t(t_) {}
  virtual ~MulLlackInst();
  virtual void codeGen(VMCodeGenInterface* cgi);
};

class ICmpLlackInst : public LlackInstruction {
 private:
  ICmpInst::Predicate p;
  const Type* t;
 public:
 ICmpLlackInst(ICmpInst::Predicate p_, const Type* t_) : p(p_), t(t_) {}
  virtual ~ICmpLlackInst();
  virtual void codeGen(VMCodeGenInterface* cgi);
};

class ShuffleLlackInst : public LlackInstruction {
 private:
  std::vector<Type*> consumption;
  std::vector<int> production;
 public:
 ShuffleLlackInst(std::vector<Type*> consumption_, std::vector<int> production_)
   : consumption(consumption_), production(production_)
  {}
  virtual ~ShuffleLlackInst();
  virtual void codeGen(VMCodeGenInterface* cgi);
};

#endif