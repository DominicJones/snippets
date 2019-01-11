template<int N>
struct counter
{
  friend constexpr int counter_value(counter<N>);
};

template<int N>
struct writer
{
  friend constexpr int counter_value(counter<N>) { return N; }
  static constexpr int value = N;
};

template<int N, int = counter_value(counter<N>{})>
int constexpr reader(int, counter<N>)
{
  return N;
}

template<int N>
int constexpr reader(float, counter<N>, int R = reader(0, counter<N-1>{}))
{
  return R;
}

int constexpr reader(float, counter<0>) { return 0; }

int constexpr max_next_value() { return 32; }

template<int N = 1>
int constexpr next(int R = writer<reader(0, counter<max_next_value()>{}) + N>::value)
{
  return R;
}


int main()
{
  static_assert(next() == 1);
  static_assert(next() == 2);
  static_assert(next() == 3);
}
