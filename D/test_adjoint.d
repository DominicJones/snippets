/*
! Demonstrate the testing of the adjoint of "eval"
!
! To generate and test the adjoint, the tangent of the primal is written and tested
! against a finite-difference derivative via invocations of primal,
! then once the tangent is tested the adjoint is implemented and tested against the tangent.
*/

import std.random, std.stdio, std.math, std.range, std.algorithm, std.numeric;


struct Term {
public:
  void eval(ref double[3] x,
            ref double y,
            ref double f,
            ref double[3] g) {
    double f2 = 0;
    for (auto i = 0; i != x.length; ++i) {
      const double u = x[i] * y;
      f2 += u^^2;
    }
    f = sqrt(f2);
    g[] = f / x[];
  }

  void eval_tng(ref double[3] x, ref double[3] x_drv,
                ref double y, ref double y_drv,
                ref double f, ref double f_drv,
                ref double[3] g, ref double[3] g_drv) {
    double f2_drv = 0;
    double f2 = 0;
    for (auto i = 0; i != x.length; ++i) {
      const double u_drv = x_drv[i] * y + x[i] * y_drv;
      const double u = x[i] * y;
      f2_drv += (2 * u) * u_drv;
      f2 += u^^2;
    }
    f_drv = f2_drv / (2 * sqrt(f2));
    f = sqrt(f2);
    g_drv[] = f_drv / x[] - (f / x[]^^2) * x_drv[];
    g[] = f / x[];
  }

  void eval_adj(ref double[3] x, ref double[3] x_drv,
                ref double y, ref double y_drv,
                ref double f, ref double f_drv,
                ref double[3] g, ref double[3] g_drv) {
    double f2_drv = 0;
    double f2 = 0;
    for (auto i = 0; i != x.length; ++i) {
      const double u = x[i] * y;
      f2 += u^^2;
    }
    f = sqrt(f2);

    f_drv += dotProduct(x[].map!(a => 1 / a), g_drv[]);
    x_drv[] -= (f / x[]^^2) * g_drv[];
    g[] = f / x[];

    f2_drv += f_drv / (2 * sqrt(f2));

    for (auto i = 0; i != x.length; ++i) {
      double u_drv = 0;
      const double u = x[i] * y;

      u_drv += (2 * u) * f2_drv;

      x_drv[i] += y * u_drv;
      y_drv += x[i] * u_drv;
    }

    f_drv = 0;
    g_drv = 0;
  }

  void eval_dif(ref double[3] x, ref double[3] x_drv, ref const double x_eps,
                ref double y, ref double y_drv, ref const double y_eps,
                ref double f, ref double f_drv,
                ref double[3] g, ref double[3] g_drv) {
    eval(x, y, f, g);

    double[3] x_dif = x[] + x_drv[] * x_eps;
    double y_dif = y + y_drv * y_eps;
    eval(x_dif, y_dif, f_drv, g_drv);

    f_drv = (f_drv - f) / x_eps;
    g_drv[] = (g_drv[] - g[]) / y_eps;
  }
}


void main() {
  const double eps = 1.0e-5, x_eps = eps, y_eps = eps;

  double[3] x, x_dif, x_tng, x_adj;
  double y, y_dif, y_tng, y_adj;

  double f, f_dif, f_tng, f_adj;
  double[3] g, g_dif, g_tng, g_adj;

  uniformDistribution(x.length, x);
  y = uniform(0.0, 1.0);

  {
    writeln("primal test:");

    Term().eval(x, y, f, g);
    writeln("  via primal  function ", f, g);

    x_tng = 0;
    y_tng = 0;
    Term().eval_tng(x, x_tng, y, y_tng, f, f_tng, g, g_tng);
    writeln("  via tangent function ", f, g);

    f_adj = 0;
    g_adj = 0;
    Term().eval_adj(x, x_adj, y, y_adj, f, f_adj, g, g_adj);
    writeln("  via adjoint function ", f, g);
  }

  {
    writeln("derivative test:");

    uniformDistribution(x_dif.length, x_dif);
    y_dif = uniform(0.0, 1.0);
    Term().eval_dif(x, x_dif, x_eps, y, y_dif, y_eps, f, f_dif, g, g_dif);
    writeln("  via primal  function ", f_dif, g_dif);

    x_tng = x_dif;
    y_tng = y_dif;
    Term().eval_tng(x, x_tng, y, y_tng, f, f_tng, g, g_tng);
    writeln("  via tangent function ", f_tng, g_tng);
  }

  {
    writeln("dot-product test:");

    uniformDistribution(x_tng.length, x_tng);
    y_tng = uniform(0.0, 1.0);
    Term().eval_tng(x, x_tng, y, y_tng, f, f_tng, g, g_tng);

    f_adj = uniform(0.0, 1.0);
    uniformDistribution(g_adj.length, g_adj);
    x_adj = 0;
    y_adj = 0;
    writeln("  outputs sum ", f_adj * f_tng + dotProduct(g_adj, g_tng));

    Term().eval_adj(x, x_adj, y, y_adj, f, f_adj, g, g_adj);
    writeln("  inputs  sum ", dotProduct(x_adj, x_tng) + y_adj * y_tng);
  }
}
