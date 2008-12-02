#ifdef DONT_COMPILE

//===-- Core.cpp ----------------------------------------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file implements the C bindings for libLLVMCore.a, which implements
// the LLVM intermediate representation.
//
//===----------------------------------------------------------------------===//

#include "llack-c/Core.h"
#include "llack/Constants.h"
#include "llack/DerivedTypes.h"
#include "llack/GlobalVariable.h"
#include "llack/TypeSymbolTable.h"
#include "llack/ModuleProvider.h"
#include "llack/Support/CallSite.h"
#include "llvm/Bitcode/ReaderWriter.h"
#include "llvm/Support/MemoryBuffer.h"
#include <cassert>
#include <cstdlib>
#include <cstring>

using namespace llack;


/*===-- Error handling ----------------------------------------------------===*/

void LlackDisposeMessage(char *Message) {
  free(Message);
}


/*===-- Operations on modules ---------------------------------------------===*/

LlackModuleRef LlackModuleCreateWithName(const char *ModuleID) {
  return wrap(new Module(ModuleID));
}

void LlackDisposeModule(LlackModuleRef M) {
  delete unwrap(M);
}

/*--.. Data layout .........................................................--*/
const char * LlackGetDataLayout(LlackModuleRef M) {
  return unwrap(M)->getDataLayout().c_str();
}

void LlackSetDataLayout(LlackModuleRef M, const char *Triple) {
  unwrap(M)->setDataLayout(Triple);
}

/*--.. Target triple .......................................................--*/
const char * LlackGetTarget(LlackModuleRef M) {
  return unwrap(M)->getTargetTriple().c_str();
}

void LlackSetTarget(LlackModuleRef M, const char *Triple) {
  unwrap(M)->setTargetTriple(Triple);
}

/*--.. Type names ..........................................................--*/
int LlackAddTypeName(LlackModuleRef M, const char *Name, LlackTypeRef Ty) {
  return unwrap(M)->addTypeName(Name, unwrap(Ty));
}

void LlackDeleteTypeName(LlackModuleRef M, const char *Name) {
  std::string N(Name);
  
  TypeSymbolTable &TST = unwrap(M)->getTypeSymbolTable();
  for (TypeSymbolTable::iterator I = TST.begin(), E = TST.end(); I != E; ++I)
    if (I->first == N)
      TST.remove(I);
}

void LlackDumpModule(LlackModuleRef M) {
  unwrap(M)->dump();
}


/*===-- Operations on types -----------------------------------------------===*/

/*--.. Operations on all types (mostly) ....................................--*/

LlackTypeKind LlackGetTypeKind(LlackTypeRef Ty) {
  return static_cast<LlackTypeKind>(unwrap(Ty)->getTypeID());
}

/*--.. Operations on integer types .........................................--*/

LlackTypeRef LlackInt1Type(void)  { return (LlackTypeRef) Type::Int1Ty;  }
LlackTypeRef LlackInt8Type(void)  { return (LlackTypeRef) Type::Int8Ty;  }
LlackTypeRef LlackInt16Type(void) { return (LlackTypeRef) Type::Int16Ty; }
LlackTypeRef LlackInt32Type(void) { return (LlackTypeRef) Type::Int32Ty; }
LlackTypeRef LlackInt64Type(void) { return (LlackTypeRef) Type::Int64Ty; }

LlackTypeRef LlackIntType(unsigned NumBits) {
  return wrap(IntegerType::get(NumBits));
}

unsigned LlackGetIntTypeWidth(LlackTypeRef IntegerTy) {
  return unwrap<IntegerType>(IntegerTy)->getBitWidth();
}

/*--.. Operations on real types ............................................--*/

LlackTypeRef LlackFloatType(void)    { return (LlackTypeRef) Type::FloatTy;     }
LlackTypeRef LlackDoubleType(void)   { return (LlackTypeRef) Type::DoubleTy;    }
LlackTypeRef LlackX86FP80Type(void)  { return (LlackTypeRef) Type::X86_FP80Ty;  }
LlackTypeRef LlackFP128Type(void)    { return (LlackTypeRef) Type::FP128Ty;     }
LlackTypeRef LlackPPCFP128Type(void) { return (LlackTypeRef) Type::PPC_FP128Ty; }

/*--.. Operations on function types ........................................--*/

LlackTypeRef LlackFunctionType(LlackTypeRef ReturnType,
                             LlackTypeRef *ParamTypes, unsigned ParamCount,
                             int IsVarArg) {
  std::vector<const Type*> Tys;
  for (LlackTypeRef *I = ParamTypes, *E = ParamTypes + ParamCount; I != E; ++I)
    Tys.push_back(unwrap(*I));
  
  return wrap(FunctionType::get(unwrap(ReturnType), Tys, IsVarArg != 0));
}

int LlackIsFunctionVarArg(LlackTypeRef FunctionTy) {
  return unwrap<FunctionType>(FunctionTy)->isVarArg();
}

LlackTypeRef LlackGetReturnType(LlackTypeRef FunctionTy) {
  return wrap(unwrap<FunctionType>(FunctionTy)->getReturnType());
}

unsigned LlackCountParamTypes(LlackTypeRef FunctionTy) {
  return unwrap<FunctionType>(FunctionTy)->getNumParams();
}

void LlackGetParamTypes(LlackTypeRef FunctionTy, LlackTypeRef *Dest) {
  FunctionType *Ty = unwrap<FunctionType>(FunctionTy);
  for (FunctionType::param_iterator I = Ty->param_begin(),
                                    E = Ty->param_end(); I != E; ++I)
    *Dest++ = wrap(*I);
}

/*--.. Operations on struct types ..........................................--*/

LlackTypeRef LlackStructType(LlackTypeRef *ElementTypes,
                           unsigned ElementCount, int Packed) {
  std::vector<const Type*> Tys;
  for (LlackTypeRef *I = ElementTypes,
                   *E = ElementTypes + ElementCount; I != E; ++I)
    Tys.push_back(unwrap(*I));
  
  return wrap(StructType::get(Tys, Packed != 0));
}

unsigned LlackCountStructElementTypes(LlackTypeRef StructTy) {
  return unwrap<StructType>(StructTy)->getNumElements();
}

void LlackGetStructElementTypes(LlackTypeRef StructTy, LlackTypeRef *Dest) {
  StructType *Ty = unwrap<StructType>(StructTy);
  for (FunctionType::param_iterator I = Ty->element_begin(),
                                    E = Ty->element_end(); I != E; ++I)
    *Dest++ = wrap(*I);
}

int LlackIsPackedStruct(LlackTypeRef StructTy) {
  return unwrap<StructType>(StructTy)->isPacked();
}

/*--.. Operations on array, pointer, and vector types (sequence types) .....--*/

LlackTypeRef LlackArrayType(LlackTypeRef ElementType, unsigned ElementCount) {
  return wrap(ArrayType::get(unwrap(ElementType), ElementCount));
}

LlackTypeRef LlackPointerType(LlackTypeRef ElementType, unsigned AddressSpace) {
  return wrap(PointerType::get(unwrap(ElementType), AddressSpace));
}

LlackTypeRef LlackVectorType(LlackTypeRef ElementType, unsigned ElementCount) {
  return wrap(VectorType::get(unwrap(ElementType), ElementCount));
}

