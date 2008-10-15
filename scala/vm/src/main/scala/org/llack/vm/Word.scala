/* -*- Mode: Scala; indent-tabs-mode: nil; tab-width: 2 -*- */


package org.llack.vm

import scala.collection._

// Equivalent of LLVM's function.

class Word(var name: Option[String], var extern: Boolean, var quotation: Option[Quotation]) extends GlobalValue {
  def this() = this(None, false, None)

  override def toString = "@" + name.getOrElse("unnamedword");
}
