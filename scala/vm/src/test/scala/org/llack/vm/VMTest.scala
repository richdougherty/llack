package org.llack.vm

import org.testng.annotations.{Test, BeforeMethod}

import org.scalatest.testng.TestNGSuite
import org.scalatest.prop.Checkers
import org.scalacheck.Arbitrary
import org.scalacheck.Arbitrary._
import org.scalacheck.Gen
import org.scalacheck.Prop._
import org.scalatest._

class VMTest extends TestNGSuite with Checkers {

  private def testModule: Module = {
    val i32 = IntegerType(32)

    val m = new Module
    m.moduleIdentifier = Some("vmmod")
    m.dataLayout = Some("e-p:32:32:32-i1:8:8-i8:8:8-i16:16:16-i32:32:32-i64:32:64-f32:32:32-f64:32:64-v64:64:64-v128:128:128-a0:0:64-f80:128:128")
    m.targetTriple = Some("i386-apple-darwin8")

    val ifWord = new Word(Some("if"), None)
    val factorialWord = new Word(Some("factorial"), None)
    val factorialAnon1Word = new Word(Some("factorial$anon1"), None)
    val factorialAnon2Word = new Word(Some("factorial$anon1"), None)

    m.words + ifWord
    m.words + factorialWord
    m.words + factorialAnon1Word
    m.words + factorialAnon2Word

    ifWord.quotation = Some(Quotation(
      SelectInst(QuotationType),
      ApplyInst
    ))

    factorialWord.quotation = Some(Quotation(
      ShuffleInst(List(i32), List(0, 0)), // dup i32
      PushInst(i32, 1),
      ICmpInst(SLECond, i32),
      PushInst(QuotationType, factorialAnon1Word),
      PushInst(QuotationType, factorialAnon2Word),
      PushInst(QuotationType, ifWord),
      ApplyInst
    ))

    factorialAnon1Word.quotation = Some(Quotation(
      ShuffleInst(List(i32), List()), // drop i32
      PushInst(i32, 1)
    ))

    factorialAnon2Word.quotation = Some(Quotation(
      ShuffleInst(List(i32), List(0, 0)), // dup i32
      PushInst(i32, 1),
      ShuffleInst(List(i32, i32), List(1, 0)), // swap i32 i32
      SubInst(i32),
      PushInst(QuotationType, factorialWord),
      ApplyInst,
      MulInst(i32)
    ))

    m
  }

  @Test
  def testAssemble: Unit = {
    println(Disassembler.disassemble(testModule))
  }

  @Test
  def testEmitCPlusPlus: Unit = {
    println(CPlusPlusEmitter.emit(testModule))
  }

}
