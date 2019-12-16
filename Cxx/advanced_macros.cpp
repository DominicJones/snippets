// based on: http://jhnet.co.uk/articles/cpp_magic

// CPP_MACROS

// Basic utilities

#define CPP_EMPTY()
#define CPP_DEFER1(m) m CPP_EMPTY()
#define CPP_DEFER2(m) m CPP_EMPTY CPP_EMPTY()()

#define CPP_CAT(a,b) a ## b
#define CPP_FIRST(a, ...) a
#define CPP_SECOND(a, b, ...) b

#define CPP_EVAL(...) CPP_EVAL8(__VA_ARGS__)
#define CPP_EVAL8(...) CPP_EVAL4(CPP_EVAL4(__VA_ARGS__))
#define CPP_EVAL4(...) CPP_EVAL2(CPP_EVAL2(__VA_ARGS__))
#define CPP_EVAL2(...) CPP_EVAL1(CPP_EVAL1(__VA_ARGS__))
#define CPP_EVAL1(...) __VA_ARGS__

// Advanced utilities

#define CPP_IS_PROBE(...) CPP_SECOND(__VA_ARGS__, 0)
#define CPP_PROBE() ~, 1

#define CPP_NOT(x) CPP_IS_PROBE(CPP_CAT(_CPP_NOT_, x))
#define _CPP_NOT_0 CPP_PROBE()

#define CPP_BOOL(x) CPP_NOT(CPP_NOT(x))

#define CPP_HAS_ARGS(...) CPP_BOOL(CPP_FIRST(_CPP_END_OF_ARGUMENTS_ __VA_ARGS__)())
#define _CPP_END_OF_ARGUMENTS_() 0

// Conditional expansion

#define CPP_IF_ELSE(condition) _CPP_IF_ELSE(CPP_BOOL(condition))
#define _CPP_IF_ELSE(condition) CPP_CAT(_CPP_IF_, condition)
#define _CPP_IF_1(...) __VA_ARGS__ _CPP_IF_1_ELSE
#define _CPP_IF_0(...)             _CPP_IF_0_ELSE
#define _CPP_IF_1_ELSE(...)
#define _CPP_IF_0_ELSE(...) __VA_ARGS__

// Iteration expansion

#define CPP_MAP(m, first, ...)           \
  m(first)                           \
  CPP_IF_ELSE(CPP_HAS_ARGS(__VA_ARGS__))(    \
    CPP_DEFER2(_CPP_MAP)()(m, __VA_ARGS__)   \
  )()
#define _CPP_MAP() CPP_MAP



#define GenerateWrapper(cond)                   \
  template<typename T>                          \
  struct Wrapper                                \
  {                                             \
    Wrapper(T const &v) : value(v) {}           \
    CPP_IF_ELSE(cond)(T const &)(T const) value;    \
  };

GenerateWrapper(CPP_BOOL(1))

#define GenerateTags(x) struct CPP_CAT(x, _tag){};
CPP_EVAL(CPP_MAP(GenerateTags, Arg0, Arg1))

int main()
{
  double d = 1;
  Wrapper<double>{d};
  Arg0_tag tag;
}


// CPP_MACROS

#undef CPP_EMPTY
#undef CPP_DEFER1
#undef CPP_DEFER2
#undef CPP_CAT
#undef CPP_FIRST
#undef CPP_SECOND
#undef CPP_EVAL
#undef CPP_EVAL8
#undef CPP_EVAL4
#undef CPP_EVAL2
#undef CPP_EVAL1
#undef CPP_IS_PROBE
#undef CPP_PROBE
#undef CPP_NOT
#undef _CPP_NOT_0
#undef CPP_BOOL
#undef CPP_HAS_ARGS
#undef _CPP_END_OF_ARGUMENTS_
#undef CPP_IF_ELSE
#undef _CPP_IF_ELSE
#undef _CPP_IF_1
#undef _CPP_IF_0
#undef _CPP_IF_1_ELSE
#undef _CPP_IF_0_ELSE
#undef CPP_MAP
#undef _CPP_MAP
