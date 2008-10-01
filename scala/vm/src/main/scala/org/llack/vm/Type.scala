/* -*- Mode: Scala; indent-tabs-mode: nil; tab-width: 2 -*- */

package org.llack.vm

import scala.collection._

sealed trait Type
case object VoidType extends Type {
  override def toString = "void"
}
case object FloatType extends Type {
  override def toString = "float"
}
case object DoubleType extends Type {
  override def toString = "double"
}
case object FP128Type extends Type {
  override def toString = "fp128"
}
case object X86_FP80Type extends Type {
  override def toString = "x86_fp80"
}
case object PPC_FP128Type extends Type {
  override def toString = "ppc_fp128"
}
case object LabelType extends Type {
  override def toString = "label"
}
case object QuotationType extends Type {
  override def toString = "quot"
}
case class IntegerType(n: Int) extends Type {
  assert(n > 0)
  override def toString = "i" + n
}
case class ArrayType(n: Int, t: Type) extends Type {
  //assert(t.hasSize)
  override def toString = "[" + n + " x " + t + "]"
}
case class FunctionType(rt: Type, params: Type*) extends Type {
  //assert(t.isScalar || t == VoidType || t.instanceOf[Struct])
  //TODO: attribute params
  //TODO: varargs
  override def toString = rt + params.mkString("( ", ", ", " )")
}
case class StructType(members: Type*) extends Type {
  //assert(members.forall(_.hasSize))
  override def toString = members.mkString("{ ", ", ", " }")
}
case class PackedStructType(members: Type*) extends Type {
  //assert(members.forall(_.hasSize))
  override def toString = members.mkString("< { ", ", ", " } >")
}
case class PointerType(t: Type, as: Int) extends Type {
  def this(t: Type) = this(t, 0)
  override def toString = {
    if (as == 0) t + "*"
    else t + " addrspace(" + as + ")*"
  }
}
case class VectorType(n: Int, t: Type) extends Type {
  //assert(t.isInteger || t.isFloatingPoint)
  def isFirstClass = true
  override def toString = "<" + n + " x " + t + ">"
}
case object OpaqueType extends Type {
  override def toString = "opaque"
}
