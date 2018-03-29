/*===========================================================================*\
 *                                                                           *
  *      Euler-angle representation and optimization of 3D cross-fields       *
   *    Copyright (C) 2014 by Mesh Generation Group, RWTH Aachen University    *
    *                       www.meshing.rwth-aachen.de                          *
     *             author: David Bommes (bommes@cs.rwth-aachen.de)               *
      *                                                                           *
       *       code based on “Boundary aligned smooth 3D cross-frame field” by     *
        *                    J. Huang, Y. Tong, H. Wei and H. Bao                   *
         *            and optimized for performance with Maplesoft Maple 18          *
          *                                                                           *
          \*===========================================================================*/

/*
written by:
 -- David Bommes (bommes@cs.rwth-aachen.de)
 -- Klaus Leppkes (klaus.leppkes@rwth-aachen.de)
 -- Johannes Lotz (johannes.lotz@rwth-aachen.de)
 -- Jacques du Toit (jacques@nag.co.uk)
This code can be used for benchmarking purposes for computing
adjoints. Whenever using or publishing this code, please contact the
authors first.
*/

#include "f_harmonic.hpp"

template<class T>
void f_harmonic(const T *x, T& _f) {
  auto &_alpha = x[0];
  auto &_beta  = x[1];
  auto &_gamma = x[2];
  auto &_beta2 = x[3];
  auto &_beta3 = x[4];
 
  const auto& t1 = cos(_beta);
  const auto& t2 = cos(_alpha);
  const auto& t3 = sin(_beta2);
  const auto& t4 = t2 - t3;
  const auto& t5 = t2 + t3;
  const auto& t6 = t1*t1;
  const auto& t7 = t6 + 0.1e1*_beta3;
  const auto& t8 = cos(_gamma);
  const auto& t9 = sin(_gamma);
  const auto& t10 = t2*t2;
  const auto& t11 = 0.2e1 * t2;
  const auto& t12 = t10 + (-t11 - t3) * t3;
  const auto& t11_2 = t10 + (t11 - t3) * t3;
  const auto& t13 = 0.1e1 + (0.6e1 + t6) * t6;
  const auto& t14 = t9*t9;
  const auto& t15 = t8*t8;
  const auto& t16 = t15*t15 + t14*t14;
  const auto& t17 = t15 - t14;
  const auto& t18 = sqrt(0.5e1);
  const auto& t19 = sin(_beta);
  const auto& t20 = sqrt(0.3e1);
  const auto& t21 = sqrt(0.7e1);
  const auto& t22 = sqrt(0.14e2);
  const auto& t23 = sqrt(0.30e2);
  const auto& t24 = t8 - t9;
  const auto& t25 = t8 + t9;
  const auto& t26 = t19*t19;
  const auto& t27 = t26*t26;
  const auto& t28 = sqrt(0.2e1);
  const auto& t29 = sqrt(0.6e1);
  const auto& t30 = sqrt(0.42e2);
  const auto& t31 = 0.2e1 * t23;
  const auto& t32 = t18 * (0.4e1 * t20 + t28 * t29) * t30 + t31 * t20 * t21;
  const auto& t33 = t15 - t14 / 0.3e1;
  const auto& t34 = t15 - 0.3e1 * t14;
  const auto& t35 = t6 + 0.3e1;
  const auto& t36 = t12 * t11_2;
  const auto& t37 = t29 * t20;
  const auto& t38 = t28 + t37;
  const auto& t39 = -0.3e1 / 0.16e2;
  const auto& t40 = t15 + t14;
  const auto& t31_2 = t20 * (t6 * t18 * t29 + t31 * (t6 - t26 * t40 / 0.2e1)) * t28 + (0.12e2 * t18 + t29 * t23) * t6;
  const auto& t41 = t40 * t6;
  const auto& t42 = t41 - t15 - t14 - 0.2e1 * t26;
  const auto& t43 = sqrt(0.10e2);
  const auto& t44 = sqrt(0.15e2);
  const auto& t45 = t20 * t28;
  const auto& t46 = t45 * t42;
  const auto& t41_2 = t46 * t43 + (t41 - t15 - t14 - 0.8e1 * t26) * t44;
  const auto& t47 = t1 - 0.1e1;
  const auto& t48 = t1 + 0.1e1;
  const auto& t30_2 = _alpha*t41_2 * sqrt(0.105e3) - 0.30e2 * t30 * t26 * t29 + 0.15e2 * t40 * t47 * t48 * t21;
  const auto& t49 = _beta*t3*t3;
  const auto& t50 = t10*t10 + (-0.6e1 * t10 + t49) * t49*_beta2;
  const auto& t51 = 0.4e1 * t2 * t3;
  const auto& t37_2 = (t45 * (t6 - 0.3e1 / 0.8e1 * t26 * t40) + t29 * t6 / 0.2e1) * t44 + (-t37 * t26 * t40 / 0.2e1 + 0.3e1 / 0.2e1 * t28 * (t6 - 0.5e1 / 0.4e1 * t26 * t40)) * t18;
  const auto& t44_2 = sqrt(0.70e2);
  const auto& t52 = sqrt(0.21e2);
  const auto& t10_2 = t51 * (t10 - t49)*_beta3;
  const auto& t49_2 = t50 * t1;
  const auto& t53 = t18 / 0.1680e4*_beta2;
  const auto& t28_2 = t53 * ((0.6e1 * t28 * t44_2 * t43 + 0.15e2 * t52 * t29) * t48 * t47 * t40 + t46 * t44_2 * t23 + t41_2 * sqrt(0.210e3));
  const auto& t44_3 = t1 * t21 / 0.6e1*_beta*_gamma;
  const auto& t46_2 = -t26 * t40 / 0.4e1;
  const auto& t53_2 = t26 * t25 * t24 * t31_2 * t21 / 0.24e2 - t53 * t30_2 * (t36 * t7 * t25 * t24 - 0.16e2 * t2 * t8 * t3 * t9 * t4 * t5 * t1);
  const auto& t54 = t19 * (t1 * t32 * t8 * t26 * t34 * t21 / 0.96e2 - t38 * ((-0.36e2 * t6 - 0.12e2) * t33 * t9 * t5 * t4 * t3 * t2 + t36 * t8 * t34 * t1 * t35) * t18 / 0.16e2);
  const auto& t55 = 0.2e1 * t8;
  const auto& t55_2 = (-0.384e3 * t8 * t9 * t17 * t7 * t5 * t4 * t3 * t2 * t1 + t36 * t13 * (0.6e1 * t16 - 0.36e2 * t14 * t15)) * t18 / 0.48e2 + t27 * t20 * t21 * t22 * t23 * (t15 + (-t55 - t9) * t9) * (t15 + (t55 - t9) * t9) / 0.48e2;
  
  const auto& _f_0 = ((0.24e2 * t16 - 0.144e3 * t14 * t15) * t7 * t5 * t4 * t3 * t2 * t1 + 0.6e1 * t9 * t12 * t11_2 * t13 * t8 * t17) * t18 / 0.12e2 + t27 * t8 * t9 * t20 * t21 * t22 * t23 * t24 * t25 / 0.12e2;
  const auto& _f_1 = t19 * (t9 * t1 * t26 * t32 * t33 * t21 / 0.32e2 + t39 * ((0.4e1 * t6 + 0.4e1 / 0.3e1) * t34 * t8 * t5 * t4 * t3 * t2 + t36 * t9 * t33 * t1 * t35) * t38 * t18);
  const auto& _f_2 = t21 * t9 * t31_2 * t26 * t8 / 0.12e2 - t30_2 * (t7 * t50 * t9 * t8 + t51 * t24 * t25 * t4 * t5 * t1) * t18 / 0.840e3;
  const auto& _f_3 = t19 * t44_3 * t9 * t37_2 + t28_2 * (t49_2 * t9 + t10_2 * t8);
  const auto& _f_4 = -t36 * (-t40 * t47 * t41_2 * t48 * t52 / 0.2e1 + t26 * (t23 * t42 * t20 + 0.6e1 * t43 * t40 * t47 * t48) * t22) * t18 / 0.420e3 + t21 * (t46_2 * t45 * t29 * (t6 + t46_2) + (t6 - 0.3e1 / 0.2e1 * t26 * t40) * t6);
  const auto& _f_5 = t19 * (t44_3 * t37_2 * t8 + t28_2 * (t49_2 * t8 - t10_2 * t9));
  const auto& _f_6 = t53_2;
  const auto& _f_7 = t54*1;
  const auto& _f_8 = t55_2;
  
  _f = _f_0 + _f_1 + _f_2 + _f_3 + _f_4 + _f_5 + _f_6 + _f_7 + _f_8;
}

template void f_harmonic<double>(const double *x, double& _f);
