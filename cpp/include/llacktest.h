/*
 * File: llacktest.h
 *
 *      This is a llack header file that is global to the entire project.
 *      It is located here so that everyone will find it.
 */

#ifndef LLACK_LLACKTEST_H
#define LLACK_LLACKTEST_H

#include "llvm/DerivedTypes.h"
#include "llvm/GlobalValue.h"
#include "llvm/Module.h"
#include "llvm/Analysis/Verifier.h"
#include "llvm/Support/IRBuilder.h"
#include "llvm/Target/TargetMachine.h"
#include "llvm/ExecutionEngine/ExecutionEngine.h"
#include "llack.h"
#include <cstdio>
#include <string>
#include <map>
#include <vector>

using namespace llvm;

extern LlackModule* createFactorialModule();

#endif