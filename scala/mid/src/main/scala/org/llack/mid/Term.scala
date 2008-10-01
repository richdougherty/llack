/* -*- Mode: Scala; indent-tabs-mode: nil; tab-width: 2 -*- */

package org.llack.mid

import scala.collection._
import org.llack.vm._

trait TermArgument
case class ConstantArgument(v: Any) extends TermArgument
case class VariableArgument(i: Int) extends TermArgument

trait Term
case class WordTerm(wid: GenericWord.Id, termArgs: List[TermArgument]) extends Term
case class LiteralTerm(value: Any, typ: TermArgument) extends Term
case class QuotationTerm(terms: List[Term]) extends Term

trait FlattenedTerm
case class QuotationFTerm(wid: GenericWord.Id, termArgs: List[TermArgument]) extends FlattenedTerm
case class LiteralFTerm(value: Any, typ: TermArgument) extends FlattenedTerm
case object ApplyFTerm extends FlattenedTerm

object Term {

  def substituteArg(termArg: TermArgument, args: GenericWord.Args): Any = termArg match {
    case ConstantArgument(v) => v
    case VariableArgument(i) => args(i)
  }

  def createGenericWord(shouldInline: Boolean, quotation: List[FlattenedTerm]): GenericWord =
    new GenericWord {
      def inline = shouldInline
      def specialize(args: GenericWord.Args) = new InstructionWriter {
        def write(buf: InstructionBuffer) = {
          for (term <- quotation) term match {
            case QuotationFTerm(wid, termArgs) => {
              val substitutedArgs =  termArgs.map(substituteArg(_, args))
              buf.pushQuotation(wid, substitutedArgs)
            }
            case ApplyFTerm => {
              buf.append(ApplyInst)
            }
            case LiteralFTerm(value, typ) => substituteArg(typ, args) match {
              case substitutedType: Type => buf.append(PushInst(substitutedType, value))
              case other => error("LiteralTerm expects a Type argument: " + other)
            }
          }
        }
      }
    }

  class VariableRewriter {
    val oldToNew = new mutable.HashMap[Int, Int]
    val newToOld = new mutable.HashMap[Int, Int]
    var newCount = 0

    def rewriteOldArg(termArg: TermArgument) = termArg match {
      case VariableArgument(oldIndex) => oldToNew.get(oldIndex) match {
        case Some(newIndex) => VariableArgument(newIndex)
        case None => {
          val newIndex = newCount
          newCount += 1
          oldToNew(oldIndex) = newIndex
          newToOld(newIndex) = oldIndex
          VariableArgument(newIndex)
        }
      }
      case other => other
    }

    def rewriteOldTerm[T <: Term](term: T): T = (term match {
      case WordTerm(wid, termArgs) => WordTerm(wid, termArgs.map(rewriteOldArg(_)))
      case LiteralTerm(value, typ) => LiteralTerm(value, rewriteOldArg(typ))
      case QuotationTerm(terms) => QuotationTerm(terms.map(rewriteOldTerm(_)))
    }).asInstanceOf[T]

    def rewrittenArgs: List[TermArgument] = {
      (for (newIndex <- 0 until newCount) yield {
        val oldIndex = newToOld(newIndex)
        VariableArgument(oldIndex)
      }).toList
    }
  }

  def addChild(dict: Dictionary, parentWid: GenericWord.Id, child: QuotationTerm): QuotationFTerm = {
    // Rewrite variables
    val rewriter = new VariableRewriter
    val rewrittenChild = rewriter.rewriteOldTerm(child)

    // Add to dictionary
    val childWid = dict.freshWordId(Some(parentWid))
    addToDictionary(dict, childWid, false, rewrittenChild)

    // Return reference for parent, with appropriate variables
    QuotationFTerm(childWid, rewriter.rewrittenArgs)
  }

  def flatten(dict: Dictionary, parentWid: GenericWord.Id, parent: QuotationTerm): List[FlattenedTerm] = {
    parent.terms.flatMap {
      case child: QuotationTerm => {
        addChild(dict, parentWid, child) :: Nil
      }
      case WordTerm(wid, termArgs) => {
        QuotationFTerm(wid, termArgs) :: ApplyFTerm :: Nil
      }
      case LiteralTerm(value, typ) => LiteralFTerm(value, typ) :: Nil
    }.asInstanceOf[List[FlattenedTerm]] // XXX: Shouldn't need cast
  }

  def addToDictionary(dict: Dictionary, wid: GenericWord.Id, inline: Boolean, quotation: QuotationTerm): Unit = {
    val flattened = flatten(dict, wid, quotation)
    val word = createGenericWord(inline, flattened)
    dict(wid) = word
  }
}
