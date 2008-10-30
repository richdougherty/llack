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
Word* word_main$q21 = Word::Create("main$q21", mod);
Word* word_times$i32 = Word::Create("times$i32", mod);
Word* word_factorial$i32 = Word::Create("factorial$i32", mod);
Word* word_main$q21$k22 = Word::Create("main$q21$k22", mod);
Word* word_dup$i32 = Word::Create("dup$i32", mod);
Word* word_times$k19$i32 = Word::Create("times$k19$i32", mod);
Word* word_factorial_accum$i32 = Word::Create("factorial_accum$i32", mod);
Word* word_swap$i32_i32 = Word::Create("swap$i32_i32", mod);
Word* word_factorial_accum$k8$i32 = Word::Create("factorial_accum$k8$i32", mod);
Word* word_factorial_accum$k9$i32 = Word::Create("factorial_accum$k9$i32", mod);
Word* word_drop$i32 = Word::Create("drop$i32", mod);
Word* word_icmp$sle_i32 = Word::Create("icmp$sle_i32", mod);
Word* word_times$k20$i32 = Word::Create("times$k20$i32", mod);
Word* word_times$q11$i32 = Word::Create("times$q11$i32", mod);
Word* word_times$q13$i32 = Word::Create("times$q13$i32", mod);
Word* word_if = Word::Create("if", mod);
Word* word_sub$i32 = Word::Create("sub$i32", mod);
Word* word_times$q13$k14$i32 = Word::Create("times$q13$k14$i32", mod);
Word* word_times$q11$k12 = Word::Create("times$q11$k12", mod);
Word* word_drop$quot = Word::Create("drop$quot", mod);
Word* word_factorial_accum$k10$i32 = Word::Create("factorial_accum$k10$i32", mod);
Word* word_swap$quot_i32 = Word::Create("swap$quot_i32", mod);
Word* word_times$q13$k15$i32 = Word::Create("times$q13$k15$i32", mod);
Word* word_dup$quot = Word::Create("dup$quot", mod);
Word* word_times$q13$k16$i32 = Word::Create("times$q13$k16$i32", mod);
Word* word_apply = Word::Create("apply", mod);
Word* word_times$q13$k17$i32 = Word::Create("times$q13$k17$i32", mod);
Word* word_swap$i32_quot = Word::Create("swap$i32_quot", mod);
Word* word_times$q13$k18$i32 = Word::Create("times$q13$k18$i32", mod);
Word* word_select$quot = Word::Create("select$quot", mod);
Word* word_if$k0 = Word::Create("if$k0", mod);
Word* word_tocont = Word::Create("tocont", mod);
Word* word_factorial_accum$q1$i32 = Word::Create("factorial_accum$q1$i32", mod);
Word* word_factorial_accum$q2$i32 = Word::Create("factorial_accum$q2$i32", mod);
Word* word_factorial_accum$q2$k3$i32 = Word::Create("factorial_accum$q2$k3$i32", mod);
Word* word_factorial_accum$q2$k4$i32 = Word::Create("factorial_accum$q2$k4$i32", mod);
Word* word_factorial_accum$q2$k5$i32 = Word::Create("factorial_accum$q2$k5$i32", mod);
Word* word_rot$i32_i32_i32 = Word::Create("rot$i32_i32_i32", mod);
Word* word_factorial_accum$q2$k6$i32 = Word::Create("factorial_accum$q2$k6$i32", mod);
Word* word_mul$i32 = Word::Create("mul$i32", mod);
Word* word_factorial_accum$q2$k7$i32 = Word::Create("factorial_accum$q2$k7$i32", mod);

{
  // main
  Word* word = word_main;
  {
    // push quot @main$q21
    LlackInstruction* inst = new LookupLlackInst(word_main$q21);
    word->instructions.push_back(inst);
  }
  {
    // push i32 3
    LlackInstruction* inst = new PushLlackInst(new LlvmLlackValue(ConstantInt::get(APInt(32,  "3", 10))));
    word->instructions.push_back(inst);
  }
  {
    // push quot @times$i32
    LlackInstruction* inst = new LookupLlackInst(word_times$i32);
    word->instructions.push_back(inst);
  }
  {
    // tocont
    LlackInstruction* inst = new ToContLlackInst();
    word->instructions.push_back(inst);
  }
}

