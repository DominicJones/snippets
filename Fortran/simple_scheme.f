C======================================================
C.....START SIMPLE RELAXATIONS (OUTER ITERATIONS)
C======================================================
C
CALL CALCUV(KGR)
	C.....CALCULATE GRADIENT VECTOR COMPONENTS AT CV-CENTER FOR U, V & P
	C.....CALCULATE FLUXES THROUGH INNER CV-FACES (CALL FLUXUV)
	C.....PRESSURE SOURCE CONTRIBUTION
	C.....INLET BOUNDARIES (CONSTANT GRADIENT BETWEEN BOUNDARY & CV-CENTER ASSUMED) (CALL FLUXUV)
	C.....OUTLET BOUNDARIES (CONSTANT GRADIENT BETWEEN BOUNDARY & CV-CENTER ASSUMED) (CALL FLUXUV)
	C.....SYMMETRY BOUNDARIES
	C.....WALL BOUNDARIES
	C.....FINAL COEFFICIENT AND SOURCES MATRIX FOR U/V-EQUATION (under-relax)
	C.....SOLVING EQUATION SYSTEM FOR U-VELOCITY USING SIP-SOLVER

CALL CALCP(KGR,1)
	C.....CALCULATE FLUXES THROUGH INNER CV-FACES (CALL FLUXM)
	C.....INLET BOUNDARIES (MASS FLUXES PRESCRIBED IN ROUTINE 'BCIN')
	C.....EXTRAPOLATED VELOCITY AT OUTLET BOUNDARY, OUTLET MASS FLUXES
	C.....CORRECT MASS FLUX TO SATISFY GLOBAL MASS CONSERVATION & ADD TO SOURCE
	C.....SOURCE TERM AND CENTRAL COEFFICIENT (F1 and F2 are ZERO at boundaries!)
	C.....TEST GLOBAL MASS CONSERVATION & SOLVE EQUATIONS SYSTEM FOR P'
	C.....UPDATE PRESSURE CORECTION AT BOUNDARIES (extrapolate) (CALL PRESB)
	C.....CALCULATE PRESSURE-CORRECTION GRADIENTS, REFERENCE P'
	C.....CORRECT MASS FLUXES AT INNER CV-FACES
	C.....CORRECT PRESSURE AND VELOCITIES
	C.....UPDATE PRESSURE AT BOUNDARIES (extrapolate) (CALL PRESB)
	C.....UPDATE VELOCITY COMPONENTS ALONG SYMMETRY BOUNDARIES


CALL FLUXUV(IJ,IJ+NJ,IJ,IJ-1,F1(IJ),AW(IJ+NJ),AE(IJ),FX(IJ),GU)
	C.....INTERPOLATED CELL FACE VALUES
	C.....SURFACE AND DISTANCE VECTOR COMPONENTS, DIFFUSION COEFFICIENT
	C.....EXPLICIT CONVECTIVE AND DIFFUSIVE FLUXES
	C.....IMPLICIT CONVECTIVE AND DIFFUSIVE FLUXES
	C.....COEFFICIENTS, DEFERRED CORRECTION, SOURCE TERMS

CALL FLUXM(IJ,IJ+NJ,IJ,IJ-1,F1(IJ),AW(IJ+NJ),AE(IJ),FX(IJ))
	C.....INTERPOLATED CELL FACE VALUES
	C.....SURFACE AND DISTANCE VECTOR COMPONENTS
	C.....MASS FLUX, COEFFICIENTS FOR THE P'-EQUATION


C#########################################################
      SUBROUTINE CALCUV(K)
C#########################################################
C     This routine discretizes and solves the linearized
C     equations for X and Y momentum componentS (U and V
C     Cartesian velocity components). 
C=========================================================
C
C.....CALCULATE GRADIENT VECTOR COMPONENTS AT CV-CENTER FOR U, V & P
C
      CALL GRADFI(K,U,DUX,DUY)
      CALL GRADFI(K,V,DVX,DVY)
      CALL GRADFI(K,P,DPX,DPY)
C
C.....INITIALIZE ARRAYS, SET BLENDING FACTOR
C
      DO IJ=IJST,IJEN
        SU(IJ)=0.
        SV(IJ)=0.
        AP(IJ)=0.  ! AP(u-comp)
        APR(IJ)=0. ! AP(v-comp) --> 1 / AP(u)
      END DO
C
      GU=GDS(1)
