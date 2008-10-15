/* -*- Mode: C++; c-basic-offset: 2; indent-tabs-mode: nil; tab-width: 2 -*- */

#include <stdio.h>
#include <stdlib.h>

#include <unistd.h>
#include <utility>

#include "llack.h"
#include "llvm/Target/TargetData.h"
#include "llvm/ModuleProvider.h"
#include "llvm/ExecutionEngine/GenericValue.h"

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

int
main (int argc, char ** argv)
{  
  /// Program 3.0
  
  LlackModule* mod = new LlackModule();
  Word* word_main = Word::Create("main", mod);
  Word* word_factorial$i32 = Word::Create("factorial$i32", mod);
  Word* word_factorial_accum$i32 = Word::Create("factorial_accum$i32", mod);
  Word* word_swap$i32_i32 = Word::Create("swap$i32_i32", mod);
  Word* word_factorial_accum$k8$i32 = Word::Create("factorial_accum$k8$i32", mod);
  Word* word_dup$i32 = Word::Create("dup$i32", mod);
  Word* word_factorial_accum$k9$i32 = Word::Create("factorial_accum$k9$i32", mod);
  Word* word_icmp$sle_i32 = Word::Create("icmp$sle_i32", mod);
  Word* word_factorial_accum$k10$i32 = Word::Create("factorial_accum$k10$i32", mod);
  Word* word_factorial_accum$q1$i32 = Word::Create("factorial_accum$q1$i32", mod);
  Word* word_factorial_accum$q2$i32 = Word::Create("factorial_accum$q2$i32", mod);
  Word* word_if = Word::Create("if", mod);
  Word* word_drop$i32 = Word::Create("drop$i32", mod);
  Word* word_factorial_accum$q2$k3$i32 = Word::Create("factorial_accum$q2$k3$i32", mod);
  Word* word_sub$i32 = Word::Create("sub$i32", mod);
  Word* word_factorial_accum$q2$k4$i32 = Word::Create("factorial_accum$q2$k4$i32", mod);
  Word* word_factorial_accum$q2$k5$i32 = Word::Create("factorial_accum$q2$k5$i32", mod);
  Word* word_rot$i32_i32_i32 = Word::Create("rot$i32_i32_i32", mod);
  Word* word_factorial_accum$q2$k6$i32 = Word::Create("factorial_accum$q2$k6$i32", mod);
  Word* word_mul$i32 = Word::Create("mul$i32", mod);
  Word* word_factorial_accum$q2$k7$i32 = Word::Create("factorial_accum$q2$k7$i32", mod);
  Word* word_select$quot = Word::Create("select$quot", mod);
  Word* word_if$k0 = Word::Create("if$k0", mod);
  Word* word_tocont = Word::Create("tocont", mod);
  
  {
    // main
    Word* word = word_main;
    {
      // push i32 5
      LlackInstruction* inst = new PushLlackInst(ConstantInt::get(APInt(32,  "5", 10)));
      word->instructions.push_back(inst);
    }
    {
      // push quot @factorial$i32
      LlackInstruction* inst = new PushWordLlackInst(word_factorial$i32);
      word->instructions.push_back(inst);
    }
    {
      // tocont
      LlackInstruction* inst = new ToContLlackInst();
      word->instructions.push_back(inst);
    }
  }
  
  {
    // factorial$i32
    Word* word = word_factorial$i32;
    {
      // push i32 1
      LlackInstruction* inst = new PushLlackInst(ConstantInt::get(APInt(32,  "1", 10)));
      word->instructions.push_back(inst);
    }
    {
      // push quot @factorial_accum$i32
      LlackInstruction* inst = new PushWordLlackInst(word_factorial_accum$i32);
      word->instructions.push_back(inst);
    }
    {
      // tocont
      LlackInstruction* inst = new ToContLlackInst();
      word->instructions.push_back(inst);
    }
  }
  
  {
    // factorial_accum$i32
    Word* word = word_factorial_accum$i32;
    {
      // push quot @swap$i32_i32
      LlackInstruction* inst = new PushWordLlackInst(word_swap$i32_i32);
      word->instructions.push_back(inst);
    }
    {
      // push quot @factorial_accum$k8$i32
      LlackInstruction* inst = new PushWordLlackInst(word_factorial_accum$k8$i32);
      word->instructions.push_back(inst);
    }
    {
      // tocont
      LlackInstruction* inst = new ToContLlackInst();
      word->instructions.push_back(inst);
    }
    {
      // tocont
      LlackInstruction* inst = new ToContLlackInst();
      word->instructions.push_back(inst);
    }
  }
  
  {
    // swap$i32_i32
    Word* word = word_swap$i32_i32;
    {
      // shuffle i32 i32 1 0
      std::vector<Type*> consumption;
      consumption.push_back(const_cast<Type*>((Type*) IntegerType::get(32)));
      consumption.push_back(const_cast<Type*>((Type*) IntegerType::get(32)));
      std::vector<int> production;
      production.push_back(1);
      production.push_back(0);
      LlackInstruction* inst = new ShuffleLlackInst(consumption, production);
      word->instructions.push_back(inst);
    }
  }

  {
    // factorial_accum$k8$i32
    Word* word = word_factorial_accum$k8$i32;
    {
      // push quot @dup$i32
      LlackInstruction* inst = new PushWordLlackInst(word_dup$i32);
      word->instructions.push_back(inst);
    }
    {
      // push quot @factorial_accum$k9$i32
      LlackInstruction* inst = new PushWordLlackInst(word_factorial_accum$k9$i32);
      word->instructions.push_back(inst);
    }
    {
      // tocont
      LlackInstruction* inst = new ToContLlackInst();
      word->instructions.push_back(inst);
    }
    {
      // tocont
      LlackInstruction* inst = new ToContLlackInst();
      word->instructions.push_back(inst);
    }
  }

  {
    // dup$i32
    Word* word = word_dup$i32;
    {
      // shuffle i32 0 0
      std::vector<Type*> consumption;
      consumption.push_back(const_cast<Type*>((Type*) IntegerType::get(32)));
      std::vector<int> production;
      production.push_back(0);
      production.push_back(0);
      LlackInstruction* inst = new ShuffleLlackInst(consumption, production);
      word->instructions.push_back(inst);
    }
  }

  {
    // factorial_accum$k9$i32
    Word* word = word_factorial_accum$k9$i32;
    {
      // push i32 1
      LlackInstruction* inst = new PushLlackInst(ConstantInt::get(APInt(32,  "1", 10)));
      word->instructions.push_back(inst);
    }
    {
      // push quot @icmp$sle_i32
      LlackInstruction* inst = new PushWordLlackInst(word_icmp$sle_i32);
      word->instructions.push_back(inst);
    }
    {
      // push quot @factorial_accum$k10$i32
      LlackInstruction* inst = new PushWordLlackInst(word_factorial_accum$k10$i32);
      word->instructions.push_back(inst);
    }
    {
      // tocont
      LlackInstruction* inst = new ToContLlackInst();
      word->instructions.push_back(inst);
    }
    {
      // tocont
      LlackInstruction* inst = new ToContLlackInst();
      word->instructions.push_back(inst);
    }
  }

  {
    // icmp$sle_i32
    Word* word = word_icmp$sle_i32;
    {
      // icmp sle i32
      LlackInstruction* inst = new ICmpLlackInst(ICmpInst::ICMP_SLE, IntegerType::get(32));
      word->instructions.push_back(inst);
    }
  }

{
  // factorial_accum$k10$i32
  Word* word = word_factorial_accum$k10$i32;
  {
    // push quot @factorial_accum$q1$i32
    LlackInstruction* inst = new PushWordLlackInst(word_factorial_accum$q1$i32);
    word->instructions.push_back(inst);
  }
  {
    // push quot @factorial_accum$q2$i32
    LlackInstruction* inst = new PushWordLlackInst(word_factorial_accum$q2$i32);
    word->instructions.push_back(inst);
  }
  {
    // push quot @if
    LlackInstruction* inst = new PushWordLlackInst(word_if);
    word->instructions.push_back(inst);
  }
  {
    // tocont
    LlackInstruction* inst = new ToContLlackInst();
    word->instructions.push_back(inst);
  }
}

{
  // factorial_accum$q1$i32
  Word* word = word_factorial_accum$q1$i32;
  {
    // push quot @drop$i32
    LlackInstruction* inst = new PushWordLlackInst(word_drop$i32);
    word->instructions.push_back(inst);
  }
  {
    // tocont
    LlackInstruction* inst = new ToContLlackInst();
    word->instructions.push_back(inst);
  }
}

{
  // factorial_accum$q2$i32
  Word* word = word_factorial_accum$q2$i32;
  {
    // push quot @dup$i32
    LlackInstruction* inst = new PushWordLlackInst(word_dup$i32);
    word->instructions.push_back(inst);
  }
  {
    // push quot @factorial_accum$q2$k3$i32
    LlackInstruction* inst = new PushWordLlackInst(word_factorial_accum$q2$k3$i32);
    word->instructions.push_back(inst);
  }
  {
    // tocont
    LlackInstruction* inst = new ToContLlackInst();
    word->instructions.push_back(inst);
  }
  {
    // tocont
    LlackInstruction* inst = new ToContLlackInst();
    word->instructions.push_back(inst);
  }
}

{
  // if
  Word* word = word_if;
  {
    // push quot @select$quot
    LlackInstruction* inst = new PushWordLlackInst(word_select$quot);
    word->instructions.push_back(inst);
  }
  {
    // push quot @if$k0
    LlackInstruction* inst = new PushWordLlackInst(word_if$k0);
    word->instructions.push_back(inst);
  }
  {
    // tocont
    LlackInstruction* inst = new ToContLlackInst();
    word->instructions.push_back(inst);
  }
  {
    // tocont
    LlackInstruction* inst = new ToContLlackInst();
    word->instructions.push_back(inst);
  }
}

{
  // drop$i32
  Word* word = word_drop$i32;
  {
    // shuffle i32
    std::vector<Type*> consumption;
    consumption.push_back(const_cast<Type*>((Type*) IntegerType::get(32)));
    std::vector<int> production;
    LlackInstruction* inst = new ShuffleLlackInst(consumption, production);
    word->instructions.push_back(inst);
  }
}

{
  // factorial_accum$q2$k3$i32
  Word* word = word_factorial_accum$q2$k3$i32;
  {
    // push i32 1
    LlackInstruction* inst = new PushLlackInst(ConstantInt::get(APInt(32,  "1", 10)));
    word->instructions.push_back(inst);
  }
  {
    // push quot @sub$i32
    LlackInstruction* inst = new PushWordLlackInst(word_sub$i32);
    word->instructions.push_back(inst);
  }
  {
    // push quot @factorial_accum$q2$k4$i32
    LlackInstruction* inst = new PushWordLlackInst(word_factorial_accum$q2$k4$i32);
    word->instructions.push_back(inst);
  }
  {
    // tocont
    LlackInstruction* inst = new ToContLlackInst();
    word->instructions.push_back(inst);
  }
  {
    // tocont
    LlackInstruction* inst = new ToContLlackInst();
    word->instructions.push_back(inst);
  }
}

{
  // sub$i32
  Word* word = word_sub$i32;
  {
    // sub i32
    LlackInstruction* inst = new SubLlackInst(IntegerType::get(32));
    word->instructions.push_back(inst);
  }
}

{
  // factorial_accum$q2$k4$i32
  Word* word = word_factorial_accum$q2$k4$i32;
  {
    // push quot @swap$i32_i32
    LlackInstruction* inst = new PushWordLlackInst(word_swap$i32_i32);
    word->instructions.push_back(inst);
  }
  {
    // push quot @factorial_accum$q2$k5$i32
    LlackInstruction* inst = new PushWordLlackInst(word_factorial_accum$q2$k5$i32);
    word->instructions.push_back(inst);
  }
  {
    // tocont
    LlackInstruction* inst = new ToContLlackInst();
    word->instructions.push_back(inst);
  }
  {
    // tocont
    LlackInstruction* inst = new ToContLlackInst();
    word->instructions.push_back(inst);
  }
}

{
  // factorial_accum$q2$k5$i32
  Word* word = word_factorial_accum$q2$k5$i32;
  {
    // push quot @rot$i32_i32_i32
    LlackInstruction* inst = new PushWordLlackInst(word_rot$i32_i32_i32);
    word->instructions.push_back(inst);
  }
  {
    // push quot @factorial_accum$q2$k6$i32
    LlackInstruction* inst = new PushWordLlackInst(word_factorial_accum$q2$k6$i32);
    word->instructions.push_back(inst);
  }
  {
    // tocont
    LlackInstruction* inst = new ToContLlackInst();
    word->instructions.push_back(inst);
  }
  {
    // tocont
    LlackInstruction* inst = new ToContLlackInst();
    word->instructions.push_back(inst);
  }
}

{
  // rot$i32_i32_i32
  Word* word = word_rot$i32_i32_i32;
  {
    // shuffle i32 i32 i32 1 2 0
    std::vector<Type*> consumption;
    consumption.push_back(const_cast<Type*>((Type*) IntegerType::get(32)));
    consumption.push_back(const_cast<Type*>((Type*) IntegerType::get(32)));
    consumption.push_back(const_cast<Type*>((Type*) IntegerType::get(32)));
    std::vector<int> production;
    production.push_back(1);
    production.push_back(2);
    production.push_back(0);
    LlackInstruction* inst = new ShuffleLlackInst(consumption, production);
    word->instructions.push_back(inst);
  }
}

{
  // factorial_accum$q2$k6$i32
  Word* word = word_factorial_accum$q2$k6$i32;
  {
    // push quot @mul$i32
    LlackInstruction* inst = new PushWordLlackInst(word_mul$i32);
    word->instructions.push_back(inst);
  }
  {
    // push quot @factorial_accum$q2$k7$i32
    LlackInstruction* inst = new PushWordLlackInst(word_factorial_accum$q2$k7$i32);
    word->instructions.push_back(inst);
  }
  {
    // tocont
    LlackInstruction* inst = new ToContLlackInst();
    word->instructions.push_back(inst);
  }
  {
    // tocont
    LlackInstruction* inst = new ToContLlackInst();
    word->instructions.push_back(inst);
  }
}

{
  // mul$i32
  Word* word = word_mul$i32;
  {
    // mul i32
    LlackInstruction* inst = new MulLlackInst(IntegerType::get(32));
    word->instructions.push_back(inst);
  }
}

{
  // factorial_accum$q2$k7$i32
  Word* word = word_factorial_accum$q2$k7$i32;
  {
    // push quot @factorial_accum$i32
    LlackInstruction* inst = new PushWordLlackInst(word_factorial_accum$i32);
    word->instructions.push_back(inst);
  }
  {
    // tocont
    LlackInstruction* inst = new ToContLlackInst();
    word->instructions.push_back(inst);
  }
}

{
  // select$quot
  Word* word = word_select$quot;
  {
    // select quot
    LlackInstruction* inst = new SelectLlackInst(PointerType::getUnqual(Type::Int8Ty));
    word->instructions.push_back(inst);
  }
}

{
  // if$k0
  Word* word = word_if$k0;
  {
    // push quot @tocont
    LlackInstruction* inst = new PushWordLlackInst(word_tocont);
    word->instructions.push_back(inst);
  }
  {
    // tocont
    LlackInstruction* inst = new ToContLlackInst();
    word->instructions.push_back(inst);
  }
}

{
  // tocont
  Word* word = word_tocont;
  {
    // tocont
    LlackInstruction* inst = new ToContLlackInst();
    word->instructions.push_back(inst);
  }
}
  
  /// Running

  Module *module = new Module("module");
  module->setDataLayout("e-p:32:32:32-i1:8:8-i8:8:8-i16:16:16-i32:32:32-i64:32:64-f32:32:32-f64:32:64-v64:64:64-v128:128:128-a0:0:64-f80:128:128");
  module->setTargetTriple("i386-apple-darwin8");
  
  ExistingModuleProvider emp(module);
  ExecutionEngine *ee = ExecutionEngine::create(&emp, false);
  
  
  /*{
    const int dataIdx = 0;
    //const StructType *vmStateTy = VMOp::vmStateType();
    
    const Type* stackTy = PointerType::getUnqual(Type::Int8Ty);
    //const Type* instructionPtrTy = Type::Int32Ty;
    const StructType* vmStateTy = StructType::get(
			 stackTy, // data
			 stackTy, // retain
			 stackTy, // return
			 //instructionPtrTy,
			 NULL
			 );
    
    //const Type* ty = Type::Int32Ty; // stub
    
    std::vector<const Type*> argTypes;
    argTypes.push_back(PointerType::getUnqual(vmStateTy));
    //argTypes.push_back(ty);
    FunctionType *funcTy = FunctionType::get(Type::VoidTy, argTypes, false);
    Function *func = Function::Create(funcTy, Function::ExternalLinkage, "", module);
    
    Function::arg_iterator args = func->arg_begin();
    Value* vmStatePtr = args++;
    vmStatePtr->setName("vmStatePtr");
    //Value* val = args++;
    //val->setName("val");
    
    BasicBlock *bb = BasicBlock::Create("entry", func);
    IRBuilder builder;
    builder.SetInsertPoint(bb);
    
    Value *dataSP = builder.CreateStructGEP(vmStatePtr, dataIdx);
    //VMOp::push(td, builder, dataSP, val);
    
    builder.CreateRetVoid();
  }*/
  
  
  
  VMState *vmState = newVMState();

  push(&vmState->cont, (Word*) NULL);  
  printf("pushing main onto cont: %p\n", (void*) word_main);
  push(&vmState->cont, word_main);

  for (;;) {
    dumpVMState(vmState);

    Word* cont = pop<Word*>(&vmState->cont);
    if (cont == NULL) break;
    printf("==== cont: %s (%p) ====\n", cont->name.c_str(), (void*) cont);
    
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
    
    Function::arg_iterator args = func->arg_begin();
    Value* vmStatePtr = args++;
    vmStatePtr->setName("vmStatePtr");

    BasicBlock *bb = BasicBlock::Create("entry", func);
    IRBuilder builder;
    builder.SetInsertPoint(bb);
    
    VMCodeGenInterface* cgi = new SimpleVMCodeGenInterface(module, ee->getTargetData(), vmStatePtr, &builder);
    for (std::vector<LlackInstruction*>::iterator iter = cont->instructions.begin(); iter < cont->instructions.end(); ++iter) {
      // Instruction generation
    
      LlackInstruction* inst = *iter;
      printf("Generating instruction code.\n");
      //Value *dataSP = builder.CreateStructGEP(vmStatePtr, 0);
      inst->codeGen(cgi);

      // Pointer to dumping Code
      
      //ConstantInt* intValue = ConstantInt::get(Type::Int32Ty, (int) &dumpVMState, false);
      //Value* dumpVMStatePtr = builder.CreateIntToPtr(intValue, PointerType::getUnqual(funcTy));
      //builder.CreateCall(dumpVMStatePtr, vmStatePtr);
    }    

    builder.CreateRetVoid();

    // Run Function.
    
    std::vector<GenericValue> argValues(1);
    argValues[0].PointerVal = vmState;
    printf("Running code.\n");
    ee->runFunction(func, argValues);
    ee->freeMachineCodeForFunction(func);
  }
  
  freeVMState(vmState);
  
  return 0;
}

