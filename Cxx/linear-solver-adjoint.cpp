SOLVE_B(A, Ab, y, yb, b, bb)
{
  At = TRANSPOSE(A);
  SOLVE(At, tmp, yb);

  bb[:] = bb[:] + tmp[:];
  SOLVE(A, y, b);

  for (each i and each j)
  {
    Ab[i,j] = Ab[i,j] - y[j]*tmp[i];
  }

  yb[:] = 0;
}
