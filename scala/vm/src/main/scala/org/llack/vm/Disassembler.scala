/* -*- Mode: Scala; indent-tabs-mode: nil; tab-width: 2 -*- */

package org.llack.vm

import scala.collection._

object Disassembler {
  def disassemble(module: Module): String = {
    val builder = new StringBuilder
    builder.append("; " + module.moduleIdentifier.getOrElse("unnamed") + "\n")
    builder.append("\n")

    for (word <- module.words) {
      val externString = if (word.extern) "extern " else ""
      builder.append("define " + externString +  "quot @" + word.name.getOrElse("unnamed") + " {\n")
      for (inst <- word.quotation.getOrElse(Quotation()).instructions) { builder.append("  " + inst.toString + "\n") }
      builder.append("}\n")
      builder.append("\n")
    }

    builder.toString
  }
}
