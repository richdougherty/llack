//===----------- ValueTypes.cpp - Implementation of MVT methods -----------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file implements methods in the CodeGen/ValueTypes.h header.
//
//===----------------------------------------------------------------------===//

#include "llack/CodeGen/ValueTypes.h"
#include "llack/Type.h"
#include "llack/DerivedTypes.h"
#include "llvm/ADT/StringExtras.h"
using namespace llack;
using llvm::cast;
using llvm::utostr;

/// getMVTString - This function returns value type as a string, e.g. "i32".
std::string MVT::getMVTString() const {
  switch (V) {
  default:
    if (isVector())
      return "v" + utostr(getVectorNumElements()) +
             getVectorElementType().getMVTString();
    if (isInteger())
      return "i" + utostr(getSizeInBits());
    assert(0 && "Invalid MVT!");
    return "?";
  case MVT::i1:      return "i1";
  case MVT::i8:      return "i8";
  case MVT::i16:     return "i16";
  case MVT::i32:     return "i32";
  case MVT::i64:     return "i64";
  case MVT::i128:    return "i128";
  case MVT::f32:     return "f32";
  case MVT::f64:     return "f64";
  case MVT::f80:     return "f80";
  case MVT::f128:    return "f128";
  case MVT::ppcf128: return "ppcf128";
  case MVT::isVoid:  return "isVoid";
  case MVT::Other:   return "ch";
  case MVT::Flag:    return "flag";
  case MVT::v8i8:    return "v8i8";
  case MVT::v4i16:   return "v4i16";
  case MVT::v2i32:   return "v2i32";
  case MVT::v1i64:   return "v1i64";
  case MVT::v16i8:   return "v16i8";
  case MVT::v8i16:   return "v8i16";
  case MVT::v4i32:   return "v4i32";
  case MVT::v2i64:   return "v2i64";
  case MVT::v2f32:   return "v2f32";
  case MVT::v4f32:   return "v4f32";
  case MVT::v2f64:   return "v2f64";
  case MVT::v3i32:   return "v3i32";
  case MVT::v3f32:   return "v3f32";
  }
}

/// getTypeForMVT - This method returns an LLVM type corresponding to the
/// specified MVT.  For integer types, this returns an unsigned type.  Note
/// that this will abort for types that cannot be represented.
const Type *MVT::getTypeForMVT() const {
  switch (V) {
  default:
    if (isVector())
      return VectorType::get(getVectorElementType().getTypeForMVT(),
                             getVectorNumElements());
    if (isInteger())
      return IntegerType::get(getSizeInBits());
    assert(0 && "MVT does not correspond to LLVM type!");
    return Type::VoidTy;
  case MVT::isVoid:  return Type::VoidTy;
  case MVT::i1:      return Type::Int1Ty;
  case MVT::i8:      return Type::Int8Ty;
  case MVT::i16:     return Type::Int16Ty;
  case MVT::i32:     return Type::Int32Ty;
  case MVT::i64:     return Type::Int64Ty;
  case MVT::i128:    return IntegerType::get(128);
  case MVT::f32:     return Type::FloatTy;
  case MVT::f64:     return Type::DoubleTy;
  case MVT::f80:     return Type::X86_FP80Ty;
  case MVT::f128:    return Type::FP128Ty;
  case MVT::ppcf128: return Type::PPC_FP128Ty;
  case MVT::v8i8:    return VectorType::get(Type::Int8Ty, 8);
  case MVT::v4i16:   return VectorType::get(Type::Int16Ty, 4);
  case MVT::v2i32:   return VectorType::get(Type::Int32Ty, 2);
  case MVT::v1i64:   return VectorType::get(Type::Int64Ty, 1);
  case MVT::v16i8:   return VectorType::get(Type::Int8Ty, 16);
  case MVT::v8i16:   return VectorType::get(Type::Int16Ty, 8);
  case MVT::v4i32:   return VectorType::get(Type::Int32Ty, 4);
  case MVT::v2i64:   return VectorType::get(Type::Int64Ty, 2);
  case MVT::v2f32:   return VectorType::get(Type::FloatTy, 2);
  case MVT::v4f32:   return VectorType::get(Type::FloatTy, 4);
  case MVT::v2f64:   return VectorType::get(Type::DoubleTy, 2);
  case MVT::v3i32:   return VectorType::get(Type::Int32Ty, 3);
  case MVT::v3f32:   return VectorType::get(Type::FloatTy, 3);
  }
}

/// getMVT - Return the value type corresponding to the specified type.  This
/// returns all pointers as MVT::iPTR.  If HandleUnknown is true, unknown types
/// are returned as Other, otherwise they are invalid.
MVT MVT::getMVT(const Type *Ty, bool HandleUnknown){
  switch (Ty->getTypeID()) {
  default:
    if (HandleUnknown) return MVT::Other;
    assert(0 && "Unknown type!");
    return MVT::isVoid;
  case Type::VoidTyID:
    return MVT::isVoid;
  case Type::IntegerTyID:
    return getIntegerVT(cast<IntegerType>(Ty)->getBitWidth());
  case Type::FloatTyID:     return MVT::f32;
  case Type::DoubleTyID:    return MVT::f64;
  case Type::X86_FP80TyID:  return MVT::f80;
  case Type::FP128TyID:     return MVT::f128;
  case Type::PPC_FP128TyID: return MVT::ppcf128;
  case Type::PointerTyID:   return MVT::iPTR;
  case Type::VectorTyID: {
    const VectorType *VTy = cast<VectorType>(Ty);
    return getVectorVT(getMVT(VTy->getElementType(), false),
                       VTy->getNumElements());
  }
  }
}
