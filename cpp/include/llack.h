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

// XXX: Need to think about memory management for these types.
// XXX: Anchor.
class LlackType {
 public:
  virtual ~LlackType() {}
};
class LlvmLlackType : public LlackType {
 private:
  const Type* llvmType;
 public:
  LlvmLlackType(const Type* llvmType_) : llvmType(llvmType_) {}
  const Type* getLlvmType() const;
};
class LocationType : public LlackType {}; // XXX: Make singleton?

// XXX: Anchor.
class LlackValue {
 public:
  virtual ~LlackValue() {}
  virtual LlackType* getLlackType() = 0;
};
class LlvmLlackValue : public LlackValue {
 private:
  Value* llvmValue;
 public:
  LlvmLlackValue(Value* llvmValue_) : llvmValue(llvmValue_) {}
  Value* getLlvmValue() { return llvmValue; }
  virtual LlackType* getLlackType() {
    const Type* llackType = llvmValue->getType();
    return new LlvmLlackType(llackType);
  }
};

class Location : public LlackValue {
 private:
  Value* llvmValue;
 public:
  Location(Value* llvmValue_) : llvmValue(llvmValue_) {}
  Value* getLlvmValue() { return llvmValue; }
  virtual LlackType* getLlackType() {
    return new LocationType();
  }
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

  // Code generation
  virtual void pushData(LlackValue *v) = 0;
  virtual LlackValue* popData(LlackType* t) = 0;
  virtual void pushRetain(LlackValue* v) = 0;
  virtual LlackValue* popRetain(LlackType* t) = 0;
  virtual void pushCont(Location* w) = 0;
  virtual Location* popCont() = 0;
  virtual Instruction* addInstruction(Instruction* inst) = 0;

  virtual Location* getWordCont(Word* word) = 0; // XXX: Rename to 'lookup'?

  // Conversion between LLVM and Llack types and values
  virtual Type* getContType() = 0; // XXX: Rename to 'getLlvmLocationType'?
  const Type* getLlvmType(LlackType* llackType);
  Value* getLlvmValue(LlackValue* llackValue);
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
  virtual void pushData(LlackValue *v);
  virtual LlackValue* popData(LlackType* t);
  virtual void pushRetain(LlackValue* v);
  virtual LlackValue* popRetain(LlackType* t);
  virtual void pushCont(Location* w);
  virtual Location* popCont();
  virtual Instruction* addInstruction(Instruction* inst);
  virtual Type* getContType();
  virtual Location* getWordCont(Word* word);
 private:
  Type* getStackType();
  Type* getVMStateType();
  Value* getDataStack();
  Value* getRetainStack();
  Value* getContStack();
  void push(Value* stack, LlackValue* v);
  LlackValue* pop(Value* stack, LlackType* t);
};

class PushLlackInst : public LlackInstruction {
 private:
  LlackValue* v;
 public:
 PushLlackInst(LlackValue* v_)
   : v(v_)
  {}
  virtual ~PushLlackInst();
  virtual void codeGen(VMCodeGenInterface* cgi);
};

class LookupLlackInst : public LlackInstruction {
 private:
  Word* w;
 public:
 LookupLlackInst(Word* w_)
   : w(w_)
  {}
  virtual ~LookupLlackInst();
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
  LlackType* t; // XXX: Make into LlackType?
 public:
 SubLlackInst(LlackType* t_) : t(t_) {}
  virtual ~SubLlackInst();
  virtual void codeGen(VMCodeGenInterface* cgi);
};

class SelectLlackInst : public LlackInstruction {
 private:
  LlackType* t;
 public:
 SelectLlackInst(LlackType* t_) : t(t_) {}
  virtual ~SelectLlackInst();
  virtual void codeGen(VMCodeGenInterface* cgi);
};

class MulLlackInst : public LlackInstruction {
 private:
  LlackType* t; // XXX: Make into LlackType?
 public:
 MulLlackInst(LlackType* t_) : t(t_) {}
  virtual ~MulLlackInst();
  virtual void codeGen(VMCodeGenInterface* cgi);
};

class ICmpLlackInst : public LlackInstruction {
 private:
  ICmpInst::Predicate p;
  LlackType* t;
 public:
 ICmpLlackInst(ICmpInst::Predicate p_, LlackType* t_) : p(p_), t(t_) {}
  virtual ~ICmpLlackInst();
  virtual void codeGen(VMCodeGenInterface* cgi);
};

class ShuffleLlackInst : public LlackInstruction {
 private:
  std::vector<LlackType*> consumption;
  std::vector<int> production;
 public:
 ShuffleLlackInst(std::vector<LlackType*> consumption_, std::vector<int> production_)
   : consumption(consumption_), production(production_)
  {}
  virtual ~ShuffleLlackInst();
  virtual void codeGen(VMCodeGenInterface* cgi);
};

class Interpretable {
 public:
  virtual ~Interpretable() = 0;
  /// Execute this region at the given VPC.
  virtual void interpret(void* vpc, VMState* vmState) = 0;
  /// Generate code to execute this region at the given VPC.
  virtual void codeGen(void* vpc, VMCodeGenInterface* cgi) = 0;
};

class ProgramWriter {
 public:
  typedef Interpretable** Location;
 private:
  char* first;
  char* next;
  char* last;
  std::map<Word*,Location> wordMap;
  Module* module;
  ExecutionEngine* ee;
 public:
  ProgramWriter(int size, Module* module, ExecutionEngine* ee);
  ~ProgramWriter();
  
  Module* getModule() {
    return module;
  }
  ExecutionEngine* getExecutionEngine() {
    return ee;
  }
  VMCodeGenInterface* getVMCodeGenInterface(Value* vmStatePtr, IRBuilder* builder);
  
  Location insertWord(Word* word);
  Location insertPush(Constant* constant);
  Location insertCall(Location location);
  Location insertJump(Location location);
  Location insertReturn();
  /// Get the location of the given, already-inserted word.
  Location getWordLocation(Word* word);
  
};

// XXX: Needs better name.
class ProgramVMCodeGenInterface : public VMCodeGenInterface {
 private:
  ProgramWriter* pw;
  Value* vmStatePtr;
  IRBuilder* builder;
 public:
  ProgramVMCodeGenInterface(ProgramWriter* pw_, Value* vmStatePtr_, IRBuilder* builder_):
    pw(pw_), vmStatePtr(vmStatePtr_), builder(builder_) {}
  virtual ~ProgramVMCodeGenInterface();
  virtual const TargetData* getTargetData();
  virtual void pushData(LlackValue *v);
  virtual LlackValue* popData(LlackType* t);
  virtual void pushRetain(LlackValue* v);
  virtual LlackValue* popRetain(LlackType* t);
  virtual void pushCont(Location* w);
  virtual Location* popCont();
  virtual Instruction* addInstruction(Instruction* inst);
  virtual Type* getContType();
  virtual Location* getWordCont(Word* word);
 private:
  Type* getStackType();
  Type* getVMStateType();
  Value* getDataStack();
  Value* getRetainStack();
  Value* getContStack();
  void push(Value* stack, LlackValue* v);
  LlackValue* pop(Value* stack, LlackType* t);
};


class WordInterpretable : public Interpretable {
 private:
  Word* word;
  Function* function;
  ProgramWriter* pw;
  void initFunction();
 public:
  WordInterpretable(Word* word_, ProgramWriter* pw_) : word(word_), pw(pw_) {}
  virtual ~WordInterpretable();
  virtual void interpret(void* vpc, VMState* vmState);
  virtual void codeGen(void* vpc, VMCodeGenInterface* cgi);
};

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

#endif