LlackTypeRef LlackGetElementType(LlackTypeRef Ty) {
  return wrap(unwrap<SequentialType>(Ty)->getElementType());
}

unsigned LlackGetArrayLength(LlackTypeRef ArrayTy) {
  return unwrap<ArrayType>(ArrayTy)->getNumElements();
}

unsigned LlackGetPointerAddressSpace(LlackTypeRef PointerTy) {
  return unwrap<PointerType>(PointerTy)->getAddressSpace();
}

unsigned LlackGetVectorSize(LlackTypeRef VectorTy) {
  return unwrap<VectorType>(VectorTy)->getNumElements();
}

/*--.. Operations on other types ...........................................--*/

LlackTypeRef LlackVoidType(void)  { return (LlackTypeRef) Type::VoidTy;  }
LlackTypeRef LlackLabelType(void) { return (LlackTypeRef) Type::LabelTy; }

LlackTypeRef LlackOpaqueType(void) {
  return wrap(llack::OpaqueType::get());
}

/*--.. Operations on type handles ..........................................--*/

LlackTypeHandleRef LlackCreateTypeHandle(LlackTypeRef PotentiallyAbstractTy) {
  return wrap(new PATypeHolder(unwrap(PotentiallyAbstractTy)));
}

void LlackDisposeTypeHandle(LlackTypeHandleRef TypeHandle) {
  delete unwrap(TypeHandle);
}

LlackTypeRef LlackResolveTypeHandle(LlackTypeHandleRef TypeHandle) {
  return wrap(unwrap(TypeHandle)->get());
}

void LlackRefineType(LlackTypeRef AbstractTy, LlackTypeRef ConcreteTy) {
  unwrap<DerivedType>(AbstractTy)->refineAbstractTypeTo(unwrap(ConcreteTy));
}


/*===-- Operations on values ----------------------------------------------===*/

/*--.. Operations on all values ............................................--*/

LlackTypeRef LlackTypeOf(LlackValueRef Val) {
  return wrap(unwrap(Val)->getType());
}

const char *LlackGetValueName(LlackValueRef Val) {
  return unwrap(Val)->getNameStart();
}

void LlackSetValueName(LlackValueRef Val, const char *Name) {
  unwrap(Val)->setName(Name);
}

void LlackDumpValue(LlackValueRef Val) {
  unwrap(Val)->dump();
}

/*--.. Operations on constants of any type .................................--*/

LlackValueRef LlackConstNull(LlackTypeRef Ty) {
  return wrap(Constant::getNullValue(unwrap(Ty)));
}

LlackValueRef LlackConstAllOnes(LlackTypeRef Ty) {
  return wrap(Constant::getAllOnesValue(unwrap(Ty)));
}

LlackValueRef LlackGetUndef(LlackTypeRef Ty) {
  return wrap(UndefValue::get(unwrap(Ty)));
}

int LlackIsConstant(LlackValueRef Ty) {
  return isa<Constant>(unwrap(Ty));
}

int LlackIsNull(LlackValueRef Val) {
  if (Constant *C = dyn_cast<Constant>(unwrap(Val)))
    return C->isNullValue();
  return false;
}

int LlackIsUndef(LlackValueRef Val) {
  return isa<UndefValue>(unwrap(Val));
}

/*--.. Operations on scalar constants ......................................--*/

LlackValueRef LlackConstInt(LlackTypeRef IntTy, unsigned long long N,
                          int SignExtend) {
  return wrap(ConstantInt::get(unwrap<IntegerType>(IntTy), N, SignExtend != 0));
}

static const fltSemantics &SemanticsForType(Type *Ty) {
  assert(Ty->isFloatingPoint() && "Type is not floating point!");
  if (Ty == Type::FloatTy)
    return APFloat::IEEEsingle;
  if (Ty == Type::DoubleTy)
    return APFloat::IEEEdouble;
  if (Ty == Type::X86_FP80Ty)
    return APFloat::x87DoubleExtended;
  if (Ty == Type::FP128Ty)
    return APFloat::IEEEquad;
  if (Ty == Type::PPC_FP128Ty)
    return APFloat::PPCDoubleDouble;
  return APFloat::Bogus;
}

LlackValueRef LlackConstReal(LlackTypeRef RealTy, double N) {
  APFloat APN(N);
  APN.convert(SemanticsForType(unwrap(RealTy)), APFloat::rmNearestTiesToEven);
  return wrap(ConstantFP::get(APN));
}

LlackValueRef LlackConstRealOfString(LlackTypeRef RealTy, const char *Text) {
  return wrap(ConstantFP::get(APFloat(SemanticsForType(unwrap(RealTy)), Text)));
}

/*--.. Operations on composite constants ...................................--*/

LlackValueRef LlackConstString(const char *Str, unsigned Length,
                             int DontNullTerminate) {
  /* Inverted the sense of AddNull because ', 0)' is a
     better mnemonic for null termination than ', 1)'. */
  return wrap(ConstantArray::get(std::string(Str, Length),
                                 DontNullTerminate == 0));
}

LlackValueRef LlackConstArray(LlackTypeRef ElementTy,
                            LlackValueRef *ConstantVals, unsigned Length) {
  return wrap(ConstantArray::get(ArrayType::get(unwrap(ElementTy), Length),
                                 unwrap<Constant>(ConstantVals, Length),
                                 Length));
}

LlackValueRef LlackConstStruct(LlackValueRef *ConstantVals, unsigned Count,
                             int Packed) {
  return wrap(ConstantStruct::get(unwrap<Constant>(ConstantVals, Count),
                                  Count, Packed != 0));
}

LlackValueRef LlackConstVector(LlackValueRef *ScalarConstantVals, unsigned Size) {
  return wrap(ConstantVector::get(unwrap<Constant>(ScalarConstantVals, Size),
                                  Size));
}

/*--.. Constant expressions ................................................--*/

LlackValueRef LlackSizeOf(LlackTypeRef Ty) {
  return wrap(ConstantExpr::getSizeOf(unwrap(Ty)));
}

LlackValueRef LlackConstNeg(LlackValueRef ConstantVal) {
  return wrap(ConstantExpr::getNeg(unwrap<Constant>(ConstantVal)));
}

LlackValueRef LlackConstNot(LlackValueRef ConstantVal) {
  return wrap(ConstantExpr::getNot(unwrap<Constant>(ConstantVal)));
}

LlackValueRef LlackConstAdd(LlackValueRef LHSConstant, LlackValueRef RHSConstant) {
  return wrap(ConstantExpr::getAdd(unwrap<Constant>(LHSConstant),
                                   unwrap<Constant>(RHSConstant)));
}

LlackValueRef LlackConstSub(LlackValueRef LHSConstant, LlackValueRef RHSConstant) {
  return wrap(ConstantExpr::getSub(unwrap<Constant>(LHSConstant),
                                   unwrap<Constant>(RHSConstant)));
}

LlackValueRef LlackConstMul(LlackValueRef LHSConstant, LlackValueRef RHSConstant) {
  return wrap(ConstantExpr::getMul(unwrap<Constant>(LHSConstant),
                                   unwrap<Constant>(RHSConstant)));
}

