/*
 * File: llacktest.c
 *
 * Description:
 *  This is a llack source file for a library.  It helps to demonstrate
 *  how to setup a project that uses the LLVM build system, header files,
 *  and libraries.
 */

#include <stdio.h>
#include <stdlib.h>

#include "llacktest.h"

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

LlackModule* createFactorialModule() {
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
      LlackInstruction* inst = new PushLlackInst(new LlvmLlackValue(ConstantInt::get(APInt(32,  "5", 10))));
      word->instructions.push_back(inst);
    }
    {
      // push quot @factorial$i32
      LlackInstruction* inst = new PushLlackInst(word_factorial$i32);
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
      LlackInstruction* inst = new PushLlackInst(new LlvmLlackValue(ConstantInt::get(APInt(32,  "1", 10))));
      word->instructions.push_back(inst);
    }
    {
      // push quot @factorial_accum$i32
      LlackInstruction* inst = new PushLlackInst(word_factorial_accum$i32);
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
      LlackInstruction* inst = new PushLlackInst(word_swap$i32_i32);
      word->instructions.push_back(inst);
    }
    {
      // push quot @factorial_accum$k8$i32
      LlackInstruction* inst = new PushLlackInst(word_factorial_accum$k8$i32);
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
      std::vector<LlackType*> consumption;
      consumption.push_back(new LlvmLlackType(IntegerType::get(32)));
      consumption.push_back(new LlvmLlackType(IntegerType::get(32)));
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
      LlackInstruction* inst = new PushLlackInst(word_dup$i32);
      word->instructions.push_back(inst);
    }
    {
      // push quot @factorial_accum$k9$i32
      LlackInstruction* inst = new PushLlackInst(word_factorial_accum$k9$i32);
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
      std::vector<LlackType*> consumption;
      consumption.push_back(new LlvmLlackType(IntegerType::get(32)));
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
      LlackInstruction* inst = new PushLlackInst(new LlvmLlackValue(ConstantInt::get(APInt(32,  "1", 10))));
      word->instructions.push_back(inst);
    }
    {
      // push quot @icmp$sle_i32
      LlackInstruction* inst = new PushLlackInst(word_icmp$sle_i32);
      word->instructions.push_back(inst);
    }
    {
      // push quot @factorial_accum$k10$i32
      LlackInstruction* inst = new PushLlackInst(word_factorial_accum$k10$i32);
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
    LlackInstruction* inst = new PushLlackInst(word_factorial_accum$q1$i32);
    word->instructions.push_back(inst);
    }
    {
    // push quot @factorial_accum$q2$i32
    LlackInstruction* inst = new PushLlackInst(word_factorial_accum$q2$i32);
    word->instructions.push_back(inst);
    }
    {
    // push quot @if
    LlackInstruction* inst = new PushLlackInst(word_if);
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
    LlackInstruction* inst = new PushLlackInst(word_drop$i32);
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
    LlackInstruction* inst = new PushLlackInst(word_dup$i32);
    word->instructions.push_back(inst);
    }
    {
    // push quot @factorial_accum$q2$k3$i32
    LlackInstruction* inst = new PushLlackInst(word_factorial_accum$q2$k3$i32);
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
    LlackInstruction* inst = new PushLlackInst(word_select$quot);
    word->instructions.push_back(inst);
    }
    {
    // push quot @if$k0
    LlackInstruction* inst = new PushLlackInst(word_if$k0);
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
    std::vector<LlackType*> consumption;
    consumption.push_back(new LlvmLlackType(IntegerType::get(32)));
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
    LlackInstruction* inst = new PushLlackInst(new LlvmLlackValue(ConstantInt::get(APInt(32,  "1", 10))));
    word->instructions.push_back(inst);
    }
    {
    // push quot @sub$i32
    LlackInstruction* inst = new PushLlackInst(word_sub$i32);
    word->instructions.push_back(inst);
    }
    {
    // push quot @factorial_accum$q2$k4$i32
    LlackInstruction* inst = new PushLlackInst(word_factorial_accum$q2$k4$i32);
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
    LlackInstruction* inst = new PushLlackInst(word_swap$i32_i32);
    word->instructions.push_back(inst);
    }
    {
    // push quot @factorial_accum$q2$k5$i32
    LlackInstruction* inst = new PushLlackInst(word_factorial_accum$q2$k5$i32);
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
    LlackInstruction* inst = new PushLlackInst(word_rot$i32_i32_i32);
    word->instructions.push_back(inst);
    }
    {
    // push quot @factorial_accum$q2$k6$i32
    LlackInstruction* inst = new PushLlackInst(word_factorial_accum$q2$k6$i32);
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
    std::vector<LlackType*> consumption;
    consumption.push_back(new LlvmLlackType(IntegerType::get(32)));
    consumption.push_back(new LlvmLlackType(IntegerType::get(32)));
    consumption.push_back(new LlvmLlackType(IntegerType::get(32)));
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
    LlackInstruction* inst = new PushLlackInst(word_mul$i32);
    word->instructions.push_back(inst);
    }
    {
    // push quot @factorial_accum$q2$k7$i32
    LlackInstruction* inst = new PushLlackInst(word_factorial_accum$q2$k7$i32);
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
    LlackInstruction* inst = new PushLlackInst(word_factorial_accum$i32);
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
    LlackInstruction* inst = new SelectLlackInst(new LlvmLlackType(PointerType::getUnqual(Type::Int8Ty)));
    word->instructions.push_back(inst);
    }
  }

  {
    // if$k0
    Word* word = word_if$k0;
    {
    // push quot @tocont
    LlackInstruction* inst = new PushLlackInst(word_tocont);
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
  return mod;
}