{
  // main$q21
  Word* word = word_main$q21;
  {
    // push i32 5
    LlackInstruction* inst = new PushLlackInst(new LlvmLlackValue(ConstantInt::get(APInt(32,  "5", 10))));
    word->instructions.push_back(inst);
  }
  {
    // push quot @factorial$i32
    LlackInstruction* inst = new LookupLlackInst(word_factorial$i32);
    word->instructions.push_back(inst);
  }
  {
    // push quot @main$q21$k22
    LlackInstruction* inst = new LookupLlackInst(word_main$q21$k22);
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
  // times$i32
  Word* word = word_times$i32;
  {
    // push quot @dup$i32
    LlackInstruction* inst = new LookupLlackInst(word_dup$i32);
    word->instructions.push_back(inst);
  }
  {
    // push quot @times$k19$i32
    LlackInstruction* inst = new LookupLlackInst(word_times$k19$i32);
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
  // factorial$i32
  Word* word = word_factorial$i32;
  {
    // push i32 1
    LlackInstruction* inst = new PushLlackInst(new LlvmLlackValue(ConstantInt::get(APInt(32,  "1", 10))));
    word->instructions.push_back(inst);
  }
  {
    // push quot @factorial_accum$i32
    LlackInstruction* inst = new LookupLlackInst(word_factorial_accum$i32);
    word->instructions.push_back(inst);
  }
  {
    // tocont
    LlackInstruction* inst = new ToContLlackInst();
    word->instructions.push_back(inst);
  }
}

{
  // main$q21$k22
  Word* word = word_main$q21$k22;
  {
    // push quot @drop$i32
    LlackInstruction* inst = new LookupLlackInst(word_drop$i32);
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
  // times$k19$i32
  Word* word = word_times$k19$i32;
  {
    // push i32 1
    LlackInstruction* inst = new PushLlackInst(new LlvmLlackValue(ConstantInt::get(APInt(32,  "1", 10))));
    word->instructions.push_back(inst);
  }
  {
    // push quot @icmp$sle_i32
    LlackInstruction* inst = new LookupLlackInst(word_icmp$sle_i32);
    word->instructions.push_back(inst);
  }
  {
    // push quot @times$k20$i32
    LlackInstruction* inst = new LookupLlackInst(word_times$k20$i32);
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
  // factorial_accum$i32
  Word* word = word_factorial_accum$i32;
  {
    // push quot @swap$i32_i32
    LlackInstruction* inst = new LookupLlackInst(word_swap$i32_i32);
    word->instructions.push_back(inst);
  }
  {
    // push quot @factorial_accum$k8$i32
    LlackInstruction* inst = new LookupLlackInst(word_factorial_accum$k8$i32);
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
    LlackInstruction* inst = new LookupLlackInst(word_dup$i32);
    word->instructions.push_back(inst);
  }
  {
    // push quot @factorial_accum$k9$i32
    LlackInstruction* inst = new LookupLlackInst(word_factorial_accum$k9$i32);
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
  // factorial_accum$k9$i32
  Word* word = word_factorial_accum$k9$i32;
  {
    // push i32 1
    LlackInstruction* inst = new PushLlackInst(new LlvmLlackValue(ConstantInt::get(APInt(32,  "1", 10))));
    word->instructions.push_back(inst);
  }
  {
    // push quot @icmp$sle_i32
    LlackInstruction* inst = new LookupLlackInst(word_icmp$sle_i32);
    word->instructions.push_back(inst);
  }
  {
    // push quot @factorial_accum$k10$i32
    LlackInstruction* inst = new LookupLlackInst(word_factorial_accum$k10$i32);
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
  // icmp$sle_i32
  Word* word = word_icmp$sle_i32;
  {
    // icmp sle i32
    LlackInstruction* inst = new ICmpLlackInst(ICmpInst::ICMP_SLE, new LlvmLlackType(IntegerType::get(32)));
    word->instructions.push_back(inst);
  }
}

{
  // times$k20$i32
  Word* word = word_times$k20$i32;
  {
    // push quot @times$q11$i32
    LlackInstruction* inst = new LookupLlackInst(word_times$q11$i32);
    word->instructions.push_back(inst);
  }
  {
    // push quot @times$q13$i32
    LlackInstruction* inst = new LookupLlackInst(word_times$q13$i32);
    word->instructions.push_back(inst);
  }
  {
    // push quot @if
    LlackInstruction* inst = new LookupLlackInst(word_if);
    word->instructions.push_back(inst);
  }
  {
    // tocont
    LlackInstruction* inst = new ToContLlackInst();
    word->instructions.push_back(inst);
  }
}

{
  // times$q11$i32
  Word* word = word_times$q11$i32;
  {
    // push quot @drop$i32
    LlackInstruction* inst = new LookupLlackInst(word_drop$i32);
    word->instructions.push_back(inst);
  }
  {
    // push quot @times$q11$k12
    LlackInstruction* inst = new LookupLlackInst(word_times$q11$k12);
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
  // times$q13$i32
  Word* word = word_times$q13$i32;
  {
    // push i32 1
    LlackInstruction* inst = new PushLlackInst(new LlvmLlackValue(ConstantInt::get(APInt(32,  "1", 10))));
    word->instructions.push_back(inst);
  }
  {
    // push quot @sub$i32
    LlackInstruction* inst = new LookupLlackInst(word_sub$i32);
    word->instructions.push_back(inst);
  }
  {
    // push quot @times$q13$k14$i32
    LlackInstruction* inst = new LookupLlackInst(word_times$q13$k14$i32);
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
    LlackInstruction* inst = new LookupLlackInst(word_select$quot);
    word->instructions.push_back(inst);
  }
  {
    // push quot @if$k0
    LlackInstruction* inst = new LookupLlackInst(word_if$k0);
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
    LlackInstruction* inst = new SubLlackInst(new LlvmLlackType(IntegerType::get(32)));
    word->instructions.push_back(inst);
  }
}

{
  // times$q13$k14$i32
  Word* word = word_times$q13$k14$i32;
  {
    // push quot @swap$quot_i32
    LlackInstruction* inst = new LookupLlackInst(word_swap$quot_i32);
    word->instructions.push_back(inst);
  }
  {
    // push quot @times$q13$k15$i32
    LlackInstruction* inst = new LookupLlackInst(word_times$q13$k15$i32);
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
  // times$q11$k12
  Word* word = word_times$q11$k12;
  {
    // push quot @drop$quot
    LlackInstruction* inst = new LookupLlackInst(word_drop$quot);
    word->instructions.push_back(inst);
  }
  {
    // tocont
    LlackInstruction* inst = new ToContLlackInst();
    word->instructions.push_back(inst);
  }
}

{
  // drop$quot
  Word* word = word_drop$quot;
  {
    // shuffle quot
    std::vector<LlackType*> consumption;
    consumption.push_back(new LocationType());
    std::vector<int> production;
    LlackInstruction* inst = new ShuffleLlackInst(consumption, production);
    word->instructions.push_back(inst);
  }
}

{
  // factorial_accum$k10$i32
  Word* word = word_factorial_accum$k10$i32;
  {
    // push quot @factorial_accum$q1$i32
    LlackInstruction* inst = new LookupLlackInst(word_factorial_accum$q1$i32);
    word->instructions.push_back(inst);
  }
  {
    // push quot @factorial_accum$q2$i32
    LlackInstruction* inst = new LookupLlackInst(word_factorial_accum$q2$i32);
    word->instructions.push_back(inst);
  }
  {
    // push quot @if
    LlackInstruction* inst = new LookupLlackInst(word_if);
    word->instructions.push_back(inst);
  }
  {
    // tocont
    LlackInstruction* inst = new ToContLlackInst();
    word->instructions.push_back(inst);
  }
}

{
  // swap$quot_i32
  Word* word = word_swap$quot_i32;
  {
    // shuffle quot i32 1 0
    std::vector<LlackType*> consumption;
    consumption.push_back(new LocationType());
    consumption.push_back(new LlvmLlackType(IntegerType::get(32)));
    std::vector<int> production;
    production.push_back(1);
    production.push_back(0);
    LlackInstruction* inst = new ShuffleLlackInst(consumption, production);
    word->instructions.push_back(inst);
  }
}

{
  // times$q13$k15$i32
  Word* word = word_times$q13$k15$i32;
  {
    // push quot @dup$quot
    LlackInstruction* inst = new LookupLlackInst(word_dup$quot);
    word->instructions.push_back(inst);
  }
  {
    // push quot @times$q13$k16$i32
    LlackInstruction* inst = new LookupLlackInst(word_times$q13$k16$i32);
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
  // dup$quot
  Word* word = word_dup$quot;
  {
    // shuffle quot 0 0
    std::vector<LlackType*> consumption;
    consumption.push_back(new LocationType());
    std::vector<int> production;
    production.push_back(0);
    production.push_back(0);
    LlackInstruction* inst = new ShuffleLlackInst(consumption, production);
    word->instructions.push_back(inst);
  }
}

{
  // times$q13$k16$i32
  Word* word = word_times$q13$k16$i32;
  {
    // push quot @apply
    LlackInstruction* inst = new LookupLlackInst(word_apply);
    word->instructions.push_back(inst);
  }
  {
    // push quot @times$q13$k17$i32
    LlackInstruction* inst = new LookupLlackInst(word_times$q13$k17$i32);
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
  // apply
  Word* word = word_apply;
  {
    // push quot @tocont
    LlackInstruction* inst = new LookupLlackInst(word_tocont);
    word->instructions.push_back(inst);
  }
  {
    // tocont
    LlackInstruction* inst = new ToContLlackInst();
    word->instructions.push_back(inst);
  }
}

{
  // times$q13$k17$i32
  Word* word = word_times$q13$k17$i32;
  {
    // push quot @swap$i32_quot
    LlackInstruction* inst = new LookupLlackInst(word_swap$i32_quot);
    word->instructions.push_back(inst);
  }
  {
    // push quot @times$q13$k18$i32
    LlackInstruction* inst = new LookupLlackInst(word_times$q13$k18$i32);
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
  // swap$i32_quot
  Word* word = word_swap$i32_quot;
  {
    // shuffle i32 quot 1 0
    std::vector<LlackType*> consumption;
    consumption.push_back(new LlvmLlackType(IntegerType::get(32)));
    consumption.push_back(new LocationType());
    std::vector<int> production;
    production.push_back(1);
    production.push_back(0);
    LlackInstruction* inst = new ShuffleLlackInst(consumption, production);
    word->instructions.push_back(inst);
  }
}

{
  // times$q13$k18$i32
  Word* word = word_times$q13$k18$i32;
  {
    // push quot @times$i32
    LlackInstruction* inst = new LookupLlackInst(word_times$i32);
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
    LlackInstruction* inst = new SelectLlackInst(new LocationType());
    word->instructions.push_back(inst);
  }
}

{
  // if$k0
  Word* word = word_if$k0;
  {
    // push quot @tocont
    LlackInstruction* inst = new LookupLlackInst(word_tocont);
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

{
  // factorial_accum$q1$i32
  Word* word = word_factorial_accum$q1$i32;
  {
    // push quot @drop$i32
    LlackInstruction* inst = new LookupLlackInst(word_drop$i32);
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
    LlackInstruction* inst = new LookupLlackInst(word_dup$i32);
    word->instructions.push_back(inst);
  }
  {
    // push quot @factorial_accum$q2$k3$i32
    LlackInstruction* inst = new LookupLlackInst(word_factorial_accum$q2$k3$i32);
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
  // factorial_accum$q2$k3$i32
  Word* word = word_factorial_accum$q2$k3$i32;
  {
    // push i32 1
    LlackInstruction* inst = new PushLlackInst(new LlvmLlackValue(ConstantInt::get(APInt(32,  "1", 10))));
    word->instructions.push_back(inst);
  }
  {
    // push quot @sub$i32
    LlackInstruction* inst = new LookupLlackInst(word_sub$i32);
    word->instructions.push_back(inst);
  }
  {
    // push quot @factorial_accum$q2$k4$i32
    LlackInstruction* inst = new LookupLlackInst(word_factorial_accum$q2$k4$i32);
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
  // factorial_accum$q2$k4$i32
  Word* word = word_factorial_accum$q2$k4$i32;
  {
    // push quot @swap$i32_i32
    LlackInstruction* inst = new LookupLlackInst(word_swap$i32_i32);
    word->instructions.push_back(inst);
  }
  {
    // push quot @factorial_accum$q2$k5$i32
    LlackInstruction* inst = new LookupLlackInst(word_factorial_accum$q2$k5$i32);
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
    LlackInstruction* inst = new LookupLlackInst(word_rot$i32_i32_i32);
    word->instructions.push_back(inst);
  }
  {
    // push quot @factorial_accum$q2$k6$i32
    LlackInstruction* inst = new LookupLlackInst(word_factorial_accum$q2$k6$i32);
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
    LlackInstruction* inst = new LookupLlackInst(word_mul$i32);
    word->instructions.push_back(inst);
  }
  {
    // push quot @factorial_accum$q2$k7$i32
    LlackInstruction* inst = new LookupLlackInst(word_factorial_accum$q2$k7$i32);
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
    LlackInstruction* inst = new MulLlackInst(new LlvmLlackType(IntegerType::get(32)));
    word->instructions.push_back(inst);
  }
}

{
  // factorial_accum$q2$k7$i32
  Word* word = word_factorial_accum$q2$k7$i32;
  {
    // push quot @factorial_accum$i32
    LlackInstruction* inst = new LookupLlackInst(word_factorial_accum$i32);
    word->instructions.push_back(inst);
  }
  {
    // tocont
    LlackInstruction* inst = new ToContLlackInst();
    word->instructions.push_back(inst);
  }
}
return mod;
}