LlackValueRef LlackConstUDiv(LlackValueRef LHSConstant, LlackValueRef RHSConstant) {
  return wrap(ConstantExpr::getUDiv(unwrap<Constant>(LHSConstant),
                                    unwrap<Constant>(RHSConstant)));
}

LlackValueRef LlackConstSDiv(LlackValueRef LHSConstant, LlackValueRef RHSConstant) {
  return wrap(ConstantExpr::getSDiv(unwrap<Constant>(LHSConstant),
                                    unwrap<Constant>(RHSConstant)));
}

LlackValueRef LlackConstFDiv(LlackValueRef LHSConstant, LlackValueRef RHSConstant) {
  return wrap(ConstantExpr::getFDiv(unwrap<Constant>(LHSConstant),
                                    unwrap<Constant>(RHSConstant)));
}

LlackValueRef LlackConstURem(LlackValueRef LHSConstant, LlackValueRef RHSConstant) {
  return wrap(ConstantExpr::getURem(unwrap<Constant>(LHSConstant),
                                    unwrap<Constant>(RHSConstant)));
}

LlackValueRef LlackConstSRem(LlackValueRef LHSConstant, LlackValueRef RHSConstant) {
  return wrap(ConstantExpr::getSRem(unwrap<Constant>(LHSConstant),
                                    unwrap<Constant>(RHSConstant)));
}

LlackValueRef LlackConstFRem(LlackValueRef LHSConstant, LlackValueRef RHSConstant) {
  return wrap(ConstantExpr::getFRem(unwrap<Constant>(LHSConstant),
                                    unwrap<Constant>(RHSConstant)));
}

LlackValueRef LlackConstAnd(LlackValueRef LHSConstant, LlackValueRef RHSConstant) {
  return wrap(ConstantExpr::getAnd(unwrap<Constant>(LHSConstant),
                                   unwrap<Constant>(RHSConstant)));
}

LlackValueRef LlackConstOr(LlackValueRef LHSConstant, LlackValueRef RHSConstant) {
  return wrap(ConstantExpr::getOr(unwrap<Constant>(LHSConstant),
                                  unwrap<Constant>(RHSConstant)));
}

LlackValueRef LlackConstXor(LlackValueRef LHSConstant, LlackValueRef RHSConstant) {
  return wrap(ConstantExpr::getXor(unwrap<Constant>(LHSConstant),
                                   unwrap<Constant>(RHSConstant)));
}

LlackValueRef LlackConstICmp(LlackIntPredicate Predicate,
                           LlackValueRef LHSConstant, LlackValueRef RHSConstant) {
  return wrap(ConstantExpr::getICmp(Predicate,
                                    unwrap<Constant>(LHSConstant),
                                    unwrap<Constant>(RHSConstant)));
}

LlackValueRef LlackConstFCmp(LlackRealPredicate Predicate,
                           LlackValueRef LHSConstant, LlackValueRef RHSConstant) {
  return wrap(ConstantExpr::getFCmp(Predicate,
                                    unwrap<Constant>(LHSConstant),
                                    unwrap<Constant>(RHSConstant)));
}

LlackValueRef LlackConstShl(LlackValueRef LHSConstant, LlackValueRef RHSConstant) {
  return wrap(ConstantExpr::getShl(unwrap<Constant>(LHSConstant),
                                  unwrap<Constant>(RHSConstant)));
}

LlackValueRef LlackConstLShr(LlackValueRef LHSConstant, LlackValueRef RHSConstant) {
  return wrap(ConstantExpr::getLShr(unwrap<Constant>(LHSConstant),
                                    unwrap<Constant>(RHSConstant)));
}

LlackValueRef LlackConstAShr(LlackValueRef LHSConstant, LlackValueRef RHSConstant) {
  return wrap(ConstantExpr::getAShr(unwrap<Constant>(LHSConstant),
                                    unwrap<Constant>(RHSConstant)));
}

LlackValueRef LlackConstGEP(LlackValueRef ConstantVal,
                          LlackValueRef *ConstantIndices, unsigned NumIndices) {
  return wrap(ConstantExpr::getGetElementPtr(unwrap<Constant>(ConstantVal),
                                             unwrap<Constant>(ConstantIndices, 
                                                              NumIndices),
                                             NumIndices));
}

LlackValueRef LlackConstTrunc(LlackValueRef ConstantVal, LlackTypeRef ToType) {
  return wrap(ConstantExpr::getTrunc(unwrap<Constant>(ConstantVal),
                                     unwrap(ToType)));
}

LlackValueRef LlackConstSExt(LlackValueRef ConstantVal, LlackTypeRef ToType) {
  return wrap(ConstantExpr::getSExt(unwrap<Constant>(ConstantVal),
                                    unwrap(ToType)));
}

LlackValueRef LlackConstZExt(LlackValueRef ConstantVal, LlackTypeRef ToType) {
  return wrap(ConstantExpr::getZExt(unwrap<Constant>(ConstantVal),
                                    unwrap(ToType)));
}

LlackValueRef LlackConstFPTrunc(LlackValueRef ConstantVal, LlackTypeRef ToType) {
  return wrap(ConstantExpr::getFPTrunc(unwrap<Constant>(ConstantVal),
                                       unwrap(ToType)));
}

LlackValueRef LlackConstFPExt(LlackValueRef ConstantVal, LlackTypeRef ToType) {
  return wrap(ConstantExpr::getFPExtend(unwrap<Constant>(ConstantVal),
                                        unwrap(ToType)));
}

LlackValueRef LlackConstUIToFP(LlackValueRef ConstantVal, LlackTypeRef ToType) {
  return wrap(ConstantExpr::getUIToFP(unwrap<Constant>(ConstantVal),
                                      unwrap(ToType)));
}

LlackValueRef LlackConstSIToFP(LlackValueRef ConstantVal, LlackTypeRef ToType) {
  return wrap(ConstantExpr::getSIToFP(unwrap<Constant>(ConstantVal),
                                      unwrap(ToType)));
}

LlackValueRef LlackConstFPToUI(LlackValueRef ConstantVal, LlackTypeRef ToType) {
  return wrap(ConstantExpr::getFPToUI(unwrap<Constant>(ConstantVal),
                                      unwrap(ToType)));
}

LlackValueRef LlackConstFPToSI(LlackValueRef ConstantVal, LlackTypeRef ToType) {
  return wrap(ConstantExpr::getFPToSI(unwrap<Constant>(ConstantVal),
                                      unwrap(ToType)));
}

LlackValueRef LlackConstPtrToInt(LlackValueRef ConstantVal, LlackTypeRef ToType) {
  return wrap(ConstantExpr::getPtrToInt(unwrap<Constant>(ConstantVal),
                                        unwrap(ToType)));
}

LlackValueRef LlackConstIntToPtr(LlackValueRef ConstantVal, LlackTypeRef ToType) {
  return wrap(ConstantExpr::getIntToPtr(unwrap<Constant>(ConstantVal),
                                        unwrap(ToType)));
}

LlackValueRef LlackConstBitCast(LlackValueRef ConstantVal, LlackTypeRef ToType) {
  return wrap(ConstantExpr::getBitCast(unwrap<Constant>(ConstantVal),
                                       unwrap(ToType)));
}

