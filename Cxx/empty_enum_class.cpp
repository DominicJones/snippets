enum class UseSpecialFeature : bool {};

void eval(UseSpecialFeature const &usf) {}


int main()
{
  eval(UseSpecialFeature{true});
  // eval(UseSpecialFeature{-1}); // error
  // eval(true); // error
}
