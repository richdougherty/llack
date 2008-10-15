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

trait LinearTerm
case class QuotationLTerm(wid: GenericWord.Id, termArgs: List[TermArgument]) extends LinearTerm
case class LiteralLTerm(value: Any, typ: TermArgument) extends LinearTerm
case object ToContLTerm extends LinearTerm

object Term {

  private class VariableRewriter {
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

    def rewriteOldFTerm[T <: FlattenedTerm](term: T): T = (term match {
      case ApplyFTerm => ApplyFTerm
      case LiteralFTerm(value, typ) => LiteralFTerm(value, rewriteOldArg(typ))
      case QuotationFTerm(wid, termArgs) => QuotationFTerm(wid, termArgs.map(rewriteOldArg(_)))
    }).asInstanceOf[T]

    def rewrittenArgs: List[TermArgument] = {
      (for (newIndex <- 0 until newCount) yield {
        val oldIndex = newToOld(newIndex)
        VariableArgument(oldIndex)
      }).toList
    }
  }

  private def addChild(dict: Dictionary, parentWid: GenericWord.Id, child: QuotationTerm): QuotationFTerm = {
    // Rewrite variables
    val rewriter = new VariableRewriter
    val rewrittenChild = rewriter.rewriteOldTerm(child)

    // Add to dictionary
    val childWid = dict.freshWordId(Some(parentWid + "$q"))
    addToDictionary(dict, childWid, false, rewrittenChild)

    // Return reference for parent, with appropriate variables
    QuotationFTerm(childWid, rewriter.rewrittenArgs)
  }

  private def flatten(dict: Dictionary, parentWid: GenericWord.Id, parent: QuotationTerm): List[FlattenedTerm] = {
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

  private def addFlattenedToDictionary(dict: Dictionary, wid: GenericWord.Id, rootWid: GenericWord.Id, makeExtern: Boolean, flattened: List[FlattenedTerm]): Unit = {
    val (beforeApply, restWithApply) = flattened.break(_ == ApplyFTerm)
    val callRestLTerms = restWithApply match {
      case Nil => Nil
      case ApplyFTerm :: Nil => {
        List(
          ToContLTerm
        )
      }
      case ApplyFTerm :: rest => {
        val rewriter = new VariableRewriter
        val rewrittenRest = for (fTerm <- rest) yield { rewriter.rewriteOldFTerm(fTerm) }

        val restWid = dict.freshWordId(Some(rootWid + "$k"))
        addFlattenedToDictionary(dict, restWid, rootWid, false, rewrittenRest)
        // The following sequence will call the quotation currently
        // on the data stack, then call the word containing the
        // rest of the terms in this flattened quotation.
        List(
          QuotationLTerm(restWid, rewriter.rewrittenArgs),
          ToContLTerm,
          ToContLTerm
        )
      }
    }
    val linear = (beforeApply.map {
      case QuotationFTerm(wid, termArgs) => QuotationLTerm(wid, termArgs)
      case LiteralFTerm(value, typ) => LiteralLTerm(value, typ)
    }) ++ callRestLTerms

    val genericWord = new GenericWord {
      def extern = makeExtern
      def specialize(args: GenericWord.Args) = new InstructionWriter {
        def write(buf: InstructionBuffer) = {
          def substituteArg(termArg: TermArgument): Any = termArg match {
            case ConstantArgument(v) => v
            case VariableArgument(i) => args(i)
          }
          for (term <- linear) term match {
            case QuotationLTerm(wid, termArgs) => {
              val substitutedArgs =  termArgs.map(substituteArg(_))
              buf.pushQuotation(wid, substitutedArgs)
            }
            case ToContLTerm => {
              buf.append(ToContInst)
            }
            case LiteralLTerm(value, typ) => substituteArg(typ) match {
              case substitutedType: Type => buf.append(PushInst(substitutedType, value))
              case other => error("LiteralTerm expects a Type argument: " + other)
            }
          }
        }
      }
    }
    dict(wid) = genericWord
  }

  def addToDictionary(dict: Dictionary, wid: GenericWord.Id, makeExtern: Boolean, quotation: QuotationTerm): Unit = {
    val flattened = flatten(dict, wid, quotation)
    val linear = addFlattenedToDictionary(dict, wid, wid, makeExtern, flattened)
    ()
  }
}