LlackValueRef LlackConstSelect(LlackValueRef ConstantCondition,
                             LlackValueRef ConstantIfTrue,
                             LlackValueRef ConstantIfFalse) {
  return wrap(ConstantExpr::getSelect(unwrap<Constant>(ConstantCondition),
                                      unwrap<Constant>(ConstantIfTrue),
                                      unwrap<Constant>(ConstantIfFalse)));
}

LlackValueRef LlackConstExtractElement(LlackValueRef VectorConstant,
                                     LlackValueRef IndexConstant) {
  return wrap(ConstantExpr::getExtractElement(unwrap<Constant>(VectorConstant),
                                              unwrap<Constant>(IndexConstant)));
}

LlackValueRef LlackConstInsertElement(LlackValueRef VectorConstant,
                                    LlackValueRef ElementValueConstant,
                                    LlackValueRef IndexConstant) {
  return wrap(ConstantExpr::getInsertElement(unwrap<Constant>(VectorConstant),
                                         unwrap<Constant>(ElementValueConstant),
                                             unwrap<Constant>(IndexConstant)));
}

LlackValueRef LlackConstShuffleVector(LlackValueRef VectorAConstant,
                                    LlackValueRef VectorBConstant,
                                    LlackValueRef MaskConstant) {
  return wrap(ConstantExpr::getShuffleVector(unwrap<Constant>(VectorAConstant),
                                             unwrap<Constant>(VectorBConstant),
                                             unwrap<Constant>(MaskConstant)));
}

/*--.. Operations on global variables, functions, and aliases (globals) ....--*/

LlackModuleRef LlackGetGlobalParent(LlackValueRef Global) {
  return wrap(unwrap<GlobalValue>(Global)->getParent());
}

int LlackIsDeclaration(LlackValueRef Global) {
  return unwrap<GlobalValue>(Global)->isDeclaration();
}

LlackLinkage LlackGetLinkage(LlackValueRef Global) {
  return static_cast<LlackLinkage>(unwrap<GlobalValue>(Global)->getLinkage());
}

void LlackSetLinkage(LlackValueRef Global, LlackLinkage Linkage) {
  unwrap<GlobalValue>(Global)
    ->setLinkage(static_cast<GlobalValue::LinkageTypes>(Linkage));
}

const char *LlackGetSection(LlackValueRef Global) {
  return unwrap<GlobalValue>(Global)->getSection().c_str();
}

void LlackSetSection(LlackValueRef Global, const char *Section) {
  unwrap<GlobalValue>(Global)->setSection(Section);
}

LlackVisibility LlackGetVisibility(LlackValueRef Global) {
  return static_cast<LlackVisibility>(
    unwrap<GlobalValue>(Global)->getVisibility());
}

void LlackSetVisibility(LlackValueRef Global, LlackVisibility Viz) {
  unwrap<GlobalValue>(Global)
    ->setVisibility(static_cast<GlobalValue::VisibilityTypes>(Viz));
}

unsigned LlackGetAlignment(LlackValueRef Global) {
  return unwrap<GlobalValue>(Global)->getAlignment();
}

void LlackSetAlignment(LlackValueRef Global, unsigned Bytes) {
  unwrap<GlobalValue>(Global)->setAlignment(Bytes);
}

/*--.. Operations on global variables ......................................--*/

LlackValueRef LlackAddGlobal(LlackModuleRef M, LlackTypeRef Ty, const char *Name) {
  return wrap(new GlobalVariable(unwrap(Ty), false,
                                 GlobalValue::ExternalLinkage, 0, Name,
                                 unwrap(M)));
}

LlackValueRef LlackGetNamedGlobal(LlackModuleRef M, const char *Name) {
  return wrap(unwrap(M)->getNamedGlobal(Name));
}

LlackValueRef LlackGetFirstGlobal(LlackModuleRef M) {
  Module *Mod = unwrap(M);
  Module::global_iterator I = Mod->global_begin();
  if (I == Mod->global_end())
    return 0;
  return wrap(I);
}

LlackValueRef LlackGetLastGlobal(LlackModuleRef M) {
  Module *Mod = unwrap(M);
  Module::global_iterator I = Mod->global_end();
  if (I == Mod->global_begin())
    return 0;
  return wrap(--I);
}

LlackValueRef LlackGetNextGlobal(LlackValueRef GlobalVar) {
  GlobalVariable *GV = unwrap<GlobalVariable>(GlobalVar);
  Module::global_iterator I = GV;
  if (++I == GV->getParent()->global_end())
    return 0;
  return wrap(I);
}

LlackValueRef LlackGetPreviousGlobal(LlackValueRef GlobalVar) {
  GlobalVariable *GV = unwrap<GlobalVariable>(GlobalVar);
  Module::global_iterator I = GV;
  if (I == GV->getParent()->global_begin())
    return 0;
  return wrap(--I);
}

void LlackDeleteGlobal(LlackValueRef GlobalVar) {
  unwrap<GlobalVariable>(GlobalVar)->eraseFromParent();
}

LlackValueRef LlackGetInitializer(LlackValueRef GlobalVar) {
  return wrap(unwrap<GlobalVariable>(GlobalVar)->getInitializer());
}

void LlackSetInitializer(LlackValueRef GlobalVar, LlackValueRef ConstantVal) {
  unwrap<GlobalVariable>(GlobalVar)
    ->setInitializer(unwrap<Constant>(ConstantVal));
}

int LlackIsThreadLocal(LlackValueRef GlobalVar) {
  return unwrap<GlobalVariable>(GlobalVar)->isThreadLocal();
}

void LlackSetThreadLocal(LlackValueRef GlobalVar, int IsThreadLocal) {
  unwrap<GlobalVariable>(GlobalVar)->setThreadLocal(IsThreadLocal != 0);
}

int LlackIsGlobalConstant(LlackValueRef GlobalVar) {
  return unwrap<GlobalVariable>(GlobalVar)->isConstant();
}

void LlackSetGlobalConstant(LlackValueRef GlobalVar, int IsConstant) {
  unwrap<GlobalVariable>(GlobalVar)->setConstant(IsConstant != 0);
}

/*--.. Operations on functions .............................................--*/

LlackValueRef LlackAddFunction(LlackModuleRef M, const char *Name,
                             LlackTypeRef FunctionTy) {
  return wrap(Function::Create(unwrap<FunctionType>(FunctionTy),
                               GlobalValue::ExternalLinkage, Name, unwrap(M)));
}

LlackValueRef LlackGetNamedFunction(LlackModuleRef M, const char *Name) {
  return wrap(unwrap(M)->getFunction(Name));
}

LlackValueRef LlackGetFirstFunction(LlackModuleRef M) {
  Module *Mod = unwrap(M);
  Module::iterator I = Mod->begin();
  if (I == Mod->end())
    return 0;
  return wrap(I);
}

LlackValueRef LlackGetLastFunction(LlackModuleRef M) {
  Module *Mod = unwrap(M);
  Module::iterator I = Mod->end();
  if (I == Mod->begin())
    return 0;
  return wrap(--I);
}

LlackValueRef LlackGetNextFunction(LlackValueRef Fn) {
  Function *Func = unwrap<Function>(Fn);
  Module::iterator I = Func;
  if (++I == Func->getParent()->end())
    return 0;
  return wrap(I);
}

