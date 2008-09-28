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
  case class TSt(ss: List[Typ], v: TV) extends Typ {
    override def toString = "[s" + v.n + "/" + ss.reverse.mkString("", " ", "") + "]"
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

  case class TVE(n: Int, s: immutable.Map[Int, Typ])

  def newtv(tve: TVE): (Typ, TVE) = (TV(tve.n), TVE(tve.n + 1, tve.s))

  def tve0: TVE = TVE(0, immutable.Map.empty)

  def tvlkup(tve: TVE, v: TVarName): Option[Typ] = tve.s.get(v)

  def tvext(tve: TVE, xt: (TVarName, Typ)): TVE = TVE(tve.n, tve.s(xt._1) = xt._2)

  def tvsub(tve: TVE, t: Typ): Typ = t match {
    case TFn(t1, t2) => TFn(tvsub(tve, t1), tvsub(tve, t2))
    case TSt(ss, v) => {
      val ssSubbed = ss.map(tvsub(tve, _))
      tvsub(tve, v) match {
        case TV(v2) => TSt(ssSubbed, TV(v2))
        case TSt(ss2, v2) => TSt(ssSubbed ++ ss2, v2)
        case t2 => error("couldn't substitute vector var for " + t + " with " + t2 + ": must be TV or TSt")
      }
    }
    case TV(v) => {
      tvlkup(tve, v) match {
        case None => t
        case Some(t2) => tvsub(tve, t2)
      }
    }
    case _ => t
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
    case (TSt(s1a::s1r, v1), TSt(s2a::s2r, v2)) => { // match scalar vars
      val tve1 = unify(s1a, s2a, tve)
      val tve2 = unify(TSt(s1r, v1), TSt(s2r, v2), tve1)
      tve2
    }
    // match stack vars
    case (TSt(Nil, TV(v1)), TSt(Nil, TV(v2))) => unifyv(v1, TV(v2), tve)
    case (TSt(ss1, TV(v1)), TSt(Nil, TV(v2))) => unifyv(v2, TSt(ss1, TV(v1)), tve)
    case (TSt(Nil, TV(v1)), TSt(ss2, TV(v2))) => unifyv(v1, TSt(ss2, TV(v2)), tve)
    case (t1, t2) => error("constant mismatch : " + t1 + " and " + t2)
  }
  
  def unifyv(v: TVarName, t: Typ, tve: TVE): TVE = (v, t) match {
    case (v1, TV(v2)) if (v1 == v2) => tve // trivial
    case (v1, TV(v2)) => tvext(tve, (v1, TV(v2)))
    case (v1, t2) if (occurs(v1, t2, tve)) => error("occurs check: " + TV(v1) + " in " + tvsub(tve, t2))
    case (v1, t2) => tvext(tve, (v1, t2))
  }

  def occurs(v: TVarName, t: Typ, tve: TVE): Boolean = (v, t) match {
    case (v, TInt) => false
    case (v, TFn(t1, t2)) => occurs(v, t1, tve) || occurs(v, t2, tve)
    case (v, TV(v2)) => tvlkup(tve, v2) match {
      case None => v == v2
      case Some(t2) => occurs(v, t2, tve)
    }
    case (v, TSt(ss, sv)) => ss.contains(occurs(v, _: Typ, tve)) || occurs(v, sv, tve)
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
      val (tb, tve3) = newtv(tve1)
      val tfn = TFn(ta, tb)
      val tve4 = unify(t, TFn(tfn, tfn), tve3)
      (tfn, tve4)
    }
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
    def renametvs1(t: Typ): Typ = t match {
      case TFn(t1, t2) => TFn(renametvs1(t1), renametvs1(t2))
      case TSt(ss, sv) => TSt(ss.map(renametvs1(_)), renametvs1(sv).asInstanceOf[TV])
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

  def main(args: Array[String]) = {

    val (t1, tve1) = newtv(tve0)
    val (t2, tve2) = newtv(tve1)

    testUnify(TSt(Nil, TV(0)), TSt(Nil, TV(0)))
    testUnify(TSt(TInt :: Nil, TV(0)), TSt(TInt :: Nil, TV(0)))
    testUnify(TSt(TInt :: Nil, TV(0)), TSt(TV(0) :: Nil, TV(1)))
    testUnify(TSt(TInt :: Nil, TV(0)), TSt(TInt :: Nil, TV(0)))
    testUnify(TSt(TV(0)::TV(1)::TV(0)::Nil,TV(2)), TSt(TInt::TV(0)::TV(1)::Nil, TV(2)))
    testUnify(TSt(TV(0)::Nil,TV(2)), TSt(TInt::TV(0)::TV(1)::Nil, TV(3)))

    // constantly('A 'b -> 'A ('C -> 'C 'b)
    val constantlyTyp =
      TFn(
        TSt(
          TV(0)::Nil, // 'b
          TV(1)), // 'A
        TSt(
          TFn(
            TSt(
              Nil, //
              TV(2)  // 'C
            ),
            TSt(
              TV(0)::Nil, // 'b
              TV(2)  // 'C
            )
          )::Nil, // ('C -> 'C 'b)
          TV(1)) // 'A
      )
    // swap ('A 'b 'c -> 'D 'f 'e)
    val swapTyp =
      TFn(
        TSt(
          TV(2)::TV(1)::Nil, // 'b 'a
          TV(0)  // 'C
        ),
        TSt(
          TV(1)::TV(2)::Nil, // 'a 'b
          TV(0)  // 'C
        ))

    testUnify(constantlyTyp.b, swapTyp.a)
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
    
    printType(
      Fix(
        L("self",
          L("counter",
            IFZ(V("counter"),
                I(0),
                A(V("self"), V("counter"))
              )))))*/
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


*/

}
