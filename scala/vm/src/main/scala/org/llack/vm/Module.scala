/* -*- Mode: Scala; indent-tabs-mode: nil; tab-width: 2 -*- */

package org.llack.vm

import scala.collection._

class Module {
  var moduleIdentifier: Option[String] = None
  var dataLayout: Option[String] = None
  var targetTriple: Option[String] = None
  val declarations = new mutable.HashMap[String, Type]
  val words = new mutable.ArrayBuffer[Word]

  def getWord(name: String): Option[Word] = {
    words.find { (word: Word) => word.name == Some(name) }
  }
}
