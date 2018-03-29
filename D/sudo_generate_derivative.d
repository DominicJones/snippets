// This is "sudo" D code to convey the intention.
// The task is to make it really work.


string eval_inner = "
mixin template eval_inner_impl()
{
  immutable auto a = alpha[e];
  immutable auto b = a.mag();
  immutable auto c = a / b;
  omega[e] = c;
}";


void eval(Vec_t, Mode_t)(Vec_t inout alpha, Vec_t inout omega)
{
  for (auto e = 0; e != alpha.size; ++e)
  {
    mixin generate!Mode_t(eval_inner);
  }
}


unittest
{
  Vector alpha = [1,2,3,4];
  Vector alpha_drv(4) = 0;
  VectorPair alphaPair(alpha, alpha_drv);

  Vector omega(4) = 0;
  Vector alpha_drv(4) = 0;
  VectorPair omegaPair(omega, omega_drv);

  eval!(Vector, Primal)(alpha, omega);
  eval!(VectorPair, Tangent)(alphaPair, omegaPair);
  eval!(VectorPair, Adjoint)(alphaPair, omegaPair);
}
