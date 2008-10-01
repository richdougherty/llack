/* -*- Mode: Scala; indent-tabs-mode: nil; tab-width: 2 -*- */

package org.llack.mid

import scala.collection._
import org.llack.vm._

object GenericWord {
  type Id = String
  type Args = List[Any]
}

trait GenericWord {
  def inline: Boolean
  def specialize(args: GenericWord.Args): InstructionWriter
}

trait InstructionBuffer {
  def append(inst: Instruction): Unit
  def pushQuotation(wid: GenericWord.Id, args: GenericWord.Args): Unit
}

trait InstructionWriter {
  def write(buf: InstructionBuffer): Unit
}

class Dictionary {
  private var nextId = 0
  private val m = new mutable.HashMap[GenericWord.Id, GenericWord]
  def freshWordId(hint: Option[String]): GenericWord.Id = {
    val wid = hint.getOrElse("") + "$anon" + nextId
    nextId += 1
    wid
  }
  def update(wid: GenericWord.Id, genericWord: GenericWord): Unit = {
    if (m.contains(wid)) throw new Exception("Already exists: " + wid)
    m(wid) = genericWord
  }
  def apply(wid: GenericWord.Id): Option[GenericWord] = {
    m.get(wid)
  }

}
