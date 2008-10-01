package org.llack.compiler

import scala.collection._

object Compiler {

  sealed trait Type
  case object VoidType extends Type
  case object FloatType extends Type
  case object DoubleType extends Type
  case object FP128Type extends Type
  case object X86_FP80Type extends Type
  case object PPC_FP128Type extends Type
  case object LabelType extends Type
  case object QuotationType extends Type
  case class IntegerType(n: Int) extends Type
  case class ArrayType(n: Int, t: Type) extends Type {
    //assert(t.hasSize)
  }
  case class FunctionType(rt: Type, params: Type*) extends Type {
    //assert(t.isScalar || t == VoidType || t.instanceOf[Struct])
    //TODO: attribute params
    //TODO: varargs
  }
  case class StructType(members: Type*) extends Type {
    //assert(members.forall(_.hasSize))
  }
  case class PackedStructType(members: Type*) extends Type {
    //assert(members.forall(_.hasSize))
  }
  case class PointerType(t: Type, as: Int) extends Type {
    def this(t: Type) = this(t, 0)
  }
  case class VectorType(n: Int, t: Type) extends Type {
    //assert(t.isInteger || t.isFloatingPoint)
    def isFirstClass = true
  }
  case object OpaqueType extends Type

  // Java runtime ///////////////////

  sealed trait ByteCode
  // Primitives
  case class Push(value: Any) extends ByteCode
  case object Drop extends ByteCode
  case object Dup extends ByteCode
  case object Swap extends ByteCode
  case object D2R extends ByteCode
  case object R2D extends ByteCode
  // Conditional
  case object Choose extends ByteCode
  // Control flow
  case object Apply extends ByteCode
  case object Return extends ByteCode
  // Primitive library
  case object Add extends ByteCode
  case object Mul extends ByteCode
  case object Dump extends ByteCode

  // Use ArrayStack when released
  class MutableStack[A] {
    var internal = new immutable.Stack[A]
    def push(a: A): Unit = {
      internal = internal.push(a)
    }
    def pop: A = {
      val a = internal.top
      internal = internal.pop
      a
    }
    def isEmpty = internal.isEmpty
    override def toString = internal.toString
  }

  class VMState {
    var ip: Int = 0
    val data = new MutableStack[Any]
    val retain = new MutableStack[Any]
    val return_ = new MutableStack[Int]
  }

  def run(byteCode: RandomAccessSeq[ByteCode], state: VMState): Unit = {
    import state._
    while (ip >= 0) {
      // Get current bytecode then eagerly advance IP address, popping return stack to allow tail recursion.
      val currentByteCode = byteCode(ip)
      println("Running " + ip + ": " + currentByteCode)
      ip += 1
      if (byteCode(ip) == Return) {
        if (return_.isEmpty) {
          ip = -1
        } else {
          ip = return_.pop.asInstanceOf[Int]
        }
      }
      currentByteCode match {
        case Return => error("Should have been eagerly popped.")
        case Push(value) => data.push(value)
        case Drop => data.pop
        case Dup => {
          val a = data.pop
          data.push(a)
          data.push(a)
        }
        case Swap => {
          val a = data.pop
          val b = data.pop
          data.push(a)
          data.push(b)
        }
        case D2R => retain.push(data.pop)
        case R2D => data.push(retain.pop)
        //case Jump(newIp) => ip = newIp
        case Apply => {
          return_.push(ip)
          ip = data.pop.asInstanceOf[Int]
        }
        case Choose => {
          val falseValue = data.pop
          val trueValue = data.pop
          val bool = data.pop.asInstanceOf[Boolean]
          val result = if (bool) trueValue else falseValue
          data.push(result)
        }
        case Add => {
          val a = data.pop.asInstanceOf[Int]
          val b = data.pop.asInstanceOf[Int]
          val result = a + b
          data.push(result)
        }
        case Mul => {
          val a = data.pop.asInstanceOf[Int]
          val b = data.pop.asInstanceOf[Int]
          val result = a * b
          data.push(result)
        }
        case Dump => {
          //println("-- Bytecode --")
          //for (i <- 0 until byteCode.length) { println(i + ": " + byteCode(i)) }
          println("-- State --")
          println("IP: " + ip)
          println("Data: " + data)
          println("Retain: " + retain)
          println("Return: " + return_)
          println("----")
        }
      }
    }
  }

  // compiler ///////////////////

  sealed trait IntegerCondition
  case object EQCondition extends IntegerCondition
  case object NECondition extends IntegerCondition
  case object UGTCondition extends IntegerCondition
  case object ULTCondition extends IntegerCondition
  case object ULECondition extends IntegerCondition
  case object SGTCondition extends IntegerCondition
  case object SGECondition extends IntegerCondition
  case object SLTCondition extends IntegerCondition
  case object SLECondition extends IntegerCondition

  type WordId = String

  trait ByteCodeBuffer {
    def append(bc: ByteCode): Unit
    def appendCall(wid: WordId, args: List[Any]): Unit
  }

  trait ByteCodeWriter {
    def write(buf: ByteCodeBuffer): Unit
  }

  class Dictionary {
    private var nextId = 0
    private val m = new mutable.HashMap[WordId, List[Any] => ByteCodeWriter]
    def freshWordId(hint: Option[String]): WordId = {
      val wid = hint.getOrElse("") + "$anon" + nextId
      nextId += 1
      wid
    }
    def update(wid: WordId, bcwf: List[Any] => ByteCodeWriter): Unit = {
      if (m.contains(wid)) throw new Exception("Already exists: " + wid)
      m(wid) = bcwf
    }
    def apply(wid: WordId): Option[List[Any] => ByteCodeWriter] = {
      m.get(wid)
    }
  }

