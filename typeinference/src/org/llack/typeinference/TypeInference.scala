package org.llack.typeinference

import scala.collection._

/*
Ruler: programming type rules 
Atze Dijkstra S. Doaitse Swierstra 
Department of Information and Computing Sciences 
Utrecht University
*/

object TypeInference {

  // misc

  def lookup[A, B](k: A, list: List[(A, B)]): Option[B] = {
    list.find(_._1 == k).flatMap { case (_, v) => Some(v) }
  }

  ///////////////

  type TVarName = Int

  sealed trait Typ
  case object TInt extends Typ {
    override def toString = "int"
  }
  case class TFn(a: Typ, b: Typ) extends Typ {
    override def toString = "(" + a + " -> " + b + ")"
  }
  case class TV(n: TVarName) extends Typ {
    override def toString = "t" + n
  }
  sealed trait TSt extends Typ with Iterable[Typ] {
    def toList: List[Typ]
    def elements = toList.elements
    override def toString = toList.mkString("{", " ", "}")
  }
  case class TStCons(r: TSt, t: Typ) extends TSt {
    override def toList = r.toList + t
  }
  case class TStV(n: TVarName) extends TSt {
    override def toList = this :: Nil
    override def toString = "s" + n
  }

  //case class TypS(vs: List[TVarName], t: Typ)

  type VarName = String

  sealed trait Term
  case class V(n: VarName) extends Term {
    override def toString: String = n.toString
  }
  case class L(n: VarName, t: Term) extends Term
  case class A(t1: Term, t2: Term) extends Term
  case class I(v: Int) extends Term {
    override def toString: String = v.toString
  }
  case class :+(t1: Term, t2: Term) extends Term {
    override def toString: String = "(" + t1 + " + " + t2 + ")"
  }
  case class IFZ(t1: Term, t2: Term, t3: Term) extends Term
  case class Fix(t: Term) extends Term

  sealed trait Word extends Term
  case class SW(t: TFn) extends Word
  sealed trait Quotation extends Word
  case class QCons(w: Word, q: Quotation) extends Quotation
  case object QNil extends Quotation
  //case class Let(n: VarName, t: Term, b: Term)

  type TEnv = List[(VarName, Typ)] // should be Map[VarName,Typ], but maybe being an ordered bag is important
  // or... type TEnv = Map[VarName, Typ]

  val env0: TEnv = Nil

  def lkup(env: TEnv, x: VarName): Typ = {
    lookup(x, env).getOrElse { throw new Exception("Unbound variable: " + x) }
  }
  // or... env(x)

  def ext(env: TEnv, xt: (VarName, Typ)) = xt :: env
  // or... env(x) + xt

  case class TVE(n: Int, s: immutable.Map[TVarName, Typ])

  def newtv(tve: TVE): (Typ, TVE) = (TV(tve.n), TVE(tve.n + 1, tve.s))
  def newstv(tve: TVE): (TSt, TVE) = (TStV(tve.n), TVE(tve.n + 1, tve.s))

  def tve0: TVE = TVE(0, immutable.Map.empty)

  def tvlkup(tve: TVE, v: TVarName): Option[Typ] = tve.s.get(v)

  def tvext(tve: TVE, xt: (TVarName, Typ)): TVE = TVE(tve.n, tve.s(xt._1) = xt._2)

  def tvsub(tve: TVE, t: Typ): Typ = t match {
    case TFn(t1, t2) => TFn(tvsub(tve, t1), tvsub(tve, t2))
    case TV(v) => {
      tvlkup(tve, v) match {
        case None => t
        case Some(t2) => tvsub(tve, t2)
      }
    }
    case st: TSt => tvstacksub(tve, st)
    case _ => t
  }

  def tvstacksub(tve: TVE, st: TSt): TSt = st match {
    case TStCons(sv, t) => TStCons(tvstacksub(tve, sv), tvsub(tve, t))
    case TStV(v) => tvlkup(tve, v) match {
      case None => TStV(v) // unchanged
      case Some(t2) => t2 match {
        case st3: TSt => tvstacksub(tve, st3)
        case _ => error("couldn't sub stack var " + v + " with non-stack type: " + t2)
      }
    }
  }

