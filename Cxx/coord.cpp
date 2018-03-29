int to1D(int const &ny, int const &nz, int const &x, int const &y, int const &z)
{
  return x + ny * (y + nz * z);
}


std::array<int, 3> to3D(int const &ny, int const &nz, int const &xyz)
{
  int const z = xyz % nz;
  int const y = (xyz / nz) % ny;
  int const x = xyz / (ny * nz);
  return std::array<int, 3>({x,y,z});
}