  class Compiler {
    def compile(wid: WordId, dict: Dictionary): RandomAccessSeq[ByteCode] = {
      val bcArray = new mutable.ArrayBuffer[ByteCode]

      type Address = Int

      case class ConcreteWordId(wid: WordId, args: List[Any])

      val wordAddressMap = new mutable.HashMap[ConcreteWordId, Address]
      val addressPatches = new MutableStack[(Address, ConcreteWordId)]

      def currentAddress: Address = bcArray.length

      val buf = new ByteCodeBuffer {
        def append(bc: ByteCode): Unit = {
          bcArray + bc
        }
        def appendCall(wid: WordId, args: List[Any]): Unit = {
          val cwid = ConcreteWordId(wid, args)
          addressPatches.push((currentAddress, cwid))
          bcArray + null
          bcArray + Apply
        }
      }

      def writeWord(cwid: ConcreteWordId) = {
        wordAddressMap(cwid) = currentAddress
        val bcwf = dict(cwid.wid).getOrElse(error("word not defined: " + cwid.wid))
        val bcw = bcwf(cwid.args)
        bcw.write(buf)
        buf.append(Return)
      }

      writeWord(ConcreteWordId(wid, Nil))

      while (!addressPatches.isEmpty) {
        val (patchAddress, cwid) = addressPatches.pop
        if (!wordAddressMap.contains(cwid)) {
          writeWord(cwid)
        }
        val wordAddress = wordAddressMap(cwid)
        bcArray(patchAddress) = Push(wordAddress)
      }
      
      bcArray
    }
  }


  //def translate(wordDef: WordDefinition): Block

  // compile a word with the given args
  // get back some bytecode along with a list of words that need their addresses patched
  //def compile(wid: WordId, args: List[Any]): (RandomAccessSeq[ByteCode], List[(WordId,Int)])

  sealed trait WordArgument
  case class VariableArgument(v: Int) extends WordArgument
  case class LiteralArgument(v: Any) extends WordArgument

/*
  sealed trait BlockElement
  case class Literal(v: Any, t: Type) extends BlockElement
  case class Word(wid: WordId, args: List[WordArgument]) extends BlockElement
*/

  def main(args: Array[String]): Unit = {

    val i32 = IntegerType(32)

    val dictionary = new Dictionary
    dictionary("dup") = {
      case (t:Type)::Nil => new ByteCodeWriter {
        def write(buf: ByteCodeBuffer) = buf.append(Dup)
      }
      case args => error("dup: expected a single type: " + args)
    }
    dictionary("?") = {
      case (t:Type)::Nil => new ByteCodeWriter {
        def write(buf: ByteCodeBuffer) = buf.append(Choose)
      }
      case args => error("?: expected a single type: " + args)
    }
    dictionary("add") = { 
      case IntegerType(n)::Nil => new ByteCodeWriter {
        def write(buf: ByteCodeBuffer) = buf.append(Add)
      }
      case args => error("add: expected an integer type: " + args)
    }
    dictionary("mul") = { 
      case IntegerType(n)::Nil => new ByteCodeWriter {
        def write(buf: ByteCodeBuffer) = buf.append(Mul)
      }
      case args => error("mul: expected an integer type: " + args)
    }
    dictionary("apply") = {
      case Nil => new ByteCodeWriter {
        def write(buf: ByteCodeBuffer) = buf.append(Apply)
      }
      case args => error("apply: expected no args: " + args)
    }
    dictionary("dump") = {
      case Nil => new ByteCodeWriter {
        def write(buf: ByteCodeBuffer) = buf.append(Dump)
      }
      case args => error("dump: expected no args: " + args)
    }

    dictionary("if") = { 
      case Nil => new ByteCodeWriter {
        def write(buf: ByteCodeBuffer) = {
          buf.appendCall("?", List(QuotationType))
          buf.appendCall("apply", Nil)
        }
      }
      case args => error("if: expected no args: " + args)
    }
    dictionary("f") = { 
      case Nil => new ByteCodeWriter {
        def write(buf: ByteCodeBuffer) = {
          buf.appendCall("add", List(i32))
          buf.appendCall("mul", List(i32))
        }
      }
      case args => error("f: expected no args: " + args)
    }
    dictionary("main") = { 
      case Nil => new ByteCodeWriter {
        def write(buf: ByteCodeBuffer) = {
          buf.append(Push(2))
          buf.append(Push(5))
          buf.append(Push(12))
          buf.appendCall("f", Nil)
          buf.appendCall("dump", Nil)
        }
      }
      case args => error("main: expected no args: " + args)
    }
    
    val bcArray = (new Compiler).compile("main", dictionary)
    println(bcArray)
    run(bcArray, new VMState)


    /*
  Definition("if", Quotation(
    Word("?", List(QuotationType)),
    Word("apply")
  )

  Definition("factorial", Quotation(
    Word("dup", List(i32)), Literal(0, i32), Word("icmp", List(SLECondition, i32)),
    Quotation(
      Word("drop", List(i32))
    ),
    Quotation(
      Word("dup", List(i32)), Literal(1, i32), Word("sub", List(i32)), Word("mul", List(i32)), Word("factorial")
    ),
    Word("if")
  )
*/

    ()
  }



}
