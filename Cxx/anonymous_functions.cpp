// A simple example of using lambda functions
//
// Note:
// Template lambda functions are not supported in C++11,
// though are expected to be in C++14.

#include <iostream>
#include <functional>

int eval(std::function<int (int, int)> opBinary, int op1, int op2)
{
    return opBinary(op1, op2);
}

auto opBinaryAdd  = [](int a, int b) -> int { return a + b; };
auto opBinaryMult = [](int a, int b) -> int { return a * b; };

int main()
{
    int op1 = 3;
    int op2 = 4;
    std::cout << eval(opBinaryAdd,  op1, op2) << std::endl;
    std::cout << eval(opBinaryMult, op1, op2) << std::endl;
}
