SUBROUTINE EVALUATE(z)
  IMPLICIT NONE
  REAL :: z
  INTRINSIC SIN
!$AD FP-LOOP "z"
  DO WHILE (true)
    z = SIN(z)
  END DO
END SUBROUTINE EVALUATE



! Generated by TAPENADE (INRIA, Ecuador team)
SUBROUTINE EVALUATE_B(z, zb1)
  IMPLICIT NONE
  REAL :: z
  REAL :: zb1
  INTRINSIC SIN
  REAL :: cumul
  REAL :: zb
  REAL :: zb0
  
  DO WHILE (true)
    z = SIN(z)
  END DO
  CALL PUSHREAL4(z)
  z = SIN(z)
  
  cumul = 1.0
  zb = zb1
  CALL ADSTACK_STARTREPEAT()
  DO WHILE (cumul .GT. 1.0e-6)
    zb0 = zb1
    CALL POPREAL4(z)
    zb1 = COS(z)*zb1
    cumul = 0.0
    zb1 = zb1 + zb
    cumul = cumul + (zb1-zb0)**2
    CALL ADSTACK_RESETREPEAT()
  END DO
  
  CALL ADSTACK_ENDREPEAT()
  CALL POPREAL4(z)
  zb1 = 0.0
END SUBROUTINE EVALUATE_B