C
C.....CALCULATE FLUXES THROUGH INNER CV-FACES: EAST
C
      DO I=2,NIM-1
      DO IJ=LI(I+IST)+2,LI(I+IST)+NJM
        CALL FLUXUV(IJ,IJ+NJ,IJ,IJ-1,F1(IJ),AW(IJ+NJ),AE(IJ),FX(IJ),GU)
      END DO
      END DO
C
C.....CALCULATE FLUXES THROUGH INNER CV-FACES: NORTH
C
      DO I=2,NIM
      DO IJ=LI(I+IST)+2,LI(I+IST)+NJM-1
        CALL FLUXUV(IJ,IJ+1,IJ-NJ,IJ,F2(IJ),AS(IJ+1),AN(IJ),FY(IJ),GU)
      END DO
      END DO
C
C.....PRESSURE SOURCE CONTRIBUTION
C
      DO I=2,NIM
      DO IJ=LI(I+IST)+2,LI(I+IST)+NJM
        SU(IJ)=SU(IJ)-DPX(IJ)*VOL(IJ)
        SV(IJ)=SV(IJ)-DPY(IJ)*VOL(IJ)
      END DO
      END DO
C
C.....INLET BOUNDARIES (CONSTANT GRADIENT BETWEEN BOUNDARY & CV-CENTER ASSUMED)
C
      DO II=IIS(K)+1,IIS(K)+NINL(K)
        IJP=IJPI(II)
        IJB=IJI(II)
        DUX(IJB)=DUX(IJP)
        DUY(IJB)=DUY(IJP)
        DVX(IJB)=DVX(IJP)
        DVY(IJB)=DVY(IJP)
        CALL FLUXUV(IJP,IJB,IJI1(II),IJI2(II),FMI(II),CP,CB,ONE,ZERO)
        AP(IJP) =AP(IJP) -CB
        APR(IJP)=APR(IJP)-CB
        SU(IJP)=SU(IJP)-CB*U(IJB)
        SV(IJP)=SV(IJP)-CB*V(IJB)
      END DO
C
C.....OUTLET BOUNDARIES (CONSTANT GRADIENT BETWEEN BOUNDARY & CV-CENTER ASSUMED)
C
      DO IO=IOS(K)+1,IOS(K)+NOUT(K)
        IJP=IJPO(IO)
        IJB=IJO(IO)
        DUX(IJB)=DUX(IJP)
        DUY(IJB)=DUY(IJP)
        DVX(IJB)=DVX(IJP)
        DVY(IJB)=DVY(IJP)
        CALL FLUXUV(IJP,IJB,IJO1(IO),IJO2(IO),FMO(IO),CP,CB,ONE,ZERO)
        AP(IJP)= AP(IJP) -CB
        APR(IJP)=APR(IJP)-CB
        SU(IJP)=SU(IJP)-CB*U(IJB)
        SV(IJP)=SV(IJP)-CB*V(IJB)
      END DO
C
C.....WALL BOUNDARIES
C
      DO IW=IWS(K)+1,IWS(K)+NWAL(K)
        IJP=IJPW(IW)
        IJB=IJW(IW)
        COEF=VIS(IJB)*SRDW(IW)
        AP(IJP) =AP(IJP) +COEF*XTW(IW)**2
        APR(IJP)=APR(IJP)+COEF*YTW(IW)**2
        SU(IJP)=SU(IJP)+COEF*(U(IJB)*XTW(IW)**2-
     *         (V(IJP)-V(IJB))*XTW(IW)*YTW(IW))
        SV(IJP)=SV(IJP)+COEF*(V(IJB)*YTW(IW)**2-
     *         (U(IJP)-U(IJB))*XTW(IW)*YTW(IW))
      END DO
C
C.....SYMMETRY BOUNDARIES
C
      DO IS=ISS(K)+1,ISS(K)+NSYM(K)
        IJP=IJPS(IS)
        IJB=IJS(IS)
        C1=2.*XNS(IS)*YNS(IS)
        COEF=VIS(IJB)*SRDS(IS)
        AP(IJP) =AP(IJP) +COEF*XNS(IS)**2
        APR(IJP)=APR(IJP)+COEF*YNS(IS)**2
        SU(IJP)=SU(IJP)-COEF*(C1*V(IJP)+U(IJP)*XNS(IS)**2)
        SV(IJP)=SV(IJP)-COEF*(C1*U(IJP)+V(IJP)*YNS(IS)**2)
      END DO
C
C.....FINAL COEFFICIENT AND SOURCES MATRIX FOR U-EQUATION
C
      DO I=2,NIM
      DO IJ=LI(I+IST)+2,LI(I+IST)+NJM
        AP(IJ)=(AP(IJ)-AE(IJ)-AW(IJ)-AN(IJ)-AS(IJ))*URFU
        SU(IJ)=SU(IJ)+(1.-URF(IU))*AP(IJ)*U(IJ)
      END DO
      END DO