LlackValueRef LlackGetPreviousFunction(LlackValueRef Fn) {
  Function *Func = unwrap<Function>(Fn);
  Module::iterator I = Func;
  if (I == Func->getParent()->begin())
    return 0;
  return wrap(--I);
}

void LlackDeleteFunction(LlackValueRef Fn) {
  unwrap<Function>(Fn)->eraseFromParent();
}

unsigned LlackGetIntrinsicID(LlackValueRef Fn) {
  if (Function *F = dyn_cast<Function>(unwrap(Fn)))
    return F->getIntrinsicID();
  return 0;
}

unsigned LlackGetFunctionCallConv(LlackValueRef Fn) {
  return unwrap<Function>(Fn)->getCallingConv();
}

void LlackSetFunctionCallConv(LlackValueRef Fn, unsigned CC) {
  return unwrap<Function>(Fn)->setCallingConv(CC);
}

const char *LlackGetGC(LlackValueRef Fn) {
  Function *F = unwrap<Function>(Fn);
  return F->hasGC()? F->getGC() : 0;
}

void LlackSetGC(LlackValueRef Fn, const char *GC) {
  Function *F = unwrap<Function>(Fn);
  if (GC)
    F->setGC(GC);
  else
    F->clearGC();
}

/*--.. Operations on parameters ............................................--*/

unsigned LlackCountParams(LlackValueRef FnRef) {
  // This function is strictly redundant to
  //   LlackCountParamTypes(LlackGetElementType(LlackTypeOf(FnRef)))
  return unwrap<Function>(FnRef)->arg_size();
}

void LlackGetParams(LlackValueRef FnRef, LlackValueRef *ParamRefs) {
  Function *Fn = unwrap<Function>(FnRef);
  for (Function::arg_iterator I = Fn->arg_begin(),
                              E = Fn->arg_end(); I != E; I++)
    *ParamRefs++ = wrap(I);
}

LlackValueRef LlackGetParam(LlackValueRef FnRef, unsigned index) {
  Function::arg_iterator AI = unwrap<Function>(FnRef)->arg_begin();
  while (index --> 0)
    AI++;
  return wrap(AI);
}

LlackValueRef LlackGetParamParent(LlackValueRef V) {
  return wrap(unwrap<Argument>(V)->getParent());
}

LlackValueRef LlackGetFirstParam(LlackValueRef Fn) {
  Function *Func = unwrap<Function>(Fn);
  Function::arg_iterator I = Func->arg_begin();
  if (I == Func->arg_end())
    return 0;
  return wrap(I);
}

LlackValueRef LlackGetLastParam(LlackValueRef Fn) {
  Function *Func = unwrap<Function>(Fn);
  Function::arg_iterator I = Func->arg_end();
  if (I == Func->arg_begin())
    return 0;
  return wrap(--I);
}

LlackValueRef LlackGetNextParam(LlackValueRef Arg) {
  Argument *A = unwrap<Argument>(Arg);
  Function::arg_iterator I = A;
  if (++I == A->getParent()->arg_end())
    return 0;
  return wrap(I);
}

LlackValueRef LlackGetPreviousParam(LlackValueRef Arg) {
  Argument *A = unwrap<Argument>(Arg);
  Function::arg_iterator I = A;
  if (I == A->getParent()->arg_begin())
    return 0;
  return wrap(--I);
}

void LlackAddAttribute(LlackValueRef Arg, LlackAttribute PA) {
  unwrap<Argument>(Arg)->addAttr(PA);
}

void LlackRemoveAttribute(LlackValueRef Arg, LlackAttribute PA) {
  unwrap<Argument>(Arg)->removeAttr(PA);
}

void LlackSetParamAlignment(LlackValueRef Arg, unsigned align) {
  unwrap<Argument>(Arg)->addAttr(
          Attribute::constructAlignmentFromInt(align));
}

/*--.. Operations on basic blocks ..........................................--*/

LlackValueRef LlackBasicBlockAsValue(LlackBasicBlockRef BB) {
  return wrap(static_cast<Value*>(unwrap(BB)));
}

int LlackValueIsBasicBlock(LlackValueRef Val) {
  return isa<BasicBlock>(unwrap(Val));
}

LlackBasicBlockRef LlackValueAsBasicBlock(LlackValueRef Val) {
  return wrap(unwrap<BasicBlock>(Val));
}

LlackValueRef LlackGetBasicBlockParent(LlackBasicBlockRef BB) {
  return wrap(unwrap(BB)->getParent());
}

unsigned LlackCountBasicBlocks(LlackValueRef FnRef) {
  return unwrap<Function>(FnRef)->size();
}

void LlackGetBasicBlocks(LlackValueRef FnRef, LlackBasicBlockRef *BasicBlocksRefs){
  Function *Fn = unwrap<Function>(FnRef);
  for (Function::iterator I = Fn->begin(), E = Fn->end(); I != E; I++)
    *BasicBlocksRefs++ = wrap(I);
}

LlackBasicBlockRef LlackGetEntryBasicBlock(LlackValueRef Fn) {
  return wrap(&unwrap<Function>(Fn)->getEntryBlock());
}

LlackBasicBlockRef LlackGetFirstBasicBlock(LlackValueRef Fn) {
  Function *Func = unwrap<Function>(Fn);
  Function::iterator I = Func->begin();
  if (I == Func->end())
    return 0;
  return wrap(I);
}

LlackBasicBlockRef LlackGetLastBasicBlock(LlackValueRef Fn) {
  Function *Func = unwrap<Function>(Fn);
  Function::iterator I = Func->end();
  if (I == Func->begin())
    return 0;
  return wrap(--I);
}

LlackBasicBlockRef LlackGetNextBasicBlock(LlackBasicBlockRef BB) {
  BasicBlock *Block = unwrap(BB);
  Function::iterator I = Block;
  if (++I == Block->getParent()->end())
    return 0;
  return wrap(I);
}

LlackBasicBlockRef LlackGetPreviousBasicBlock(LlackBasicBlockRef BB) {
  BasicBlock *Block = unwrap(BB);
  Function::iterator I = Block;
  if (I == Block->getParent()->begin())
    return 0;
  return wrap(--I);
}

LlackBasicBlockRef LlackAppendBasicBlock(LlackValueRef FnRef, const char *Name) {
  return wrap(BasicBlock::Create(Name, unwrap<Function>(FnRef)));
}

LlackBasicBlockRef LlackInsertBasicBlock(LlackBasicBlockRef InsertBeforeBBRef,
                                       const char *Name) {
  BasicBlock *InsertBeforeBB = unwrap(InsertBeforeBBRef);
  return wrap(BasicBlock::Create(Name, InsertBeforeBB->getParent(),
                                 InsertBeforeBB));
}

void LlackDeleteBasicBlock(LlackBasicBlockRef BBRef) {
  unwrap(BBRef)->eraseFromParent();
}

/*--.. Operations on instructions ..........................................--*/

LlackBasicBlockRef LlackGetInstructionParent(LlackValueRef Inst) {
  return wrap(unwrap<Instruction>(Inst)->getParent());
}

