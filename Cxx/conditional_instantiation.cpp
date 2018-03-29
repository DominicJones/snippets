/*
 * always explicitly instantiate Evaluate<dreal>
 * if real is not the same as dreal then explicitly instantiate Evaluate<real>
 */

#include <type_traits>


using dreal = double;

#ifdef MixedPrecision
using real = float;
#else
using real = dreal;
#endif


template<typename T> class Evaluate
{
  // implementation
};


template<> class Evaluate<std::nullptr_t> {};

using Evaluate_real = std::conditional<!std::is_same<real, dreal>::value, Evaluate<real>, Evaluate<std::nullptr_t> >::type;
using Evaluate_dreal = Evaluate<dreal>;

template<typename T> using alias = T;
template class alias<Evaluate_real>::Evaluate;
template class alias<Evaluate_dreal>::Evaluate;


int main(){}
