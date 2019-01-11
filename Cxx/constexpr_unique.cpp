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


template<typename T, int N>
struct Unique
{
  T value;
  static constexpr int tag = N;
};

template<typename T, int N = next()> Unique(T) -> Unique<T, N>;


int main()
{
  Unique v0{0}; // tag = 1
  Unique v1{1}; // tag = 2
}
