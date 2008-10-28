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
          val word = new Word(Some(name), false, None)
          module.words + word
          wordsToCompile + cwid
          word
        }
      }
    }

    val mainWord = getWord(ConcreteWordId(wid, args))

    while (!wordsToCompile.isEmpty) {
      val cwid = wordsToCompile.elements.next
      try {
        wordsToCompile - cwid

        val genericWord = dict(cwid.wid).getOrElse(error("Word not defined: " + cwid.wid))
        val instructionWriter = genericWord.specialize(cwid.args)

        class SimpleInstructionBuffer extends InstructionBuffer {
          private val data = new mutable.ArrayBuffer[Instruction]
          def append(inst: Instruction): Unit = {
            data + inst
          }
          def pushQuotation(wid: GenericWord.Id, args: GenericWord.Args): Unit = {
            val cwid = ConcreteWordId(wid, args)
            data + PushInst(QuotationType, getWord(cwid))
          }
          def toList = data.toList
        }

        val buf = new SimpleInstructionBuffer
        instructionWriter.write(buf)
      
        val word = getWord(cwid)
        word.quotation = Some(Quotation(buf.toList.toArray: _*))
        word.extern = genericWord.extern
        wordsToCompile - cwid // XXX: redundant?
      } catch {
        case e: Exception => throw new Exception("Error compiling: " + cwid, e)
      }
    }

    mainWord
  }

}
