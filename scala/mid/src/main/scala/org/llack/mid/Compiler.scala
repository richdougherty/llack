/* -*- Mode: Scala; indent-tabs-mode: nil; tab-width: 2 -*- */

package org.llack.mid

import scala.collection._
import org.llack.vm._

object Compiler {

  def compile(wid: GenericWord.Id, args: GenericWord.Args, dict: Dictionary, module: Module): Word = {

    case class ConcreteWordId(wid: GenericWord.Id, args: GenericWord.Args)

    val wordsToCompile = new mutable.HashSet[ConcreteWordId]

    def getWord(cwid: ConcreteWordId) = {
      val name =
        if (cwid.args.isEmpty) cwid.wid
        else cwid.wid + "$" + cwid.args.mkString("", "_", "")
      module.getWord(name) match {
        case Some(word) => word
        case None => {
          val word = new Word(Some(name), None)
          module.words + word
          wordsToCompile + cwid
          word
        }
      }
    }

    val mainWord = getWord(ConcreteWordId(wid, args))

    while (!wordsToCompile.isEmpty) {
      val cwid = wordsToCompile.elements.next
      wordsToCompile - cwid

      val genericWord = dict(cwid.wid).getOrElse(error("Word not defined: " + cwid.wid))
      val instructionWriter = genericWord.specialize(cwid.args)

      case class PushQuotation(wid: GenericWord.Id, args: GenericWord.Args)

      class SimpleInstructionBuffer extends InstructionBuffer {
        private val data = new mutable.ArrayBuffer[Any]
        def append(inst: Instruction): Unit = {
          data + inst
        }
        def pushQuotation(wid: GenericWord.Id, args: GenericWord.Args): Unit = {
          data + PushQuotation(wid, args)
        }
        def toList: List[Any] = data.toList
      }

      def getFixpoint[A](f: A => A, input: A): A = {
        val output = f(input)
        if (output == input) output
        else getFixpoint(f, output)
      }

      def resolveCalls(input: List[Any]): List[Instruction] = input match {
        case PushQuotation(wid, args) :: tail => {
          val cwid = ConcreteWordId(wid, args)
          PushInst(QuotationType, getWord(cwid)) :: resolveCalls(tail)
        }
        case (head: Instruction) :: tail => head :: resolveCalls(tail)
        case Nil => Nil
        case other => error("Cannot resolve calls for: " + other)
      }

      // XXX: Handle inline cycles?
      def inline(input: List[Any]): List[Any] = input match {
        case PushQuotation(wid, args) :: ApplyInst :: tail if dict(wid).get.inline => {
          val inlineBuf = new SimpleInstructionBuffer
          val calleeInstructionWriter = dict(wid).get.specialize(args)
          calleeInstructionWriter.write(inlineBuf)
          inlineBuf.toList ::: tail
        }
        case head :: tail => head :: inline(tail)
        case Nil => Nil
      }

      val buf = new SimpleInstructionBuffer
      instructionWriter.write(buf)
      val inlined = getFixpoint(inline(_: List[Any]), buf.toList)
      val resolved = resolveCalls(inlined)
      val instructions = resolved
      
      val word = getWord(cwid)
      word.quotation = Some(Quotation(instructions.toArray: _*))
      wordsToCompile - cwid
    }

    mainWord
  }

}
