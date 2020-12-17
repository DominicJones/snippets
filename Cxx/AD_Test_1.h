// -*- C++ -*-
#pragma once

struct AD_Test_1
{
  static inline void
  evaluate(double const &a,
           double const &b,
           double &r) noexcept
  {
    auto const c1 = (double(1));
    auto const c2 = (double(2));
    auto const c3 = (double(3));
    auto const c4 = (double(4));
    auto const c5 = (double(5));
    auto const c6 = (double(6));
    auto const c7 = (double(7));
    auto const c8 = (double(8));
    auto const c9 = (double(9));
    auto const c10 = (double(10));
    auto const c11 = (double(11));
    auto const c12 = (double(12));
    auto const tmp0 = (c1 * a * sin(a) / cos(b) * c2);
    auto const tmp1 = (b * c3 * sin(b) / cos(a));
    auto const tmp2 = (sin(tmp0) * tmp1 + sin(tmp1) * tmp0);
    auto const tmp3 = (sin(tmp2) / cos(tmp2) + cos(tmp0) * c4 / sin(tmp0));
    auto const tmp4 = (sin(tmp2) / c5 * cos(tmp2) + cos(tmp1) / sin(tmp3));
    auto const tmp5 = (sin(tmp2) * cos(tmp4) + cos(tmp0) * sin(tmp1));
    auto const tmp6 = (c6 * sin(tmp2) / cos(tmp5) + cos(tmp0) / sin(tmp3));
    auto const tmp7 = (sin(tmp4) + cos(tmp6) + cos(tmp0) * c7 / sin(tmp5));
    auto const tmp8 = (sin(tmp1) / cos(tmp5) * cos(tmp0) * sin(tmp6));
    auto const tmp9 = (sin(tmp6) + c8 * cos(tmp4) + cos(tmp0) / sin(tmp1));
    auto const tmp10 = (sin(tmp5) / cos(tmp3) + cos(tmp7) + sin(tmp2));
    auto const tmp11 = (sin(tmp8) * c9 * cos(tmp2) + cos(tmp0) / sin(tmp3));
    auto const tmp12 = (c10 * sin(tmp10) / cos(tmp1) + cos(tmp0) / sin(tmp4));
    auto const tmp13 = (sin(tmp12) + cos(tmp0) * c11 + cos(tmp0) / sin(tmp3));
    auto const tmp14 = (cos(tmp13 / sin(tmp0) + tmp6 / sin(tmp11) * c12 * tmp13 * tmp11 * tmp9));
    r = tmp14;
  }
};
