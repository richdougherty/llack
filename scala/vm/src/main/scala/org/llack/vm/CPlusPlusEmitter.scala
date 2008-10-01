/* -*- Mode: Scala; indent-tabs-mode: nil; tab-width: 2 -*- */

package org.llack.vm

import scala.collection._

object CPlusPlusEmitter {
  def emit(mod: Module): String = {
    val builder = new StringBuilder

    var indent = 0
    def p(s: String) = builder.append(s)
    def pindent = for (_ <- 0 until indent) { p("  ") }
    def pln(s: String) = { pindent ; p(s) ; p("\n") }
    def pspace = p("\n")

    def typeString(t: Type): String = t match {
      case IntegerType(n) => "IntegerType::get()"
      case QuotationType => "QuotationType::get()"
      case other => error("Cannot emit C++ code for: " + other)
    }
    def valueString(t: Type, v: Any): String = (t, v) match {
      case (IntegerType(n), i) => "ConstantInt::get(APInt(" + n + ",  \"" + i +  "\", 10))"
      case (QuotationType, w: Word) => "word_" + w.name.get
      case other => error("Cannot emit C++ code for: " + other)
    }
    def iCondString(ic: IntegerCondition): String = ic match {
      case SLECond => "ICmpInst::ICMP_SLE"
      case other => error("Cannot emit C++ code for: " + other)
    }


    pln("using namespace llack;")
    pln("Module* mod = new Module(\"" + mod.moduleIdentifier.get + "\");")
    pln("mod->setDataLayout(\"" + mod.dataLayout.get + "\");")
    pln("mod->setTargetTriple(\"" + mod.targetTriple.get + "\");")

    for (word <- mod.words) {
      pln("Word* word_" + word.name.get + " = Word::Create(\"" + word.name.get + "\");")
    }

    for (word <- mod.words) {
      pspace
      pln("{")
      indent += 1
      pln("// " + word.name.get)
      pln("Word* word = word_" + word.name.get + ";")
      for (inst <- word.quotation.getOrElse(Quotation()).instructions) {
        pln("{")
        indent += 1
        pln("// " + inst.toString)
        def pinst(s: String) = pln("Instruction* inst = " + s + ";")
        inst match {
          case ShuffleInst(ts, is) => {
            pln("std::vector<Type*> consumption;")
            for (t <- ts) { pln("consumption.push_back(" + typeString(t) + ");") }
            pln("std::vector<int> production;")
            for (i <- is) { pln("production.push_back(" + i + ");") }
            pinst("new ShuffleInst(consumption, production)")
          }
          case SelectInst(t) => pinst("new SelectInst(" + typeString(t) + ")")
          case AddInst(t) => pinst("new AddInst(" + typeString(t) + ")")
          case MulInst(t) => pinst("new MulInst(" + typeString(t) + ")")
          case SubInst(t) => pinst("new SubInst(" + typeString(t) + ")")
          case ApplyInst => pinst("new ApplyInst()")
          case ICmpInst(ic, t) => pinst("new ICmpInst(" + iCondString(ic) + ", " + typeString(t) + ")")
          case PushInst(t, v) => {
            pln("Value* value = " + valueString(t, v) + ";")
            pinst("new PushInst(" + typeString(t) + ", value)")
          }
          case other => error("Cannot emit C++ code for: " + other)
        }
        pln("word->instructions.push_back(inst)")
        indent -= 1
        pln("}")
      }
      indent -= 1
      pln("}")
    }

    builder.toString
  }
}
