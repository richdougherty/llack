/* -*- Mode: C++; c-basic-offset: 2; indent-tabs-mode: nil; tab-width: 2 -*- */

#include <stdio.h>
#include <stdlib.h>

#include <unistd.h>
#include <utility>

#include "llack.h"
#include "llacktest.h"
#include "llvm/Target/TargetData.h"
#include "llvm/ModuleProvider.h"
#include "llvm/ExecutionEngine/GenericValue.h"
#include "llvm/CallingConv.h"
#include "llvm/ADT/StringMap.h"

void printStack(char* message, char* bot, char* top) {
  printf("%s:\n", message);
  for (char* i = top - 1; i >= bot; i--) {
    printf("%d\n", *i);
  }
}

template <class T>
void push(char** stack, T value) {
  *((T*) *stack) = value;
  *stack += sizeof(T);
}

template <class T>
T pop(char** stack) {
  *stack -= sizeof(T);
  return *((T*) *stack);
}

Function* generateFunction(Module* module, ExecutionEngine* ee, Word* word, bool generateDebug) {
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
  
  Function *func = Function::Create(funcTy, Function::ExternalLinkage, "", module);
  func->setCallingConv(CallingConv::Fast);
  
  Function::arg_iterator args = func->arg_begin();
  Value* vmStatePtr = args++;
  vmStatePtr->setName("vmStatePtr");

  BasicBlock *bb = BasicBlock::Create("entry", func);
  IRBuilder builder;
  builder.SetInsertPoint(bb);
  
  VMCodeGenInterface* cgi = new SimpleVMCodeGenInterface(module, ee->getTargetData(), vmStatePtr, &builder);
  for (std::vector<LlackInstruction*>::iterator iter = word->instructions.begin(); iter < word->instructions.end(); ++iter) {
    // Instruction generation
  
    LlackInstruction* inst = *iter;
    printf("Generating instruction code.\n");
    //Value *dataSP = builder.CreateStructGEP(vmStatePtr, 0);
    inst->codeGen(cgi);

    // Pointer to dumping Code
    
    if (generateDebug) {
      ConstantInt* intValue = ConstantInt::get(Type::Int32Ty, (int) &dumpVMState, false);
      Value* dumpVMStatePtr = builder.CreateIntToPtr(intValue, PointerType::getUnqual(funcTy));
      builder.CreateCall(dumpVMStatePtr, vmStatePtr);
    }
  }    

  builder.CreateRetVoid();

  return func;
}

int
main (int argc, char ** argv)
{
  LlackModule* mod = createFactorialModule();
  
  /// Running

  Module *module = new Module("module");
  module->setDataLayout("e-p:32:32:32-i1:8:8-i8:8:8-i16:16:16-i32:32:32-i64:32:64-f32:32:32-f64:32:64-v64:64:64-v128:128:128-a0:0:64-f80:128:128");
  module->setTargetTriple("i386-apple-darwin8");
  
  ExistingModuleProvider emp(module);
  ExecutionEngine *ee = ExecutionEngine::create(&emp, false);

  ProgramWriter* pw = new ProgramWriter(1024, module, ee);
  for (std::vector<Word*>::iterator iter = mod->words.begin(); iter < mod->words.end(); ++iter) {
    pw->insertWord(*iter);
  }

  VMState *vmState = newVMState();

  push(&vmState->cont, (Word*) NULL);  
  Word* word_main = mod->getWord("main");
  ProgramWriter::Location mainLocation = pw->getWordLocation(word_main);
  printf("pushing main onto cont: %p\n", (void*) word_main);
  assert(word_main != NULL && "Expected 'main' word.");
  push(&vmState->cont, mainLocation);

  for (;;) {
    dumpVMState(vmState);

    Interpretable** cont = pop<Interpretable**>(&vmState->cont);
    
    if (cont == NULL) break;
    printf("==== cont: (%p) ====\n", (void*) cont);
    Interpretable* i = *cont;
    i->interpret(cont + 1, vmState);
  }
  
  freeVMState(vmState);
  
  return 0;
}

