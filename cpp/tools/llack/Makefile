##===- projects/llack/tools/llack/Makefile ---------------*- Makefile -*-===##

#
# Indicate where we are relative to the top of the source tree.
#
LEVEL=../..

#
# Give the name of the tool.
#
TOOLNAME=Llack

#
# List libraries that we'll need
# We use LIBS because llack is a dynamic library.
#
USEDLIBS = llack.a

LINK_COMPONENTS := jit bitwriter native interpreter

#LIBS = /Users/rich/Documents/Tamarin/llvm-2.3/build/Release/lib/LLVMSparc.o /Users/rich/Documents/Tamarin/llvm-2.3/build/Release/lib/LLVMPowerPC.o /Users/rich/Documents/Tamarin/llvm-2.3/build/Release/lib/LLVMMSIL.o /Users/rich/Documents/Tamarin/llvm-2.3/build/Release/lib/LLVMMips.o -lLLVMLinker -lLLVMipo /Users/rich/Documents/Tamarin/llvm-2.3/build/Release/lib/LLVMInterpreter.o -lLLVMInstrumentation /Users/rich/Documents/Tamarin/llvm-2.3/build/Release/lib/LLVMIA64.o /Users/rich/Documents/Tamarin/llvm-2.3/build/Release/lib/LLVMExecutionEngine.o /Users/rich/Documents/Tamarin/llvm-2.3/build/Release/lib/LLVMJIT.o -lLLVMDebugger /Users/rich/Documents/Tamarin/llvm-2.3/build/Release/lib/LLVMCppBackend.o /Users/rich/Documents/Tamarin/llvm-2.3/build/Release/lib/LLVMCellSPU.o /Users/rich/Documents/Tamarin/llvm-2.3/build/Release/lib/LLVMCBackend.o -lLLVMBitWriter /Users/rich/Documents/Tamarin/llvm-2.3/build/Release/lib/LLVMX86.o -lLLVMAsmParser /Users/rich/Documents/Tamarin/llvm-2.3/build/Release/lib/LLVMARM.o -lLLVMArchive -lLLVMBitReader /Users/rich/Documents/Tamarin/llvm-2.3/build/Release/lib/LLVMAlpha.o -lLLVMSelectionDAG -lLLVMCodeGen -lLLVMScalarOpts -lLLVMTransformUtils -lLLVMipa -lLLVMAnalysis -lLLVMTarget -lLLVMCore -lLLVMSupport -lLLVMSystem

#
# Include Makefile.common so we know what to do.
#
include $(LEVEL)/Makefile.common