C
C.....SOLVING EQUATION SYSTEM FOR U-VELOCITY USING SIP-SOLVER
C
      CALL SIPSOL(U,IU,K)
C 
C.....FINAL COEFFICIENT AND SOURCES MATRIX FOR V-EQUATION
C
      DO I=2,NIM
      DO IJ=LI(I+IST)+2,LI(I+IST)+NJM
        AP(IJ)=(APR(IJ)-AE(IJ)-AW(IJ)-AN(IJ)-AS(IJ))*URFV
        SU(IJ)=SV(IJ)+(1.-URF(IV))*AP(IJ)*V(IJ)
        APR(IJ)=1./(AP(IJ)+SMALL)
      END DO
      END DO
C
C.....SOLVING EQUATION SYSTEM FOR V-VELOCITY USING SIP-SOLVER
C
      CALL SIPSOL(V,IV,K)
C
C############################################################## 
      SUBROUTINE CALCP(K,INTMF) 
C############################################################## 
C     This routine assembles and solves the pressure-correction
C     equation using colocated grid. SIMPLE algorithm with one
C     corrector step (non-orthogonality effects neglected) is
C     applied.
C==============================================================
C
      DO IJ=IJST,IJEN
        SU(IJ)=0.
        AP(IJ)=0.
      END DO
C
C.....CALCULATE FLUXES THROUGH INNER CV-FACES (EAST & NORTH)
C
      DO I=2,NIM-1
      DO IJ=LI(I+IST)+2,LI(I+IST)+NJM
        CALL FLUXM(IJ,IJ+NJ,IJ,IJ-1,F1(IJ),AW(IJ+NJ),AE(IJ),FX(IJ))
      END DO
      END DO
C
      DO I=2,NIM
      DO IJ=LI(I+IST)+2,LI(I+IST)+NJM-1
        CALL FLUXM(IJ,IJ+1,IJ-NJ,IJ,F2(IJ),AS(IJ+1),AN(IJ),FY(IJ))
      END DO
      END DO
C
C.....INLET BOUNDARIES (MASS FLUXES PRESCRIBED IN ROUTINE 'BCIN')
C
      DO II=IIS(K)+1,IIS(K)+NINL(K)
        SU(IJPI(II))=SU(IJPI(II))-FMI(II)
      END DO
C
C.....EXTRAPOLATED VELOCITY AT OUTLET BOUNDARY, OUTLET MASS FLUXES
C
      FLOWO=0.
      DO IO=IOS(K)+1,IOS(K)+NOUT(K)
        IJB=IJO(IO)
        IJP=IJPO(IO)
        U(IJB)=U(IJP)
        V(IJB)=V(IJP)
        RS=0.5*(R(IJO1(IO))+R(IJO2(IO)))
        SX=(Y(IJO1(IO))-Y(IJO2(IO)))
        SY=(X(IJO2(IO))-X(IJO1(IO)))
        FMO(IO)=DEN(IJP)*(U(IJB)*SX+V(IJB)*SY)*RS
        FLOWO=FLOWO+FMO(IO)
      END DO
C
C.....CORRECT MASS FLUX TO SATISFY GLOBAL MASS CONSERVATION & ADD TO SOURCE
C
      FAC=FLOMAS/(FLOWO+SMALL)
      DO IO=IOS(K)+1,IOS(K)+NOUT(K)
        IB=IJO(IO)
        FMO(IO)=FMO(IO)*FAC
        U(IB)=U(IB)*FAC
        V(IB)=V(IB)*FAC
        SU(IJPO(IO))=SU(IJPO(IO))-FMO(IO)
      END DO
C
C.....SOURCE TERM AND CENTRAL COEFFICIENT (F1 and F2 are ZERO at boundaries!)
C
      DO I=2,NIM
        DO IJ=LI(I+IST)+2,LI(I+IST)+NJM
          PP(IJ)=0.
          SU(IJ)=SU(IJ)+F1(IJ-NJ)-F1(IJ)+F2(IJ-1)-F2(IJ)
          AP(IJ)=AP(IJ)-AE(IJ)-AW(IJ)-AN(IJ)-AS(IJ)
          SUM=SUM+SU(IJ)
        END DO
      END DO
C    
C.....TEST GLOBAL MASS CONSERVATION & SOLVE EQUATIONS SYSTEM FOR P'
C
      CALL SIPSOL(PP,IP,K)
