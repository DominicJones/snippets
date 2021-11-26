! https://www.onlinegdb.com/online_fortran_compiler

SUBROUTINE selfdyad(x, d)
  REAL, dimension(3)::x
  REAL, dimension(3,3)::d
  DO i = 1, 3
  DO j = 1, 3
  d(i,j) = x(i)*x(j)
  ENDDO
  ENDDO
END SUBROUTINE

SUBROUTINE dyad(x, y, d)
  REAL, dimension(3)::x
  REAL, dimension(3)::y
  REAL, dimension(3,3)::d
  DO i = 1, 3
  DO j = 1, 3
  d(i,j) = x(i)*y(j)
  ENDDO
  ENDDO
END SUBROUTINE

PROGRAM main
  REAL, dimension(3)::x
  REAL, dimension(3)::y
  REAL, dimension(3,3)::d
  x(1) = 2
  y(1) = 3
  call dyad(x, y, d)
  PRINT *, 'd ', d(1,1)
END PROGRAM
