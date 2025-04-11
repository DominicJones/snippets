void eval(bool option, int &value)
{
  auto eval_impl = [&]<auto option>()
  {
     value = 10*(option + 1);
  };

  if (option)
    eval_impl.template operator()<true>();
  else
    eval_impl.template operator()<false>();
}

int main()
{
  bool option = true;
  int value = 0;
  eval(option, value);
  return value;
}
