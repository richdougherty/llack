/* -*- Mode: Scala; indent-tabs-mode: nil; tab-width: 2 -*- */

package org.llack.vm

import scala.collection._

sealed trait Instruction
abstract case class BinaryOperator(name: String, ty: Type) extends Instruction {
  override def toString = name + " " + ty
}
// Stack operations
case class PushInst(t: Type, v: Any) extends Instruction {
  override def toString = "push " + t + " " + v
}
case class ShuffleInst(ts: List[Type], indices: List[Int]) extends Instruction {
  override def toString = "shuffle " + (ts ++ indices).mkString("", " ", "")
}
case class ToRetainInst(t: Type) extends Instruction {
  override def toString = "toretain " + t
}
case class FromRetainInst(t: Type) extends Instruction {
  override def toString = "fromretain " + t
}
// Control flow within quotations
case object ApplyInst extends Instruction {
  override def toString = "apply"
}
//case object RetInst extends Instruction
// LLVM term instructions
//case object BrInst
//case class SwitchInst(t1: Type, t2: Type, default: Any, table: (Int, Any)*)
//case object InvokeInst(ft: FunctionType, ) // calling conv
//case object UnwindInst
//case object UnreachableInst
// LLVM standard binary operators
case class AddInst(t: Type) extends BinaryOperator("add", t)
case class SubInst(t: Type) extends BinaryOperator("sub", t)
case class MulInst(t: Type) extends BinaryOperator("mul", t)
case class UDivInst(t: Type) extends BinaryOperator("udiv", t)
case class SDivInst(t: Type) extends BinaryOperator("sdiv", t)
case class FDivInst(t: Type) extends BinaryOperator("fdiv", t)
case class URemInst(t: Type) extends BinaryOperator("urem", t)
case class SRemInst(t: Type) extends BinaryOperator("srem", t)
case class FRemInst(t: Type) extends BinaryOperator("frem", t)
// LVM logical operators (integer operands)
case class ShlInst(t: Type) extends BinaryOperator("shl", t)
case class LShrInst(t: Type) extends BinaryOperator("lshr", t)
case class AShrInst(t: Type) extends BinaryOperator("ashr", t)
case class AndInst(t: Type) extends BinaryOperator("and", t)
case class OrInst(t: Type) extends BinaryOperator("or", t)
case class XorInst(t: Type) extends BinaryOperator("xor", t)
// LLVM memory operators
case class MallocInst(t: Type) extends Instruction
case class FreeInst(t: Type) extends Instruction
//AllocaInst
case class LoadInst(t: Type) extends Instruction
case class StoreInst(t: Type) extends Instruction
case class GetElementPtrInst(t: Type, indices: (Type, Int)*) extends Instruction
// LLVM: Cast operators
case class TruncInst(t1: Type, t2: Type) extends Instruction
case class ZExtInst(t1: Type, t2: Type) extends Instruction
case class SExtInst(t1: Type, t2: Type) extends Instruction
case class FPToUIInst(t1: Type, t2: Type) extends Instruction
case class FPToSIInst(t1: Type, t2: Type) extends Instruction
case class UIToFPInst(t1: Type, t2: Type) extends Instruction
case class SIToFPInst(t1: Type, t2: Type) extends Instruction
case class FPTruncInst(t1: Type, t2: Type) extends Instruction
case class FPExtInst(t1: Type, t2: Type) extends Instruction
case class PtrToIntInst(t1: Type, t2: Type) extends Instruction
case class IntToPtrInst(t1: Type, t2: Type) extends Instruction
case class BitCastInst(t1: Type, t2: Type) extends Instruction
// LLVM other
case class ICmpInst(cond: IntegerCondition, t: Type) extends Instruction {
  override def toString = "icmp " + cond + " " + t
}
case class FCmpInst(cond: FPCondition, t: Type) extends Instruction
//case class PHINode(t: Type) extends Instruction
case class CallInst(t: Type) extends Instruction // add calling convention
case class SelectInst(t: Type) extends Instruction {
  override def toString = "select " + t
}
//case class VAArg(t: Type) extends Instruction
case class ExtractElement(t: Type) extends Instruction
case class InsertElement(t: Type) extends Instruction
case class ShuffleVector(t: Type) extends Instruction
case class GetResult(t: Type, index: Int) extends Instruction
