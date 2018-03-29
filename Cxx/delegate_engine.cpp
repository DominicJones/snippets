#include <iostream>


template<class T> struct Identity { using Type = T; };
template<class T> struct Identity<Identity<T> > { using Type = T; };


template<class T>
class VirtualEngine
{
public:
  virtual ~VirtualEngine() {}
  virtual T operator[](int it) const { return T(0); }
};


template<class T, class What, class Where>
class SpecificEngineProvider
{
public:
  virtual ~SpecificEngineProvider() {}

  virtual
  VirtualEngine<T> *
  makeSpecificEngine(Identity<What> what, Where const &where) const
  {
    return nullptr;
  }
};


class Cell;
class Face;
class FvRegion;


template<class T>
class Node
  : public SpecificEngineProvider<T, Cell, FvRegion>
  , public SpecificEngineProvider<T, Face, FvRegion>
{
public:
  virtual ~Node() {}
  using SpecificEngineProvider<T, Cell, FvRegion>::makeSpecificEngine;
  using SpecificEngineProvider<T, Face, FvRegion>::makeSpecificEngine;
};


template<class A1, class A2, class R>
class BinaryOpNode
  : public Node<R>
{
public:
  VirtualEngine<R> *
  makeSpecificEngine(Identity<Cell> what, FvRegion const &where) const override;

  VirtualEngine<R> *
  makeSpecificEngine(Identity<Face> what, FvRegion const &where) const override;

  Node<A1> * n1;
  Node<A2> * n2;
};


template<class A1, class A2, class R>
class BinaryOpEngine
  : public VirtualEngine<R>
{
public:
  BinaryOpEngine(VirtualEngine<A1> * e1, VirtualEngine<A2> * e2) {}

  virtual R operator[](int it) const { return (*e1)[it] * (*e2)[it]; }

  VirtualEngine<A1> * e1;
  VirtualEngine<A2> * e2;
};


template<class A1, class A2, class R>
VirtualEngine<R> *
BinaryOpNode<A1, A2, R>::
makeSpecificEngine(Identity<Cell> what, FvRegion const &where) const
{
  if (n1 && n2)
    return new BinaryOpEngine<A1, A2, R>(n1->makeSpecificEngine(what, where),
                                         n2->makeSpecificEngine(what, where));

  return nullptr;
}


template<class A1, class A2, class R>
VirtualEngine<R> *
BinaryOpNode<A1, A2, R>::
makeSpecificEngine(Identity<Face> what, FvRegion const &where) const
{
  if (n1 && n2)
    return new BinaryOpEngine<A1, A2, R>(n1->makeSpecificEngine(what, where),
                                         n2->makeSpecificEngine(what, where));

  return nullptr;
}


class FvRegion {};


int main()
{
  FvRegion region;
  Node<double> * node(new BinaryOpNode<double, double, double>());
  VirtualEngine<double> * engine(node->makeSpecificEngine(Identity<Cell>(), region));
}
