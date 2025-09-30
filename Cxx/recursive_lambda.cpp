int main()
{ 
  auto fib = [](this auto self, int n)
  {
    if (n < 2) return n;
    return self(n - 1) + self(n - 2);
  };

  return fib(20);
}
