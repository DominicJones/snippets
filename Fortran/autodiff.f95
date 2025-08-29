! http://www-tapenade.inria.fr:8080/tapenade/

SUBROUTINE HYPOT(a, b, r)
  IMPLICIT NONE
  REAL :: a, b, r, t0, t1, t2
  INTRINSIC SQRT
  t0 = a*a
  t1 = b*b
  t2 = SQRT(t0 + t1)
  r = t2
END SUBROUTINE
