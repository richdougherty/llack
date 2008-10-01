/* -*- Mode: Scala; indent-tabs-mode: nil; tab-width: 2 -*- */

package org.llack.vm

import scala.collection._

sealed trait IntegerCondition
case object EQCond extends IntegerCondition {
  override def toString = "eq"
}
case object NECond extends IntegerCondition {
  override def toString = "ne"
}
case object UGTCond extends IntegerCondition {
  override def toString = "ugt"
}
case object ULTCond extends IntegerCondition {
  override def toString = "ult"
}
case object ULECond extends IntegerCondition {
  override def toString = "ule"
}
case object SGTCond extends IntegerCondition {
  override def toString = "sgt"
}
case object SGECond extends IntegerCondition {
  override def toString = "sge"
}
case object SLTCond extends IntegerCondition {
  override def toString = "slt"
}
case object SLECond extends IntegerCondition {
  override def toString = "sle"
}

sealed trait FPCondition
case object FalseFPCond extends FPCondition {
  override def toString = "false"
}
case object OEQFPCond extends FPCondition {
  override def toString = "oeq"
}
case object OGTFPCond extends FPCondition {
  override def toString = "ogt"
}
case object OGEFPCond extends FPCondition {
  override def toString = "oge"
}
case object OLTFPCond extends FPCondition {
  override def toString = "olt"
}
case object ONEFPCond extends FPCondition {
  override def toString = "one"
}
case object ORDFPCond extends FPCondition {
  override def toString = "ord"
}
case object UEQFPCond extends FPCondition {
  override def toString = "ueq"
}
case object UGTFPCond extends FPCondition {
  override def toString = "ugt"
}
case object UGEFPCond extends FPCondition {
  override def toString = "uge"
}
case object ULTFPCond extends FPCondition {
  override def toString = "ult"
}
case object ULEFPCond extends FPCondition {
  override def toString = "ule"
}
case object UNEFPCond extends FPCondition {
  override def toString = "une"
}
case object UNOFPCond extends FPCondition {
  override def toString = "uno"
}
case object TrueFPCond extends FPCondition {
  override def toString = "true"
}
