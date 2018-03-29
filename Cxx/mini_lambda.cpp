////////////////////////////////////////////////////////////////////////////////
// Mini-lambda library with Boost.Proto as described in
// http://cpp-next.com/archive/2010/11/expressive-c-function-composition-2
// 
//  Copyright 2010 Eric Niebler. Distributed under the Boost
//  Software License, Version 1.0. (See accompanying file
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
////////////////////////////////////////////////////////////////////////////////

#include <boost/proto/proto.hpp>
#include <boost/tuple/tuple.hpp>
namespace mpl = boost::mpl;
namespace proto = boost::proto;

// A TR1-style function object that accepts an argN_tag
// instance and a tuple and returns the proper tuple element.
struct tuple_get : proto::callable
{
    template<typename Sig> struct result;

    template<typename This, typename ArgNTag, typename Tuple>
    struct result<This(ArgNTag const &, Tuple const &)>
    {
        // ArgNTag is an instance of argN_tag, which has a nested index_type typedef
        typedef typename ArgNTag::index_type N;
        // boost::tuples::element is a trait that returns the type of the Nth element
        typedef typename boost::tuples::element<N::value, Tuple>::type type;
    };

    template<typename ArgNTag, typename Tuple>
    typename result<tuple_get(ArgNTag const &, Tuple const &)>::type
    operator()(ArgNTag const &, Tuple const & tup) const
    {
        typedef typename ArgNTag::index_type N;
        // boost::get is a function that returns the Nth element of a tuple.
        return boost::get<N::value>(tup);
    }
};

template<typename Index>
struct argN_tag
{
    typedef Index index_type;
};

template<typename Index>
struct argN_term
  : proto::terminal< argN_tag<Index> >
{};

// A Proto algorithm for evaluating lambda expressions
struct Lambda
  : proto::or_<
        // When evaluating a placeholder terminal, return the state.
        proto::when<
            argN_term< proto::_ >
          , tuple_get(proto::_value(proto::_), proto::_state)
        >
        // Otherwise, do the "default" thing.
      , proto::otherwise< proto::_default< Lambda >(proto::_, proto::_state) >
    >
{};

// Forward-declare our custom expression wrapper
template<typename ProtoExpr> struct lambda_expr;

// Define a lambda domain and its generator, which
// simply wraps all new expressions our custom wrapper
struct lambda_domain
  : proto::domain< proto::pod_generator<lambda_expr> >
{};

// A lambda is an expression with an operator() that
// evaluates the lambda.
template<typename ProtoExpr>
struct lambda_expr
{
    BOOST_PROTO_BASIC_EXTENDS(ProtoExpr, lambda_expr, lambda_domain)
    BOOST_PROTO_EXTENDS_ASSIGN()
    BOOST_PROTO_EXTENDS_SUBSCRIPT()

    // So that boost::result_of can be used to calculate
    // the return type of this lambda expression.
    template<typename Sig> struct result;

    template<typename This, typename A1>
    struct result<This(A1)>
      : boost::result_of<Lambda(This const &, boost::tuple<A1> const &)>
    {};

    template<typename This, typename A1, typename A2>
    struct result<This(A1, A2)>
      : boost::result_of<Lambda(This const &, boost::tuple<A1, A2> const &)>
    {};

    template<typename This, typename A1, typename A2, typename A3>
    struct result<This(A1, A2, A3)>
      : boost::result_of<Lambda(This const &, boost::tuple<A1, A2, A3> const &)>
    {};

    // Evaluate the lambda expressions
    template<typename A1>
    typename result<lambda_expr(A1 const &)>::type
    operator()(A1 const & a1) const
    {
        boost::tuple<A1 const &> const state(a1);
        return Lambda()(*this, state);
    }

    template<typename A1, typename A2>
    typename result<lambda_expr(A1 const &, A2 const &)>::type
    operator()(A1 const & a1, A2 const & a2) const
    {
        boost::tuple<A1 const &, A2 const &> const state(a1, a2);
        return Lambda()(*this, state);
    }

    template<typename A1, typename A2, typename A3>
    typename result<lambda_expr(A1 const &, A2 const &, A3 const &)>::type
    operator()(A1 const & a1, A2 const & a2, A2 const & a3) const
    {
        boost::tuple<A1 const &, A2 const &, A3 const &> const state(a1, a2, a3);
        return Lambda()(*this, state);
    }
};

typedef mpl::int_<0> nil_t;
typedef mpl::int_<1> one_t;
typedef mpl::int_<2> two_t;

lambda_expr< argN_term< nil_t >::type > const arg1 = {};
lambda_expr< argN_term< one_t >::type > const arg2 = {};
lambda_expr< argN_term< two_t >::type > const arg3 = {};

#include <algorithm>

int main()
{
    int data[10] = {0,1};

    // generate the Fibonacci sequence
    std::transform(data, data+8, data+1, data+2, arg1 + arg2);

    // write each element to std::cout
    std::for_each(data, data+10, std::cout << arg1 << ' ');
}
