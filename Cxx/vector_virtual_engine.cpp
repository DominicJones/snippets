#include <algorithm>
#include <array>
#include <cassert>
#include <chrono>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <limits>
#include <map>
#include <memory>
#include <numeric>
#include <set>
#include <tuple>
#include <type_traits>
#include <typeinfo>
#include <utility>
#include <vector>




template<class Variable_t>
class Node
{
public:
  class Engine {
  public:
    virtual Variable_t operator[](std::size_t const &it) const = 0;
  };

  class TangentEngine {
  public:
    virtual Variable_t operator[](std::size_t const &it) const = 0;
  };

  class AdjointEngine {
  public:
    virtual void operator()(std::size_t const &it, Variable_t const &result_drv) = 0;
  };

  virtual std::unique_ptr<Engine> makeEngine() const = 0;
  virtual std::unique_ptr<TangentEngine> makeTangentEngine() const = 0;
  virtual std::unique_ptr<AdjointEngine> makeAdjointEngine() = 0;

  virtual std::string disassemble() const = 0;
};




/// force value semantics
template<class Variable_t>
class ValueNode: public Node<Variable_t>
{
public:
  ValueNode() : ValueNode(Variable_t(0), Variable_t(0)) {}

  ValueNode(Variable_t const &&data) : ValueNode(std::move(data), Variable_t(0)) {}

  ValueNode(Variable_t const &&data,
            Variable_t const &&data_drv)
    : _data(data)
    , _data_drv(data_drv)
  {}

  class Engine: public Node<Variable_t>::Engine {
  public:
    Engine(Variable_t const &engine0): _engine0(engine0) {}
    Variable_t operator[](std::size_t const &it) const { return _engine0; }

  private:
    Variable_t const &_engine0;
  };

  class TangentEngine: public Node<Variable_t>::TangentEngine {
  public:
    TangentEngine(Variable_t const &engine0): _engine0(engine0) {}
    Variable_t operator[](std::size_t const &it) const { return _engine0; }

  private:
    Variable_t const &_engine0;
  };

  class AdjointEngine: public Node<Variable_t>::AdjointEngine {
  public:
    AdjointEngine(Variable_t &engine0): _engine0(engine0) {}
    void operator()(std::size_t const &it, Variable_t const &result_drv) { _engine0 += result_drv; }

  private:
    Variable_t &_engine0;
  };

  std::unique_ptr<typename Node<Variable_t>::Engine> makeEngine() const {
    return std::unique_ptr<typename Node<Variable_t>::Engine>(new Engine(this->_data));
  }

  std::unique_ptr<typename Node<Variable_t>::TangentEngine> makeTangentEngine() const {
    return std::unique_ptr<typename Node<Variable_t>::TangentEngine>(new TangentEngine(this->_data_drv));
  }

  std::unique_ptr<typename Node<Variable_t>::AdjointEngine> makeAdjointEngine() {
    return std::unique_ptr<typename Node<Variable_t>::AdjointEngine>(new AdjointEngine(this->_data_drv));
  }

  operator Variable_t() const { return _data_drv; }

  std::string disassemble() const {
    std::ostringstream stream;
    stream << std::addressof(this);
    return stream.str();
  }

private:
  Variable_t _data;
  Variable_t _data_drv;
};




/// force reference semantics
template<class Variable_t>
class FieldNode: public Node<Variable_t>
{
public:
  FieldNode(std::vector<Variable_t> const &&data,
            std::vector<Variable_t> &data_drv) = delete;

  FieldNode(std::vector<Variable_t> const &data,
            std::vector<Variable_t> &data_drv)
    : _data(data)
    , _data_drv(data_drv)
  {}

  class Engine: public Node<Variable_t>::Engine {
  public:
    Engine(std::vector<Variable_t> const &engine0): _engine0(engine0) {}
    Variable_t operator[](std::size_t const &it) const { return _engine0[it]; }

  private:
    std::vector<Variable_t> const &_engine0;
  };

  class TangentEngine: public Node<Variable_t>::TangentEngine {
  public:
    TangentEngine(std::vector<Variable_t> const &engine0): _engine0(engine0) {}
    Variable_t operator[](std::size_t const &it) const { return _engine0[it]; }

  private:
    std::vector<Variable_t> const &_engine0;
  };

  class AdjointEngine: public Node<Variable_t>::AdjointEngine {
  public:
    AdjointEngine(std::vector<Variable_t> &engine0): _engine0(engine0) {}
    void operator()(std::size_t const &it, Variable_t const &result_drv) { _engine0[it] += result_drv; }

  private:
    std::vector<Variable_t> &_engine0;
  };

  std::unique_ptr<typename Node<Variable_t>::Engine> makeEngine() const {
    return std::unique_ptr<typename Node<Variable_t>::Engine>(new Engine(this->_data));
  }

