#include <vector>
#include <iostream>
#include <cmath>
#include <cstdlib>

typedef long double Real;

Real pNorm(int expnt, std::vector<int> &values) {
  Real norm = Real(0);
  for (int i = 0; i != values.size(); ++i) {
    Real base = abs(Real(values[i]));
    norm += pow(base, expnt);
  }
  Real expnt_r = Real(1) / expnt;
  norm = pow(norm, expnt_r);
  return norm;
}

int main() {
  int refVals_[] = {16, 2, 77, 29};
  std::vector<int> refVals(refVals_, refVals_ + sizeof(refVals_) / sizeof(int));
  std::vector<Real> refNorms;

  int cmpVals_[] = {17, 3, 78, 28};
  std::vector<int> cmpVals(cmpVals_, cmpVals_ + sizeof(cmpVals_) / sizeof(int));
  std::vector<Real> cmpNorms;

  std::vector<int> delVals(refVals);
  for (int i = 0; i != delVals.size(); ++i) { delVals[i] = cmpVals[i] - refVals[i]; }
  std::vector<Real> delNorms;

  int nNorm = 4;

  for (int i = 1; i != nNorm; ++i) {
    refNorms.push_back(pNorm(i, refVals));
  }

  for (int i = 1; i != nNorm; ++i) {
    cmpNorms.push_back(pNorm(i, cmpVals));
  }

  for (int i = 1; i != nNorm; ++i) {
    delNorms.push_back(pNorm(i, delVals));
  }

  for (int i = 1; i != nNorm; ++i) {
    std::cout << refNorms[i-1] << " "
              << cmpNorms[i-1] << " "
              << cmpNorms[i-1] / refNorms[i-1] << " "
              << delNorms[i-1] << " "
              << std::endl;
  }
}
