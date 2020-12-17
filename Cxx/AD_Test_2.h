// -*- C++ -*-
#pragma once

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

/*
minor modifications by:
 -- Dominic Jones (dominic.jones@cd-adapco.com)
*/

struct AD_Test_2
{
  static inline void
  evaluate(double const &_alpha,
           double const &_beta,
           double const &_gamma,
           double const &_beta2,
           double const &_beta3,
           double &_f) noexcept
  {
    auto const t1((cos(_beta)));
    auto const t2((cos(_alpha)));
    auto const t3((sin(_beta2)));
    auto const t4((t2 - t3));
    auto const t5((t2 + t3));
    auto const t6((t1*t1));
    auto const t7((t6 + double(0.1e1)*_beta3));
    auto const t8((cos(_gamma)));
    auto const t9((sin(_gamma)));
    auto const t10((t2*t2));
    auto const t11((double(0.2e1) * t2));
    auto const t12((t10 + (-t11 - t3) * t3));
    auto const t11_2((t10 + (t11 - t3) * t3));
    auto const t13((double(0.1e1) + (double(0.6e1) + t6) * t6));
    auto const t14((t9*t9));
    auto const t15((t8*t8));
    auto const t16((t15*t15 + t14*t14));
    auto const t17((t15 - t14));
    auto const t18 = sqrt(double(0.5e1));
    auto const t19((sin(_beta)));
    auto const t20 = sqrt(double(0.3e1));
    auto const t21 = sqrt(double(0.7e1));
    auto const t22 = sqrt(double(0.14e2));
    auto const t23 = sqrt(double(0.30e2));
    auto const t24((t8 - t9));
    auto const t25((t8 + t9));
    auto const t26((t19*t19));
    auto const t27((t26*t26));
    auto const t28 = sqrt(double(0.2e1));
    auto const t29 = sqrt(double(0.6e1));
    auto const t30 = sqrt(double(0.42e2));
    auto const t31 = double(0.2e1) * t23;
    auto const t32 = t18 * (double(0.4e1) * t20 + t28 * t29) * t30 + t31 * t20 * t21;
    auto const t33((t15 - t14 / double(0.3e1)));
    auto const t34((t15 - double(0.3e1) * t14));
    auto const t35((t6 + double(0.3e1)));
    auto const t36((t12 * t11_2));
    auto const t37 = t29 * t20;
    auto const t38 = t28 + t37;
    auto const t39 = -double(0.3e1) / double(0.16e2);
    auto const t40((t15 + t14));
    auto const t31_2((t20 * (t6 * t18 * t29 + t31 * (t6 - t26 * t40 / double(0.2e1))) * t28 + (double(0.12e2) * t18 + t29 * t23) * t6));
    auto const t41((t40 * t6));
    auto const t42((t41 - t15 - t14 - double(0.2e1) * t26));
    auto const t43 = sqrt(double(0.10e2));
    auto const t44 = sqrt(double(0.15e2));
    auto const t45 = t20 * t28;
    auto const t46((t45 * t42));
    auto const t41_2((t46 * t43 + (t41 - t15 - t14 - double(0.8e1) * t26) * t44));
    auto const t47((t1 - double(0.1e1)));
    auto const t48((t1 + double(0.1e1)));
    auto const t30_2((_alpha*t41_2 * sqrt(double(0.105e3)) - double(0.30e2) * t30 * t26 * t29 + double(0.15e2) * t40 * t47 * t48 * t21));
    auto const t49((_beta*t3*t3));
    auto const t50((t10*t10 + (-double(0.6e1) * t10 + t49) * t49*_beta2));
    auto const t51((double(0.4e1) * t2 * t3));
    auto const t37_2(((t45 * (t6 - double(0.3e1) / double(0.8e1) * t26 * t40) + t29 * t6 / double(0.2e1)) * t44 + (-t37 * t26 * t40 / double(0.2e1) + double(0.3e1) / double(0.2e1) * t28 * (t6 - double(0.5e1) / double(0.4e1) * t26 * t40)) * t18));
    auto const t44_2 = sqrt(double(0.70e2));
    auto const t52 = sqrt(double(0.21e2));
    auto const t10_2((t51 * (t10 - t49)*_beta3));
    auto const t49_2((t50 * t1));
    auto const t53((t18 / double(0.1680e4)*_beta2));
    auto const t28_2((t53 * ((double(0.6e1) * t28 * t44_2 * t43 + double(0.15e2) * t52 * t29) * t48 * t47 * t40 + t46 * t44_2 * t23 + t41_2 * sqrt(double(0.210e3)))));
    auto const t44_3((t1 * t21 / double(0.6e1)*_beta*_gamma));
    auto const t46_2((-t26 * t40 / double(0.4e1)));
    auto const t53_2((t26 * t25 * t24 * t31_2 * t21 / double(0.24e2) - t53 * t30_2 * (t36 * t7 * t25 * t24 - double(0.16e2) * t2 * t8 * t3 * t9 * t4 * t5 * t1)));
    auto const t54((t19 * (t1 * t32 * t8 * t26 * t34 * t21 / double(0.96e2) - t38 * ((-double(0.36e2) * t6 - double(0.12e2)) * t33 * t9 * t5 * t4 * t3 * t2 + t36 * t8 * t34 * t1 * t35) * t18 / double(0.16e2))));
    auto const t55((double(0.2e1) * t8));
    auto const t55_2(((-double(0.384e3) * t8 * t9 * t17 * t7 * t5 * t4 * t3 * t2 * t1 + t36 * t13 * (double(0.6e1) * t16 - double(0.36e2) * t14 * t15)) * t18 / double(0.48e2) + t27 * t20 * t21 * t22 * t23 * (t15 + (-t55 - t9) * t9) * (t15 + (t55 - t9) * t9) / double(0.48e2)));

    auto const _f_0((((double(0.24e2) * t16 - double(0.144e3) * t14 * t15) * t7 * t5 * t4 * t3 * t2 * t1 + double(0.6e1) * t9 * t12 * t11_2 * t13 * t8 * t17) * t18 / double(0.12e2) + t27 * t8 * t9 * t20 * t21 * t22 * t23 * t24 * t25 / double(0.12e2)));
    auto const _f_1((t19 * (t9 * t1 * t26 * t32 * t33 * t21 / double(0.32e2) + t39 * ((double(0.4e1) * t6 + double(0.4e1) / double(0.3e1)) * t34 * t8 * t5 * t4 * t3 * t2 + t36 * t9 * t33 * t1 * t35) * t38 * t18)));
    auto const _f_2((t21 * t9 * t31_2 * t26 * t8 / double(0.12e2) - t30_2 * (t7 * t50 * t9 * t8 + t51 * t24 * t25 * t4 * t5 * t1) * t18 / double(0.840e3)));
    auto const _f_3((t19 * t44_3 * t9 * t37_2 + t28_2 * (t49_2 * t9 + t10_2 * t8)));
    auto const _f_4((-t36 * (-t40 * t47 * t41_2 * t48 * t52 / double(0.2e1) + t26 * (t23 * t42 * t20 + double(0.6e1) * t43 * t40 * t47 * t48) * t22) * t18 / double(0.420e3) + t21 * (t46_2 * t45 * t29 * (t6 + t46_2) + (t6 - double(0.3e1) / double(0.2e1) * t26 * t40) * t6)));
    auto const _f_5((t19 * (t44_3 * t37_2 * t8 + t28_2 * (t49_2 * t8 - t10_2 * t9))));
    auto const &_f_6(t53_2);
    auto const _f_7((t54*double(1)));
    auto const &_f_8(t55_2);

    _f = _f_0 + _f_1 + _f_2 + _f_3 + _f_4 + _f_5 + _f_6 + _f_7 + _f_8;
  }
}
