// CPP_MACROS

// Basic utilities

#define EMPTY()
#define DEFER1(m) m EMPTY()
#define DEFER2(m) m EMPTY EMPTY()()

#define CAT(a,b) a ## b
#define FIRST(a, ...) a
#define SECOND(a, b, ...) b

#define EVAL(...) EVAL8(__VA_ARGS__)
#define EVAL8(...) EVAL4(EVAL4(__VA_ARGS__))
#define EVAL4(...) EVAL2(EVAL2(__VA_ARGS__))
#define EVAL2(...) EVAL1(EVAL1(__VA_ARGS__))
#define EVAL1(...) __VA_ARGS__

// Advanced utilities

#define IS_PROBE(...) SECOND(__VA_ARGS__, 0)
#define PROBE() ~, 1

#define NOT(x) IS_PROBE(CAT(_NOT_, x))
#define _NOT_0 PROBE()

#define BOOL(x) NOT(NOT(x))

#define HAS_ARGS(...) BOOL(FIRST(_END_OF_ARGUMENTS_ __VA_ARGS__)())
#define _END_OF_ARGUMENTS_() 0

// Conditional expansion

#define IF_ELSE(condition) _IF_ELSE(BOOL(condition))
#define _IF_ELSE(condition) CAT(_IF_, condition)
#define _IF_1(...) __VA_ARGS__ _IF_1_ELSE
#define _IF_0(...)             _IF_0_ELSE
#define _IF_1_ELSE(...)
#define _IF_0_ELSE(...) __VA_ARGS__

// Iteration expansion

#define MAP(m, first, ...)           \
  m(first)                           \
  IF_ELSE(HAS_ARGS(__VA_ARGS__))(    \
    DEFER2(_MAP)()(m, __VA_ARGS__)   \
  )()
#define _MAP() MAP



#define GenerateWrapper(cond)                   \
  template<typename T>                          \
  struct Wrapper                                \
  {                                             \
    Wrapper(T const &v) : value(v) {}           \
    IF_ELSE(cond)(T const &)(T const) value;    \
  };

GenerateWrapper(BOOL(1))

#define GenerateTags(x) struct CAT(x, _tag){};
EVAL(MAP(GenerateTags, Arg0, Arg1))

int main()
{
  double d = 1;
  Wrapper<double>{d};
  Arg0_tag tag;
}


// CPP_MACROS

#undef EMPTY
#undef DEFER1
#undef DEFER2
#undef CAT
#undef FIRST
#undef SECOND
#undef EVAL
#undef EVAL8
#undef EVAL4
#undef EVAL2
#undef EVAL1
#undef IS_PROBE
#undef PROBE
#undef NOT
#undef _NOT_0
#undef BOOL
#undef HAS_ARGS
#undef _END_OF_ARGUMENTS_
#undef IF_ELSE
#undef _IF_ELSE
#undef _IF_1
#undef _IF_0
#undef _IF_1_ELSE
#undef _IF_0_ELSE
#undef MAP
#undef _MAP