  def tvchase(tve: TVE, t: Typ): Typ = t match {
    case TV(v) => {
      tvlkup(tve, v) match {
        case None => t
        case Some(t2) => tvchase(tve, t2)
      }
    }
    case _ => t
  }

  def id[A] = (a: A) => a

  def unify(t1: Typ, t2: Typ, tve: TVE): TVE = {
    println(t1 + " <=> " + t2)
    val tve1 = unify1(tvchase(tve, t1), tvchase(tve, t2), tve)
    println("TVE: " + tve1)
    tve1
  }

  //def unifyEx(t1: Typ, t2: Typ, tve: TVE): TVE =
  //  unify(t1, t2, tve).fold(error(_), id[TVE])

  def unify1(t1: Typ, t2: Typ, tve: TVE): TVE = (t1, t2) match {
    case (TInt, TInt) => tve
    case (TFn(t1a, t1r), TFn(t2a, t2r)) => {
      val tve1 = unify(t1a, t2a, tve)
      val tve2 = unify(t1r, t2r, tve1)
      tve2
    }
    case (TV(v1), t2) => unifyv(v1, t2, tve)
    case (t1, TV(v2)) => unifyv(v2, t1, tve)
    case (TStCons(st1, t1), TStCons(st2, t2)) => {
      val tve1 = unify(t1, t2, tve)
      val tve2 = unify(st1, st2, tve1)
      tve2
    }
    case (TStV(v1), st2: TSt) => unifysv(v1, st2, tve)
    case (st1: TSt, TStV(v2)) => unifysv(v2, st1, tve)
    case (t1, t2) => error("constant mismatch : " + t1 + " and " + t2)
  }
  
  def unifyv(v: TVarName, t: Typ, tve: TVE): TVE = (v, t) match {
    case (v1, TV(v2)) if (v1 == v2) => tve // trivial
    case (v1, TV(v2)) => tvext(tve, (v1, TV(v2)))
    case (v1, t2) if (occurs(v1, t2, tve)) => error("occurs check: " + v1 + " in " + tvsub(tve, t2))
    case (v1, t2) => tvext(tve, (v1, t2))
  }

  def unifysv(v: TVarName, st: TSt, tve: TVE): TVE = (v, st) match {
    case (v1, TStV(v2)) if (v1 == v2) => tve // trivial
    case (v1, TStV(v2)) => tvext(tve, (v1, TStV(v2)))
    case (v1, t2) if (occurs(v1, t2, tve)) => error("occurs check: " + v1 + " in " + tvsub(tve, t2))
    case (v1, t2) => tvext(tve, (v1, t2))
  }

  def occurs(v: TVarName, t: Typ, tve: TVE): Boolean = (v, t) match {
    case (v, TInt) => false
    case (v, TFn(t1, t2)) => occurs(v, t1, tve) || occurs(v, t2, tve)
    case (v, TV(v2)) => tvlkup(tve, v2) match {
      case None => v == v2
      case Some(t2) => occurs(v, t2, tve)
    }
    case (v, TStV(v2)) => tvlkup(tve, v2) match {
      case None => v == v2
      case Some(t2) => occurs(v, t2, tve)
    }
    case (v, TStCons(st, t1)) => occurs(v, t1, tve) || occurs(v, st, tve)
  }

