#include <cassert>


char shiftHigh(char a, char i = 1)
{
  return a << i;
}

char shiftLow(char a, char i = 1)
{
  return a >> i;
}

char complement(char a)
{
  return ~a;
}

char bitwiseAND(char a, char b)
{
  return a & b;
}

char bitwiseOR(char a, char b)
{
  return a | b;
}

char bitwiseXOR(char a, char b)
{
  return a ^ b;
}


int main()
{
  {
    char a(0b01001000);
    char r(shiftHigh(a));
    char t(0b10010000);
    assert(r == t);
  }

  {
    char a(0b01001000);
    char r(shiftHigh(a, 2));
    char t(0b00100000);
    assert(r == t);
  }

  {
    char a(0b01001000);
    char r(shiftLow(a));
    char t(0b00100100);
    assert(r == t);
  }

  {
    char a(0b01001000);
    char r(shiftLow(a, 2));
    char t(0b00010010);
    assert(r == t);
  }

  {
    char a(0b01001000);
    char r(complement(a));
    char t(0b10110111);
    assert(r == t);
  }

  {
    char a(0b01001000);
    char b(0b10111000);
    char r(bitwiseAND(a, b));
    char t(0b00001000);
    assert(r == t);
  }

  {
    char a(0b01001000);
    char b(0b10111000);
    char r(bitwiseOR(a, b));
    char t(0b11111000);
    assert(r == t);
  }

  {
    char a(0b01001000);
    char b(0b10111000);
    char r(bitwiseXOR(a, b));
    char t(0b11110000);
    assert(r == t);
  }
}