LlackValueRef LlackGetFirstInstruction(LlackBasicBlockRef BB) {
  BasicBlock *Block = unwrap(BB);
  BasicBlock::iterator I = Block->begin();
  if (I == Block->end())
    return 0;
  return wrap(I);
}

LlackValueRef LlackGetLastInstruction(LlackBasicBlockRef BB) {
  BasicBlock *Block = unwrap(BB);
  BasicBlock::iterator I = Block->end();
  if (I == Block->begin())
    return 0;
  return wrap(--I);
}

LlackValueRef LlackGetNextInstruction(LlackValueRef Inst) {
  Instruction *Instr = unwrap<Instruction>(Inst);
  BasicBlock::iterator I = Instr;
  if (++I == Instr->getParent()->end())
    return 0;
  return wrap(I);
}

LlackValueRef LlackGetPreviousInstruction(LlackValueRef Inst) {
  Instruction *Instr = unwrap<Instruction>(Inst);
  BasicBlock::iterator I = Instr;
  if (I == Instr->getParent()->begin())
    return 0;
  return wrap(--I);
}

/*--.. Call and invoke instructions ........................................--*/

unsigned LlackGetInstructionCallConv(LlackValueRef Instr) {
  Value *V = unwrap(Instr);
  if (CallInst *CI = dyn_cast<CallInst>(V))
    return CI->getCallingConv();
  else if (InvokeInst *II = dyn_cast<InvokeInst>(V))
    return II->getCallingConv();
  assert(0 && "LlackGetInstructionCallConv applies only to call and invoke!");
  return 0;
}

void LlackSetInstructionCallConv(LlackValueRef Instr, unsigned CC) {
  Value *V = unwrap(Instr);
  if (CallInst *CI = dyn_cast<CallInst>(V))
    return CI->setCallingConv(CC);
  else if (InvokeInst *II = dyn_cast<InvokeInst>(V))
    return II->setCallingConv(CC);
  assert(0 && "LlackSetInstructionCallConv applies only to call and invoke!");
}

void LlackAddInstrAttribute(LlackValueRef Instr, unsigned index, 
                           LlackAttribute PA) {
  CallSite Call = CallSite(unwrap<Instruction>(Instr));
  Call.setAttributes(
    Call.getAttributes().addAttr(index, PA));
}

void LlackRemoveInstrAttribute(LlackValueRef Instr, unsigned index, 
                              LlackAttribute PA) {
  CallSite Call = CallSite(unwrap<Instruction>(Instr));
  Call.setAttributes(
    Call.getAttributes().removeAttr(index, PA));
}

void LlackSetInstrParamAlignment(LlackValueRef Instr, unsigned index, 
                                unsigned align) {
  CallSite Call = CallSite(unwrap<Instruction>(Instr));
  Call.setAttributes(
    Call.getAttributes().addAttr(index, 
        Attribute::constructAlignmentFromInt(align)));
}

/*--.. Operations on call instructions (only) ..............................--*/

int LlackIsTailCall(LlackValueRef Call) {
  return unwrap<CallInst>(Call)->isTailCall();
}

void LlackSetTailCall(LlackValueRef Call, int isTailCall) {
  unwrap<CallInst>(Call)->setTailCall(isTailCall);
}

/*--.. Operations on phi nodes .............................................--*/

void LlackAddIncoming(LlackValueRef PhiNode, LlackValueRef *IncomingValues,
                     LlackBasicBlockRef *IncomingBlocks, unsigned Count) {
  PHINode *PhiVal = unwrap<PHINode>(PhiNode);
  for (unsigned I = 0; I != Count; ++I)
    PhiVal->addIncoming(unwrap(IncomingValues[I]), unwrap(IncomingBlocks[I]));
}

unsigned LlackCountIncoming(LlackValueRef PhiNode) {
  return unwrap<PHINode>(PhiNode)->getNumIncomingValues();
}

LlackValueRef LlackGetIncomingValue(LlackValueRef PhiNode, unsigned Index) {
  return wrap(unwrap<PHINode>(PhiNode)->getIncomingValue(Index));
}

LlackBasicBlockRef LlackGetIncomingBlock(LlackValueRef PhiNode, unsigned Index) {
  return wrap(unwrap<PHINode>(PhiNode)->getIncomingBlock(Index));
}


/*===-- Instruction builders ----------------------------------------------===*/

LlackBuilderRef LlackCreateBuilder(void) {
  return wrap(new IRBuilder<>());
}

void LlackPositionBuilder(LlackBuilderRef Builder, LlackBasicBlockRef Block,
                         LlackValueRef Instr) {
  BasicBlock *BB = unwrap(Block);
  Instruction *I = Instr? unwrap<Instruction>(Instr) : (Instruction*) BB->end();
  unwrap(Builder)->SetInsertPoint(BB, I);
}

void LlackPositionBuilderBefore(LlackBuilderRef Builder, LlackValueRef Instr) {
  Instruction *I = unwrap<Instruction>(Instr);
  unwrap(Builder)->SetInsertPoint(I->getParent(), I);
}

void LlackPositionBuilderAtEnd(LlackBuilderRef Builder, LlackBasicBlockRef Block) {
  BasicBlock *BB = unwrap(Block);
  unwrap(Builder)->SetInsertPoint(BB);
}

LlackBasicBlockRef LlackGetInsertBlock(LlackBuilderRef Builder) {
   return wrap(unwrap(Builder)->GetInsertBlock());
}

void LlackDisposeBuilder(LlackBuilderRef Builder) {
  delete unwrap(Builder);
}

/*--.. Instruction builders ................................................--*/

LlackValueRef LlackBuildRetVoid(LlackBuilderRef B) {
  return wrap(unwrap(B)->CreateRetVoid());
}

LlackValueRef LlackBuildRet(LlackBuilderRef B, LlackValueRef V) {
  return wrap(unwrap(B)->CreateRet(unwrap(V)));
}

LlackValueRef LlackBuildBr(LlackBuilderRef B, LlackBasicBlockRef Dest) {
  return wrap(unwrap(B)->CreateBr(unwrap(Dest)));
}

LlackValueRef LlackBuildCondBr(LlackBuilderRef B, LlackValueRef If,
                             LlackBasicBlockRef Then, LlackBasicBlockRef Else) {
  return wrap(unwrap(B)->CreateCondBr(unwrap(If), unwrap(Then), unwrap(Else)));
}

LlackValueRef LlackBuildSwitch(LlackBuilderRef B, LlackValueRef V,
                             LlackBasicBlockRef Else, unsigned NumCases) {
  return wrap(unwrap(B)->CreateSwitch(unwrap(V), unwrap(Else), NumCases));
}

LlackValueRef LlackBuildInvoke(LlackBuilderRef B, LlackValueRef Fn,
                             LlackValueRef *Args, unsigned NumArgs,
                             LlackBasicBlockRef Then, LlackBasicBlockRef Catch,
                             const char *Name) {
  return wrap(unwrap(B)->CreateInvoke(unwrap(Fn), unwrap(Then), unwrap(Catch),
                                      unwrap(Args), unwrap(Args) + NumArgs,
                                      Name));
}

LlackValueRef LlackBuildUnwind(LlackBuilderRef B) {
  return wrap(unwrap(B)->CreateUnwind());
}

