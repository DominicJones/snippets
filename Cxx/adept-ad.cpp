// https://github.com/rjhogan/Adept
#include <adept_arrays.h>           

void hypot(aReal &a, aReal &b, aReal &r)
{
  using namespace adept;
  
  Stack stack;            // Object to store differential statements
  stack.new_recording();  // Clear any existing differential statements

  {
    aReal t0 = a * a;
    aReal t1 = b * b;
    aReal t2 = sqrt(t0 + t1);
    r = t2;
  }
  
  r.set_gradient(1.0);    // Seed the dependent variable, i.e. dr/dr = 1
  
  stack.reverse();        // Reverse-mode (adjoint) differentiation

  // dr/da = a.get_gradient()
  // dr/db = b.get_gradient()
}