  // creates function for term, which
  def teval1(env: TEnv, term: Term): (TVE => (Typ, TVE)) = term match {
    case V(x) => { tve0: TVE => (lkup(env, x), tve0) }
    case L(x, e) => { tve0: TVE =>
      val (tv, tve1) = newtv(tve0) // create fresh type
      val (te, tve2) = teval1(ext(env, (x, tv)), e)(tve1) // eval body in env where arg ident is bound to just-made fresh type
      (TFn(tv, te), tve2) // type of arg and body
    }
    case A(e1, e2) => { tve0: TVE =>
      val (t1, tve1) = teval1(env, e1)(tve0) // type for function, which is being applied
      val (t2, tve2) = teval1(env, e2)(tve1) // type for input expression
      val (t1r, tve3) = newtv(tve2) // type for result of application
      val tve4 = unify(t1, TFn(t2, t1r), tve3) // function must take t2 as input
      (t1r, tve4) // type of A is result type of e1
    }
    case I(n) => { tve0: TVE => (TInt, tve0) }
    case :+(e1, e2) => { tve0: TVE =>
      val (t1, tve1) = teval1(env, e1)(tve0)
      val (t2, tve2) = teval1(env, e2)(tve1)
      val tve3 = unify(t2, TInt, tve2)
      val tve4 = unify(t1, TInt, tve3)
      (TInt, tve4)
    }
    case IFZ(e1, e2, e3) => { tve0: TVE =>
      val (t1, tve1) = teval1(env, e1)(tve0)
      val (t2, tve2) = teval1(env, e2)(tve1)
      val (t3, tve3) = teval1(env, e3)(tve2)
      val tve4 = unify(t1, TInt, tve3)
      val tve5 = unify(t2, t3, tve4)
      (t2, tve5)
    }
    case Fix(e) => { tve0: TVE =>
      val (t, tve1) = teval1(env, e)(tve0)
      val (ta, tve2) = newtv(tve1)
      val (tb, tve3) = newtv(tve2)
      val tfn = TFn(ta, tb)
      val tve4 = unify(t, TFn(tfn, tfn), tve3)
      (tfn, tve4)
    }
    case SW(t) => { tve0: TVE => renametvs(t, tve0) }
    case QNil => { tve0: TVE =>
      val (t, tve1) = newtv(tve0)
      (TFn(t, t), tve1)
    }
    case QCons(w, q) => { tve0: TVE =>
      val (wt, tve1) = teval1(env, w)(tve0)
      val (qt, tve2) = teval1(env, q)(tve1)
      println("Composing: " + w + " and " + q)
      println("Type of word: " + wt)
      println("Type of remaining quote: " + qt)
      (wt, qt) match {
        case (TFn(wc, wp), TFn(qc, qp)) => {
          val tve3 = unify(wp, qc, tve2)
          (TFn(wc, qp), tve3)
        }
        case _ => error("Expected types of word and rest of quotation to be functions: " + wt + ", " + qt)
      }
    }
                  /*
    case Q(Nil) => {
      val (ta, tve1) = newtv(tve0)
      val (tb, tve2) = newtv(tve1)
      
    }*/
    case _ => throw new MatchError
  }

  def teval(env: TEnv, e: Term): Typ = {
    val (t, tve) = teval1(env, e)(tve0)
    tvsub(tve, t)
  }

//  trait TVEM[A] extends StateMonad[A, TVEM, TVE]

//  class TVEMTyp extends TVEM[Typ] {
//
//    def stateTrans(tve: TVE) = (TVE(tve.nextVar + 1, 
//  }


  def renametvs(t: Typ, tve: TVE): (Typ, TVE) = {
    var tve1 = tve
    var ren = tve0
    def renamev(v: TVarName): Typ = {
      tvlkup(ren, v) match {
        case None => {
          val (t, tve2) = newtv(tve1) // fresh tv from target env
          tve1 = tve2
          ren = tvext(ren, (v, t)) // save mapping from old to new
          //println("Renamed: " + v + " to " + t)
          t
        }
        case Some(t2) => t2
      }
    }
    def renamesv(v: TVarName): Typ = {
      tvlkup(ren, v) match {
        case None => {
          val (t, tve2) = newstv(tve1) // fresh tv from target env
          tve1 = tve2
          ren = tvext(ren, (v, t)) // save mapping from old to new
          //println("Renamed: " + v + " to " + t)
          t
        }
        case Some(t2) => t2
      }
    }
    def renametvs1(t: Typ): Typ = t match {
      case TFn(t1, t2) => TFn(renametvs1(t1), renametvs1(t2))
      case TStCons(st, t1) => TStCons(renametvs1(st).asInstanceOf[TSt], renametvs1(t1))
      case TStV(v) => renamesv(v)
      case TV(v) => renamev(v)
      case _ => t
    }
    //println(ren)
    (renametvs1(t), tve1)
  }

