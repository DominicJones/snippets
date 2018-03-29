// gdc hello.d -o hello -nophoboslib -s

extern(C) int puts(const char*);

// this is here to make the linker happy
extern(C) __gshared void* _Dmodule_ref; 

extern(C) int main()
{
  return puts("Hello world!");
}
