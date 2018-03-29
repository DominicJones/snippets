// -*- C++ -*-

#ifndef _Moments_h_
#define _Moments_h_

#include <cmath>
#include <vector>


/*
  Given an array of data, provide:
  mean -- ave,
  average deviation -- adev,
  standard deviation -- sdev,
  variance -- var,
  skewness -- skew,
  kurtosis -- kurt.
*/


class Moments
{
public:
  Moments(std::vector<double> const &data)
    : _data(data)
  {}

  void evaluate()
  {
    eval(_data.data(), _data.size(), &_ave, &_adev, &_sdev, &_var, &_skew, &_kurt);
  }

  double ave() const { return _ave; }
  double adev() const { return _adev; }
  double sdev() const { return _sdev; }
  double var() const { return _var; }
  double skew() const { return _skew; }
  double kurt() const { return _kurt; }

private:
  void eval(double const data[],
            int const n,
            double *ave,
            double *adev,
            double *sdev,
            double *var,
            double *skew,
            double *kurt)
  {
    if (n <= 1) return;

    double ep = 0;
    double s = 0;
    double p;

    for (int j=1;j<=n;j++) { s += data[j]; }

    *ave=s/n;
    *adev=(*var)=(*skew)=(*kurt)=0;

    for (int j=1;j<=n;j++)
    {
      *adev += std::abs(s=data[j]-(*ave));
      ep += s;
      *var += (p=s*s);
      *skew += (p *= s);
      *kurt += (p *= s);
    }

    *adev /= n;
    *var=(*var-ep*ep/n)/(n-1);
    *sdev=std::sqrt(*var);

    if (*var)
    {
      *skew /= (n*(*var)*(*sdev));
      *kurt=(*kurt)/(n*(*var)*(*var))-3;
    }
  }

  std::vector<double> const &_data;
  double _ave = 0;
  double _adev = 0;
  double _sdev = 0;
  double _var = 0;
  double _skew = 0;
  double _kurt = 0;
};


#endif // _Moments_h_
