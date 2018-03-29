// from:
// https://stackoverflow.com/questions/17424477/implementation-c14-make-integer-sequence/17426611#17426611

// using aliases for cleaner syntax
template<class T> using Invoke = typename T::type;

template<unsigned...> struct seq{ using type = seq; };

template<class S1, class S2> struct concat;

template<unsigned... I1, unsigned... I2>
struct concat<seq<I1...>, seq<I2...>>
  : seq<I1..., (sizeof...(I1)+I2)...>{};

template<class S1, class S2>
using Concat = Invoke<concat<S1, S2>>;

template<unsigned N> struct gen_seq;
template<unsigned N> using GenSeq = Invoke<gen_seq<N>>;

template<unsigned N>
struct gen_seq : Concat<GenSeq<N/2>, GenSeq<N - N/2>>{};

template<> struct gen_seq<0> : seq<>{};
template<> struct gen_seq<1> : seq<0>{};
