// from DDJ/WalterBright

import std.stdio;

void main()
{
  writeln(clock());
  writeln(tan(1.0));
}

long clock()
{
   // Long's are returned in EDX and EAX
   asm
   {
     rdtsc;
   }
}

real tan(real x)
{
  asm
  {
    fld     x[EBP]   ; // load x
    fxam             ; // test for oddball values
    fstsw   AX       ;
    sahf             ;
    jc      trigerr  ; // x is NAN, infinity, or empty
                       // 387's can handle denormals
SC18:
    fptan            ;
    fstp    ST(0)    ; // dump X, which is always 1
    fstsw   AX       ;
    sahf             ;
    jnp     Lret     ; // C2 = 1 (x is out of range)
    // Do argument reduction to bring x into range
    fldpi            ;
    fxch             ;
SC17:
    fprem1           ;
    fstsw   AX       ;
    sahf             ;
    jp      SC17     ;
    fstp    ST(1)    ; // remove pi from stack
    jmp     SC18     ;
  }
trigerr:
  return real.nan;
Lret:
  ;
}