LlackValueRef LlackBuildUnreachable(LlackBuilderRef B) {
  return wrap(unwrap(B)->CreateUnreachable());
}

void LlackAddCase(LlackValueRef Switch, LlackValueRef OnVal,
                 LlackBasicBlockRef Dest) {
  unwrap<SwitchInst>(Switch)->addCase(unwrap<ConstantInt>(OnVal), unwrap(Dest));
}

/*--.. Arithmetic ..........................................................--*/

LlackValueRef LlackBuildAdd(LlackBuilderRef B, LlackValueRef LHS, LlackValueRef RHS,
                          const char *Name) {
  return wrap(unwrap(B)->CreateAdd(unwrap(LHS), unwrap(RHS), Name));
}

LlackValueRef LlackBuildSub(LlackBuilderRef B, LlackValueRef LHS, LlackValueRef RHS,
                          const char *Name) {
  return wrap(unwrap(B)->CreateSub(unwrap(LHS), unwrap(RHS), Name));
}

LlackValueRef LlackBuildMul(LlackBuilderRef B, LlackValueRef LHS, LlackValueRef RHS,
                          const char *Name) {
  return wrap(unwrap(B)->CreateMul(unwrap(LHS), unwrap(RHS), Name));
}

LlackValueRef LlackBuildUDiv(LlackBuilderRef B, LlackValueRef LHS, LlackValueRef RHS,
                           const char *Name) {
  return wrap(unwrap(B)->CreateUDiv(unwrap(LHS), unwrap(RHS), Name));
}

LlackValueRef LlackBuildSDiv(LlackBuilderRef B, LlackValueRef LHS, LlackValueRef RHS,
                           const char *Name) {
  return wrap(unwrap(B)->CreateSDiv(unwrap(LHS), unwrap(RHS), Name));
}

LlackValueRef LlackBuildFDiv(LlackBuilderRef B, LlackValueRef LHS, LlackValueRef RHS,
                           const char *Name) {
  return wrap(unwrap(B)->CreateFDiv(unwrap(LHS), unwrap(RHS), Name));
}

LlackValueRef LlackBuildURem(LlackBuilderRef B, LlackValueRef LHS, LlackValueRef RHS,
                           const char *Name) {
  return wrap(unwrap(B)->CreateURem(unwrap(LHS), unwrap(RHS), Name));
}

LlackValueRef LlackBuildSRem(LlackBuilderRef B, LlackValueRef LHS, LlackValueRef RHS,
                           const char *Name) {
  return wrap(unwrap(B)->CreateSRem(unwrap(LHS), unwrap(RHS), Name));
}

LlackValueRef LlackBuildFRem(LlackBuilderRef B, LlackValueRef LHS, LlackValueRef RHS,
                           const char *Name) {
  return wrap(unwrap(B)->CreateFRem(unwrap(LHS), unwrap(RHS), Name));
}

LlackValueRef LlackBuildShl(LlackBuilderRef B, LlackValueRef LHS, LlackValueRef RHS,
                          const char *Name) {
  return wrap(unwrap(B)->CreateShl(unwrap(LHS), unwrap(RHS), Name));
}

LlackValueRef LlackBuildLShr(LlackBuilderRef B, LlackValueRef LHS, LlackValueRef RHS,
                           const char *Name) {
  return wrap(unwrap(B)->CreateLShr(unwrap(LHS), unwrap(RHS), Name));
}

LlackValueRef LlackBuildAShr(LlackBuilderRef B, LlackValueRef LHS, LlackValueRef RHS,
                           const char *Name) {
  return wrap(unwrap(B)->CreateAShr(unwrap(LHS), unwrap(RHS), Name));
}

LlackValueRef LlackBuildAnd(LlackBuilderRef B, LlackValueRef LHS, LlackValueRef RHS,
                          const char *Name) {
  return wrap(unwrap(B)->CreateAnd(unwrap(LHS), unwrap(RHS), Name));
}

LlackValueRef LlackBuildOr(LlackBuilderRef B, LlackValueRef LHS, LlackValueRef RHS,
                         const char *Name) {
  return wrap(unwrap(B)->CreateOr(unwrap(LHS), unwrap(RHS), Name));
}

LlackValueRef LlackBuildXor(LlackBuilderRef B, LlackValueRef LHS, LlackValueRef RHS,
                          const char *Name) {
  return wrap(unwrap(B)->CreateXor(unwrap(LHS), unwrap(RHS), Name));
}

LlackValueRef LlackBuildNeg(LlackBuilderRef B, LlackValueRef V, const char *Name) {
  return wrap(unwrap(B)->CreateNeg(unwrap(V), Name));
}

LlackValueRef LlackBuildNot(LlackBuilderRef B, LlackValueRef V, const char *Name) {
  return wrap(unwrap(B)->CreateNot(unwrap(V), Name));
}

/*--.. Memory ..............................................................--*/

LlackValueRef LlackBuildMalloc(LlackBuilderRef B, LlackTypeRef Ty,
                             const char *Name) {
  return wrap(unwrap(B)->CreateMalloc(unwrap(Ty), 0, Name));
}

LlackValueRef LlackBuildArrayMalloc(LlackBuilderRef B, LlackTypeRef Ty,
                                  LlackValueRef Val, const char *Name) {
  return wrap(unwrap(B)->CreateMalloc(unwrap(Ty), unwrap(Val), Name));
}

LlackValueRef LlackBuildAlloca(LlackBuilderRef B, LlackTypeRef Ty,
                             const char *Name) {
  return wrap(unwrap(B)->CreateAlloca(unwrap(Ty), 0, Name));
}

LlackValueRef LlackBuildArrayAlloca(LlackBuilderRef B, LlackTypeRef Ty,
                                  LlackValueRef Val, const char *Name) {
  return wrap(unwrap(B)->CreateAlloca(unwrap(Ty), unwrap(Val), Name));
}

LlackValueRef LlackBuildFree(LlackBuilderRef B, LlackValueRef PointerVal) {
  return wrap(unwrap(B)->CreateFree(unwrap(PointerVal)));
}


LlackValueRef LlackBuildLoad(LlackBuilderRef B, LlackValueRef PointerVal,
                           const char *Name) {
  return wrap(unwrap(B)->CreateLoad(unwrap(PointerVal), Name));
}

LlackValueRef LlackBuildStore(LlackBuilderRef B, LlackValueRef Val, 
                            LlackValueRef PointerVal) {
  return wrap(unwrap(B)->CreateStore(unwrap(Val), unwrap(PointerVal)));
}

LlackValueRef LlackBuildGEP(LlackBuilderRef B, LlackValueRef Pointer,
                          LlackValueRef *Indices, unsigned NumIndices,
                          const char *Name) {
  return wrap(unwrap(B)->CreateGEP(unwrap(Pointer), unwrap(Indices),
                                   unwrap(Indices) + NumIndices, Name));
}

/*--.. Casts ...............................................................--*/

LlackValueRef LlackBuildTrunc(LlackBuilderRef B, LlackValueRef Val,
                            LlackTypeRef DestTy, const char *Name) {
  return wrap(unwrap(B)->CreateTrunc(unwrap(Val), unwrap(DestTy), Name));
}