C
C.....UPDATE PRESSURE CORECTION AT BOUNDARIES
C
      CALL PRESB(K,PP)
C
C.....CALCULATE PRESSURE-CORRECTION GRADIENTS, REFERENCE P'
C
      CALL GRADFI(K,PP,DPX,DPY)
      PPO=PP(IJPR)
C
C.....CORRECT MASS FLUXES AT INNER CV-FACES
C
      DO I=2,NIM-1
        DO IJ=LI(I+IST)+2,LI(I+IST)+NJM
          F1(IJ)=F1(IJ)+AE(IJ)*(PP(IJ+NJ)-PP(IJ))
        END DO
      END DO
C
      DO I=2,NIM
        DO IJ=LI(I+IST)+2,LI(I+IST)+NJM-1
          F2(IJ)=F2(IJ)+AN(IJ)*(PP(IJ+1)-PP(IJ))
        END DO
      END DO
C
      DO I=IOCS(K)+1,IOCS(K)+NOC(K)
        FMOC(I)=FMOC(I)+AR(I)*(PP(IJR(I))-PP(IJL(I)))
      END DO
C
C.....CORRECT PRESSURE AND VELOCITIES
C
      DO I=2,NIM
        DO IJ=LI(I+IST)+2,LI(I+IST)+NJM
          U(IJ)=U(IJ)-DPX(IJ)*VOL(IJ)*APR(IJ)
          V(IJ)=V(IJ)-DPY(IJ)*VOL(IJ)*APR(IJ)
          P(IJ)=P(IJ)+URF(IP)*(PP(IJ)-PPO)
        END DO
      END DO
C
C.....UPDATE PRESSURE AT BOUNDARIES
C
      CALL PRESB(K,P)
C
C.....UPDATE VELOCITY COMPONENTS ALONG SYMMETRY BOUNDARIES
C
      DO IS=ISS(K)+1,ISS(K)+NSYM(K)
        IJP=IJPS(IS)
        IJB=IJS(IS)
        UN=U(IJP)*XNS(IS)+V(IJP)*YNS(IS)
        U(IJB)=U(IJP)-UN*XNS(IS)
        V(IJB)=V(IJP)-UN*YNS(IS)
      END DO
C
C################################################################
      SUBROUTINE FLUXUV(IJP,IJN,IJ1,IJ2,FM,CAP,CAN,FAC,G)
C################################################################
C     This routine calculates momentum fluxes (convective and
C     diffusive) through the cell face between nodes IJP and IJN. 
C     IJ1 and IJ2 are the indices of CV corners defining the cell 
C     face. FM is the mass flux through the face, and FAC is the 
C     interpolation factor (distance from node IJP to cell face 
C     center over the sum of this distance and the distance from 
C     cell face center to node IJN). CAP and CAN are the 
C     contributions to matrix coefficients in the momentum
C     equations at nodes IJP and IJN. Diffusive fluxes are
C     discretized using central differences; for convective
C     fluxes, linear interpolation can be blended with upwind
C     approximation; see Sect. 8.6 for details. Note: cell
C     face surface vector is directed from P to N.
C
C     IN : FM,FAC,G
C     OUT : CAP,CAN
C==============================================================
C
C.....INTERPOLATED CELL FACE VALUES
C
      FACP=1.-FAC
      UI=U(IJN)*FAC+U(IJP)*FACP
      VI=V(IJN)*FAC+V(IJP)*FACP
      VISI=VIS(IJN)*FAC+VIS(IJP)*FACP
      DUXI=DUX(IJN)*FAC+DUX(IJP)*FACP
      DVXI=DVX(IJN)*FAC+DVX(IJP)*FACP
      DUYI=DUY(IJN)*FAC+DUY(IJP)*FACP
      DVYI=DVY(IJN)*FAC+DVY(IJP)*FACP
C
C.....SURFACE AND DISTANCE VECTOR COMPONENTS, DIFFUSION COEFFICIENT
C
      RC=0.5*(R(IJ1)+R(IJ2))
      SX=(Y(IJ1)-Y(IJ2))*RC
      SY=(X(IJ2)-X(IJ1))*RC
      XPN=XC(IJN)-XC(IJP)
      YPN=YC(IJN)-YC(IJP)
      VSOL=VISI*SQRT((SX**2+SY**2)/(XPN**2+YPN**2))
C
C.....EXPLICIT CONVECTIVE AND DIFFUSIVE FLUXES
C
      FCUE=FM*UI
      FCVE=FM*VI
      FDUE=VISI*(2.*DUXI*SX+(DUYI+DVXI)*SY)
      FDVE=VISI*((DUYI+DVXI)*SX+2.*DVYI*SY)