  def printType(e: Term) = {
    println
    println("Type of: " + e)
    println("=> " + teval(env0, e))
  }

  def testUnify(t1: Typ, t2: Typ) = {
    println
    println("Unifying: " + t1 + " = " + t2)
    val (t1a, tve1) = renametvs(t1, tve0)
    val (t2a, tve2) = renametvs(t2, tve1)
    println("  Renamed to: " + t1a + " = " + t2a)
    val tve3 = unify(t1a, t2a, tve2)
    println("  TVE: " + tve3)
    println("Result: " + tvsub(tve3, t1a) + " = " + tvsub(tve3, t2a))
  }

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
        case Dump => {
          println("-- Bytecode --")
          for (i <- 0 until byteCode.length) { println(i + ": " + byteCode(i)) }
          println("-- State --")
          println("IP: " + ip)
          println("Data: " + data)
          println("Retain: " + retain)
          println("Return: " + return_)
        }
      }
    }
  }

  def main(args: Array[String]) = {

    val (t1, tve1) = newtv(tve0)
    val (t2, tve2) = newtv(tve1)

    // s0 <=> s1
    testUnify(TStV(0), TStV(0))
    testUnify(TStCons(TStV(0), TInt), TStCons(TStV(0), TInt))
    // {s0 int} = {s0 t1}
    testUnify(TStCons(TStV(0), TInt), TStCons(TStV(0), TV(1)))
    // {s0 int} = {s2 t1 t0 int}
    testUnify(TStCons(TStV(0), TInt), TStCons(TStCons(TStCons(TStV(2), TV(1)), TV(0)), TInt))
    // {s0 t1} = {s2 t1 t0 int}
    testUnify(TStCons(TStV(0), TV(1)), TStCons(TStCons(TStCons(TStV(2), TV(1)), TV(0)), TInt))
    
    // constantly('A 'b -> 'A ('C -> 'C 'b))
    val constantlyTyp = TFn(
        TStCons(TStV(1), TV(0)), // 'A 'b
        TStCons(
	  TStV(1),
          TFn(TStV(2), TStCons(TStV(2), TV(0))) // ('C -> 'C 'b)
          ))
    // swap ('A 'b 'c -> 'D 'f 'e)
    val swapTyp = TFn(
        TStCons(TStCons(TStV(2), TV(1)), TV(0)),
        TStCons(TStCons(TStV(2), TV(0)), TV(1)))

    testUnify(constantlyTyp.b, swapTyp.a)

    val byteCode = Array(
      Push(1),
      Push(2),
      Add,
      Push(7), // double
      Apply,
      Dump,
      Return,
      // double (int int -> int)
      Dup,
      Dump,
      Add,
      Return
    )

    run(byteCode, new VMState)

    printType(
      SW(TFn(TStV(100), TStV(100)))
    )

    val constantly = SW(constantlyTyp)
    val swap = SW(swapTyp)
    printType(QCons(swap, QNil))
    printType(QCons(constantly, QCons(swap, QNil)))

    // [t1/([[/t4]/] -> [[/t4]/t0])] = [t0/t1 [/t4]]
    // ['A/([[/t4]/] -> [[/t4]/t0])] = [t0/'A [/t4]]
/*
    println("!")
    printType(:+(I(1), I(2)))
    val ifz = IFZ(I(0), I(1), I(2))
    printType(ifz)
    printType(L("x",
          IFZ(I(0), V("x"), I(2))
        ))
    val add2 = L("x",
          :+(V("x"), I(2))
        )
    printType(add2)
    printType(L("x", V("x")))
    */
    printType(
      Fix(
        L("self",
          L("counter",
            IFZ(V("counter"),
                I(0),
                A(V("self"), V("counter"))
              )))))

    printType(
      Fix(
        L("self",
          L("counter",
            IFZ(V("counter"),
                I(0),
                A(V("self"), V("counter"))
              )))))


    /*class Dictionary {
      
      def ge
    }*/

    /*printType(
      A(
        add2,
        I(1)
      )
    )*/
  }