LlackValueRef LlackBuildZExt(LlackBuilderRef B, LlackValueRef Val,
                           LlackTypeRef DestTy, const char *Name) {
  return wrap(unwrap(B)->CreateZExt(unwrap(Val), unwrap(DestTy), Name));
}

LlackValueRef LlackBuildSExt(LlackBuilderRef B, LlackValueRef Val,
                           LlackTypeRef DestTy, const char *Name) {
  return wrap(unwrap(B)->CreateSExt(unwrap(Val), unwrap(DestTy), Name));
}

LlackValueRef LlackBuildFPToUI(LlackBuilderRef B, LlackValueRef Val,
                             LlackTypeRef DestTy, const char *Name) {
  return wrap(unwrap(B)->CreateFPToUI(unwrap(Val), unwrap(DestTy), Name));
}

LlackValueRef LlackBuildFPToSI(LlackBuilderRef B, LlackValueRef Val,
                             LlackTypeRef DestTy, const char *Name) {
  return wrap(unwrap(B)->CreateFPToSI(unwrap(Val), unwrap(DestTy), Name));
}

LlackValueRef LlackBuildUIToFP(LlackBuilderRef B, LlackValueRef Val,
                             LlackTypeRef DestTy, const char *Name) {
  return wrap(unwrap(B)->CreateUIToFP(unwrap(Val), unwrap(DestTy), Name));
}

LlackValueRef LlackBuildSIToFP(LlackBuilderRef B, LlackValueRef Val,
                             LlackTypeRef DestTy, const char *Name) {
  return wrap(unwrap(B)->CreateSIToFP(unwrap(Val), unwrap(DestTy), Name));
}

LlackValueRef LlackBuildFPTrunc(LlackBuilderRef B, LlackValueRef Val,
                              LlackTypeRef DestTy, const char *Name) {
  return wrap(unwrap(B)->CreateFPTrunc(unwrap(Val), unwrap(DestTy), Name));
}

LlackValueRef LlackBuildFPExt(LlackBuilderRef B, LlackValueRef Val,
                            LlackTypeRef DestTy, const char *Name) {
  return wrap(unwrap(B)->CreateFPExt(unwrap(Val), unwrap(DestTy), Name));
}

LlackValueRef LlackBuildPtrToInt(LlackBuilderRef B, LlackValueRef Val,
                               LlackTypeRef DestTy, const char *Name) {
  return wrap(unwrap(B)->CreatePtrToInt(unwrap(Val), unwrap(DestTy), Name));
}

LlackValueRef LlackBuildIntToPtr(LlackBuilderRef B, LlackValueRef Val,
                               LlackTypeRef DestTy, const char *Name) {
  return wrap(unwrap(B)->CreateIntToPtr(unwrap(Val), unwrap(DestTy), Name));
}

LlackValueRef LlackBuildBitCast(LlackBuilderRef B, LlackValueRef Val,
                              LlackTypeRef DestTy, const char *Name) {
  return wrap(unwrap(B)->CreateBitCast(unwrap(Val), unwrap(DestTy), Name));
}

/*--.. Comparisons .........................................................--*/

LlackValueRef LlackBuildICmp(LlackBuilderRef B, LlackIntPredicate Op,
                           LlackValueRef LHS, LlackValueRef RHS,
                           const char *Name) {
  return wrap(unwrap(B)->CreateICmp(static_cast<ICmpInst::Predicate>(Op),
                                    unwrap(LHS), unwrap(RHS), Name));
}

LlackValueRef LlackBuildFCmp(LlackBuilderRef B, LlackRealPredicate Op,
                           LlackValueRef LHS, LlackValueRef RHS,
                           const char *Name) {
  return wrap(unwrap(B)->CreateFCmp(static_cast<FCmpInst::Predicate>(Op),
                                    unwrap(LHS), unwrap(RHS), Name));
}

/*--.. Miscellaneous instructions ..........................................--*/

LlackValueRef LlackBuildPhi(LlackBuilderRef B, LlackTypeRef Ty, const char *Name) {
  return wrap(unwrap(B)->CreatePHI(unwrap(Ty), Name));
}

LlackValueRef LlackBuildCall(LlackBuilderRef B, LlackValueRef Fn,
                           LlackValueRef *Args, unsigned NumArgs,
                           const char *Name) {
  return wrap(unwrap(B)->CreateCall(unwrap(Fn), unwrap(Args),
                                    unwrap(Args) + NumArgs, Name));
}

LlackValueRef LlackBuildSelect(LlackBuilderRef B, LlackValueRef If,
                             LlackValueRef Then, LlackValueRef Else,
                             const char *Name) {
  return wrap(unwrap(B)->CreateSelect(unwrap(If), unwrap(Then), unwrap(Else),
                                      Name));
}

LlackValueRef LlackBuildVAArg(LlackBuilderRef B, LlackValueRef List,
                            LlackTypeRef Ty, const char *Name) {
  return wrap(unwrap(B)->CreateVAArg(unwrap(List), unwrap(Ty), Name));
}

LlackValueRef LlackBuildExtractElement(LlackBuilderRef B, LlackValueRef VecVal,
                                      LlackValueRef Index, const char *Name) {
  return wrap(unwrap(B)->CreateExtractElement(unwrap(VecVal), unwrap(Index),
                                              Name));
}

LlackValueRef LlackBuildInsertElement(LlackBuilderRef B, LlackValueRef VecVal,
                                    LlackValueRef EltVal, LlackValueRef Index,
                                    const char *Name) {
  return wrap(unwrap(B)->CreateInsertElement(unwrap(VecVal), unwrap(EltVal),
                                             unwrap(Index), Name));
}

LlackValueRef LlackBuildShuffleVector(LlackBuilderRef B, LlackValueRef V1,
                                    LlackValueRef V2, LlackValueRef Mask,
                                    const char *Name) {
  return wrap(unwrap(B)->CreateShuffleVector(unwrap(V1), unwrap(V2),
                                             unwrap(Mask), Name));
}


/*===-- Module providers --------------------------------------------------===*/

LlackModuleProviderRef
LlackCreateModuleProviderForExistingModule(LlackModuleRef M) {
  return wrap(new ExistingModuleProvider(unwrap(M)));
}

void LlackDisposeModuleProvider(LlackModuleProviderRef MP) {
  delete unwrap(MP);
}


/*===-- Memory buffers ----------------------------------------------------===*/

int LlackCreateMemoryBufferWithContentsOfFile(const char *Path,
                                             LlackMemoryBufferRef *OutMemBuf,
                                             char **OutMessage) {
  std::string Error;
  if (MemoryBuffer *MB = MemoryBuffer::getFile(Path, &Error)) {
    *OutMemBuf = wrap(MB);
    return 0;
  }
  
  *OutMessage = strdup(Error.c_str());
  return 1;
}

int LlackCreateMemoryBufferWithSTDIN(LlackMemoryBufferRef *OutMemBuf,
                                    char **OutMessage) {
  if (MemoryBuffer *MB = MemoryBuffer::getSTDIN()) {
    *OutMemBuf = wrap(MB);
    return 0;
  }
  
  *OutMessage = strdup("stdin is empty.");
  return 1;
}

void LlackDisposeMemoryBuffer(LlackMemoryBufferRef MemBuf) {
  delete unwrap(MemBuf);
}

#endif /* DONT_COMPILE */