  std::unique_ptr<typename Node<Variable_t>::TangentEngine> makeTangentEngine() const {
    return std::unique_ptr<typename Node<Variable_t>::TangentEngine>(new TangentEngine(this->_data_drv));
  }

  std::unique_ptr<typename Node<Variable_t>::AdjointEngine> makeAdjointEngine() {
    return std::unique_ptr<typename Node<Variable_t>::AdjointEngine>(new AdjointEngine(this->_data_drv));
  }

  std::string disassemble() const {
    std::ostringstream stream;
    stream << std::addressof(this);
    return stream.str();
  }

private:
  std::vector<Variable_t> const &_data;
  std::vector<Variable_t> &_data_drv;
};




template<class Variable0_t, class Variable1_t, class Operator>
class BinaryNode: public Node<decltype(Operator::apply(Variable0_t(), Variable1_t()))>
{
public:
  using Result_t = decltype(Operator::apply(Variable0_t(), Variable1_t()));

  BinaryNode(Node<Variable0_t> &node0,
             Node<Variable1_t> &node1)
    : _node0(node0)
    , _node1(node1)
  {}

  class Engine: public Node<Result_t>::Engine {
  public:
    Engine(Node<Variable0_t> const &node0,
           Node<Variable1_t> const &node1)
      : _engine0(node0.makeEngine())
      , _engine1(node1.makeEngine())
    {}

    Result_t operator[](std::size_t const &it) const {
      return Operator::apply((*_engine0)[it], (*_engine1)[it]);
    }

  private:
    std::unique_ptr<typename Node<Variable0_t>::Engine> const _engine0;
    std::unique_ptr<typename Node<Variable1_t>::Engine> const _engine1;
  };

  class TangentEngine: public Node<Result_t>::TangentEngine {
  public:
    TangentEngine(Node<Variable0_t> const &node0,
                  Node<Variable1_t> const &node1)
      : _engine0(node0.makeEngine())
      , _engine0_drv(node0.makeTangentEngine())
      , _engine1(node1.makeEngine())
      , _engine1_drv(node1.makeTangentEngine())
    {}

    Result_t operator[](std::size_t const &it) const {
      if (Operator::isTrivial())
        return Operator::apply(Variable0_t(0), (*_engine0_drv)[it], Variable1_t(0), (*_engine1_drv)[it]);
      else
        return Operator::apply((*_engine0)[it], (*_engine0_drv)[it], (*_engine1)[it], (*_engine1_drv)[it]);
    }

  private:
    std::unique_ptr<typename Node<Variable0_t>::Engine> const _engine0;
    std::unique_ptr<typename Node<Variable0_t>::TangentEngine> const _engine0_drv;
    std::unique_ptr<typename Node<Variable1_t>::Engine> const _engine1;
    std::unique_ptr<typename Node<Variable1_t>::TangentEngine> const _engine1_drv;
  };

  class AdjointEngine: public Node<Result_t>::AdjointEngine {
  public:
    AdjointEngine(Node<Variable0_t> &node0,
                  Node<Variable1_t> &node1)
      : _engine0(node0.makeEngine())
      , _engine0_drv(node0.makeAdjointEngine())
      , _engine1(node1.makeEngine())
      , _engine1_drv(node1.makeAdjointEngine())
    {}

    void operator()(std::size_t const &it, Result_t const &result_drv) {
      Variable0_t a0_drv(0);
      Variable1_t a1_drv(0);

      if (Operator::isTrivial())
        Operator::apply(Variable0_t(0), a0_drv, Variable1_t(0), a1_drv, result_drv);
      else
        Operator::apply((*_engine0)[it], a0_drv, (*_engine1)[it], a1_drv, result_drv);

      (*_engine0_drv)(it, a0_drv);
      (*_engine1_drv)(it, a1_drv);
    }

  private:
    std::unique_ptr<typename Node<Variable0_t>::Engine> const _engine0;
    std::unique_ptr<typename Node<Variable0_t>::AdjointEngine> _engine0_drv;
    std::unique_ptr<typename Node<Variable1_t>::Engine> const _engine1;
    std::unique_ptr<typename Node<Variable1_t>::AdjointEngine> _engine1_drv;
  };


  std::unique_ptr<typename Node<Result_t>::Engine> makeEngine() const {
    return std::unique_ptr<typename Node<Result_t>::Engine>(new Engine(this->_node0, this->_node1));
  }

  std::unique_ptr<typename Node<Result_t>::TangentEngine> makeTangentEngine() const {
    return std::unique_ptr<typename Node<Result_t>::TangentEngine>(new TangentEngine(this->_node0, this->_node1));
  }

  std::unique_ptr<typename Node<Result_t>::AdjointEngine> makeAdjointEngine() {
    return std::unique_ptr<typename Node<Result_t>::AdjointEngine>(new AdjointEngine(this->_node0, this->_node1));
  }