/*

constantly('A 'b -> 'A ('C -> 'C 'b)

let
  add2(int -> int) = 2 add
in
  let
    four(-> int) = 4
  in
    four add2
  end
end

def countdown('A int -> 'A 'B int) [
  dup // 'A 3 3    Word("dup", TSt(TV("a") :: Nil, TV("A") :: Nil), TSt(TV("a") :: TV("a") :: Nil, TV("A") :: Nil)  //('A 'a -> 'A 'a 'a)
  [
    // 'A 3
  ]
  [
    dup // 'A 3 3
    -1 // 'A 3 3 -1
    + // 'A 3 2
    countdown
  ]
  ifz // 'A 3
]

x = 1
y = 1
x + y = ?

1 is an int (-> int)
2 is an int (-> int)
+ is a word that accepts 2 ints and puts 1 out (int int -> int)

1 (-> int)
2 (-> int)
+ (int int -> int)
swap (a b -> b a)


1 (-> int) (int)
2 (-> int) (int int)
swap (a b -> b a) -- what is b, what is a? (int int)
+ (int int -> int) (int)


(a b) = (int int)
a = int
b = int


[1 2 +] (-> int)



swap: takes 2 concepts, one after the other, puts the first one in second place and the second one in first place

swap: take 2 concepts, called 'a' and 'b' and reverses their order
swap: a b changes to b a

dup: x changes to x x

is it safe to run dup and then swap? i.e. does swap take as input, what dup gives as output?
- swap's input is a b
- dups output is x x

yes
if x is type '?', can we work out a and b?
a, b, and x are all the same type if you run dup then swap

what's the type of 'swap swap'?
swap (first second -> second first)


'swap 1 swap'

---- x y
swap (a b -> b a) --- y x
1 (-> int) ---- y x int
swap (f g -> g f) ---- int=f, x=g --- y int x


{A a b} -> {A b a}


average {s0 a: Int b: Int} -> {s0 avg: Int} 
{s0 a: Int b: Int} + {s0 sum: Int} + 2 {s0 sum: Int divisor: Int}


apply(A (A -> B) -> B)

[add_i32] apply
  gives
apply(i32:a i32:b (i32 i32 -> i32):c -> i32:d)

add_i32(%a:i32 %b:i32 -> %c:i32)
  %c = add %a, %b

swap_i8_i32(%a:i8 %b:i32 -> %c:i32 %d:i8)
  %c = %b
  %d = %a

calling a word (rather than inlining)


simple compilation
each word gets a number of specialized methods
each method is given a pointer to the vm
each method reads its own elements off the stack, and writes them back on
always jump, never call/return?

interpreter
executes instructions
instructions either push a literal or call a word

while


{void*, void*, void*}* apply

struct vmstate {
  byte* data = malloc(8192)
  byte* retain = malloc(1024)
  int ip = &main_bytecode;
  int* return = malloc(256 * sizeof(int))
}

void advance(

int main() {
  interpret(&generated_bytecode); // linked
}

void interpret(byte* bytecode) {
  struct vmstate vm;
  vm.data = malloc(8192)
  vm.retain = malloc(1024)
  vm.return = malloc(1024)
  vm.ip = 0;

  for (;;) {
    byte* curr_ip = vm.ip;
    
    // Support tail recursion by eagerly advancing the IP and popping the return stack.
    

    switch (bytecode[ip]) {
      case PUSH1:
        
        ip += 2;
        data = 
        break;
      case JUMP:
        int offset = *(curr_ip + 1);
        ip += (1 + sizeof(i32))
        // support tail-recursion by popping return stack early
        if (bytecode[ip] == RETURN) {
          ip = return
          return -= sizeof(int)
        }
        break;
      case APPLY:
        
        
      case ADD_I32 => {
        
      }
    }
  }
  return 0
}

%1 = 

*/

}