C
C.....IMPLICIT CONVECTIVE AND DIFFUSIVE FLUXES
C
      FCUI=MIN(FM,ZERO)*U(IJN)+MAX(FM,ZERO)*U(IJP)
      FCVI=MIN(FM,ZERO)*V(IJN)+MAX(FM,ZERO)*V(IJP)
      FDUI=VSOL*(DUXI*XPN+DUYI*YPN)
      FDVI=VSOL*(DVXI*XPN+DVYI*YPN)
C
C.....COEFFICIENTS, DEFERRED CORRECTION, SOURCE TERMS
C
      CAN=-VSOL+MIN(FM,ZERO)
      CAP=-VSOL-MAX(FM,ZERO)
      FUC=G*(FCUE-FCUI)
      FVC=G*(FCVE-FCVI)
C
      SU(IJP)=SU(IJP)-FUC+FDUE-FDUI
      SU(IJN)=SU(IJN)+FUC-FDUE+FDUI
      SV(IJP)=SV(IJP)-FVC+FDVE-FDVI
      SV(IJN)=SV(IJN)+FVC-FDVE+FDVI
C
C##############################################################
      SUBROUTINE FLUXM(IJP,IJN,IJ1,IJ2,FM,CAP,CAN,FAC)
C##############################################################
C     This routine calculates mass flux through the cell face 
C     between nodes IJP and IJN. IJ1 and IJ2 are the indices of 
C     CV corners defining the cell face. FM is the mass flux 
C     through the face, and FAC is the interpolation
C     factor (distance from node IJP to cell face center over
C     the sum of this distance and the distance from cell face 
C     center to node IJN). CAP and CAN are the contributions to
C     matrix coefficients in the pressure-correction equation
C     at nodes IJP and IJN. Surface vector directed from P to N.
C
C     IN : FAC,G
C     OUT : FM,CAP,CAN
C==============================================================
C
C.....INTERPOLATED CELL FACE VALUES
C
      FACP=1.-FAC
      UI=U(IJN)*FAC+U(IJP)*FACP
      VI=V(IJN)*FAC+V(IJP)*FACP
      DENI=DEN(IJN)*FAC+DEN(IJP)*FACP
C
C.....SURFACE AND DISTANCE VECTOR COMPONENTS
C
      RC=0.5*(R(IJ1)+R(IJ2))
      SX=(Y(IJ1)-Y(IJ2))*RC
      SY=(X(IJ2)-X(IJ1))*RC
      XPN=XC(IJN)-XC(IJP)
      YPN=YC(IJN)-YC(IJP)
      SMDPN=(SX**2+SY**2)/(SX*XPN+SY*YPN)   
C
C.....MASS FLUX, COEFFICIENTS FOR THE P'-EQUATION
C
      CAP=-0.5*(VOL(IJP)*APR(IJP)+VOL(IJN)*APR(IJN))*DENI*SMDPN
      CAN=CAP
      DPXI=0.5*(DPX(IJN)+DPX(IJP))*XPN
      DPYI=0.5*(DPY(IJN)+DPY(IJP))*YPN
      FM=DENI*(UI*SX+VI*SY)+CAP*(P(IJN)-P(IJP)-DPXI-DPYI) ! <-- mass flux
C
C###############################################################
      SUBROUTINE PRESB(K,FI)
C###############################################################
C     This routine extrapolates the pressure or pressure
C     correction from interior to the boundary.
C==============================================================
C
C.....EXTRAPOLATE TO SOUTH AN NORTH BOUNDARIES
C
      DO I=2,NIM
        IJ=LI(I+IST)+1
        FI(IJ)=FI(IJ+1)+(FI(IJ+1)-FI(IJ+2))*FY(IJ+1)
        IJ=LI(I+IST)+NJ
        FI(IJ)=FI(IJ-1)+(FI(IJ-1)-FI(IJ-2))*(1.-FY(IJ-2)) 
      END DO 
C
C.....EXTRAPOLATE TO WEST AND EAST BOUNDARIES
C
      DO J=2,NJM
        IJ=LI(1+IST)+J
        FI(IJ)=FI(IJ+NJ)+(FI(IJ+NJ)-FI(IJ+NJ+NJ))*FX(IJ+NJ) 
        IJ=LI(NI+IST)+J
        FI(IJ)=FI(IJ-NJ)+(FI(IJ-NJ)-FI(IJ-NJ-NJ))*(1.-FX(IJ-NJ-NJ))
      END DO
C