  std::string disassemble() const {
    return "(" + Operator::name() + " " + _node0.disassemble() + " " + _node1.disassemble() + ")";
  }

private:
  Node<Variable0_t> &_node0;
  Node<Variable1_t> &_node1;
};




class Plus
{
public:
  static std::string name() { return std::string("+"); }
  static constexpr bool isTrivial() { return true; }

  template<class A0, class A1>
  static auto apply(A0 const &a0, A1 const &a1)
    -> decltype(a0 + a1) { return a0 + a1; }

  template<class A0, class A1>
  static auto apply(A0 const &a0, A0 const &a0_drv, A1 const &a1, A1 const &a1_drv)
    -> decltype(a0 + a1) { return a1_drv + a0_drv; }

  template<class A0, class A1>
  static auto apply(A0 const &a0, A0 &a0_drv, A1 const &a1, A1 &a1_drv, decltype(a0 + a1) const &result_drv)
    -> void { a1_drv += result_drv; a0_drv += result_drv; }
};


class Multiplies
{
public:
  static std::string name() { return std::string("*"); }
  static constexpr bool isTrivial() { return false; }

  template<class A0, class A1>
  static auto apply(A0 const &a0, A1 const &a1)
    -> decltype(a0 * a1) { return a0 * a1; }

  template<class A0, class A1>
  static auto apply(A0 const &a0, A0 const &a0_drv, A1 const &a1, A1 const &a1_drv)
    -> decltype(a0 * a1) { return a0 * a1_drv + a0_drv * a1; }

  template<class A0, class A1>
  static auto apply(A0 const &a0, A0 &a0_drv, A1 const &a1, A1 &a1_drv, decltype(a0 * a1) const &result_drv)
    -> void { a1_drv += a0 * result_drv; a0_drv += result_drv * a1; }
};




int main(int argc, char** argv)
{
  using Variable0_t = float;
  using Variable1_t = double;

  std::size_t const size(1);
  std::vector<Variable0_t> data0(size, 3);
  std::vector<Variable0_t> data0_drv(size, 0);
  std::vector<Variable1_t> data1(size, 4);
  std::vector<Variable1_t> data1_drv(size, 0);
  Variable0_t data2(5);

  FieldNode<Variable0_t> node0(data0, data0_drv);
  FieldNode<Variable1_t> node1(data1, data1_drv);
  ValueNode<Variable0_t> node2;
  BinaryNode<Variable0_t, Variable1_t, Multiplies> node3(node0, node1);
  BinaryNode<Variable0_t, Variable1_t, Plus> node4(node2, node3);
  BinaryNode<Variable1_t, Variable1_t, Multiplies> node5(node3, node4);


  auto &root = node5;

  {
    std::cout << root.disassemble() << std::endl;
  }

  // primal
  {
    node2 = ValueNode<Variable0_t>(std::move(data2));
    std::vector<Variable1_t> result(size, 0);
    auto engine(root.makeEngine());
    for (std::size_t it = 0; it != size; ++it)
      result[it] = (*engine)[it];
    std::cout << "r = " << result[0] << std::endl;
  }

  // tangent
  {
    node2 = ValueNode<Variable0_t>(std::move(data2));
    std::vector<Variable1_t> result(size, 0);
    for (auto &e: data0_drv) e = 1;
    auto engine(root.makeTangentEngine());
    for (std::size_t it = 0; it != size; ++it)
      result[it] = (*engine)[it];
    std::cout << "dr/da = " << result[0] << std::endl;
    for (auto &e: data0_drv) e = 0;
  }
  {
    node2 = ValueNode<Variable0_t>(std::move(data2));
    std::vector<Variable1_t> result(size, 0);
    for (auto &e: data1_drv) e = 1;
    auto engine(root.makeTangentEngine());
    for (std::size_t it = 0; it != size; ++it)
      result[it] = (*engine)[it];
    std::cout << "dr/db = " << result[0] << std::endl;
    for (auto &e: data1_drv) e = 0;
  }
  {
    node2 = ValueNode<Variable0_t>(std::move(data2), Variable0_t(1));
    std::vector<Variable1_t> result(size, 0);
    auto engine(root.makeTangentEngine());
    for (std::size_t it = 0; it != size; ++it)
      result[it] = (*engine)[it];
    std::cout << "dr/dc = " << result[0] << std::endl;
  }

  // adjoint
  {
    node2 = ValueNode<Variable0_t>(std::move(data2));
    std::vector<Variable1_t> result(size, 1);
    auto engine(root.makeAdjointEngine());
    for (std::size_t it = 0; it != size; ++it)
      (*engine)(it, result[it]);
    std::cout << "dr/da^t = " << data0_drv[0] << std::endl;
    std::cout << "dr/db^t = " << data1_drv[0] << std::endl;
    std::cout << "dr/dc^t = " << Variable0_t(node2) << std::endl;
  }
}
