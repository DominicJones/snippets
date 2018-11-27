void main()
{
    import std.stdio;
    
    int var = 123;
    pragma(msg, typeof(var));                       // int
    pragma(msg, typeof(__traits(identifier, var))); // string
    writeln(var);                                   // 123
    writeln(__traits(identifier, var));             // "var"
}
