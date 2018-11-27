/*
 * not helpful but considered:
 *   typeof(name), __traits(identifier, name))
 *   mixin template Unique(alias name)
 */

import std.stdio;

struct Unique(T, string s)
{
    T value;
}

mixin template MakeUnique(T, string s, Args...)
{
    mixin("auto "~s~" = Unique!(T, s)("~Args[0].stringof~");");
}

void main()
{
    auto value = 123;
    mixin MakeUnique!(int, "var", value);
    pragma(msg, typeof(var));             // Unique!(int, "var")
    writeln(var.value);                   // 123
}
