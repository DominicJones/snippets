/+ input file:
  U = [1.12, 2, -4.2e-4]; // ref. velocity
  quit = [1000, 0.001]; // iter. cut-off (max iter, min norm)
 +/

import
  std.stdio, std.conv, std.regex,
  std.typetuple, std.string;


void main()
{
  /// open input file
  File f;
  f.open("input.txt", "r");


  /// parse input file
  string input[string];
  string key, val;

  auto lvalFmt = regex(r"^\s*(\w+)\s*=");
  auto rvalFmt = regex(r"=\s*(.+)\s*;");

  foreach(ref string line; lines(f))
  {
    auto lval = match(line, lvalFmt);
    if(lval.captures.length > 1)
    {
      auto rval = match(line, rvalFmt);
      if(rval.captures.length > 1)
      {
        /// store entry
        key = lval.captures[1];
        val = rval.captures[1];
        input[key] = val;
      }
    }
  }


  /// initialize variables
  auto U = new double[3];
  TypeTuple!(int, double) quit;

  U = parse!(typeof(U))(input["U"]);

  auto buf = input["quit"];
  foreach(i, T; quit)
  {
    munch(buf, "[, ]");
    quit[i] = parse!(typeof(T))(buf);
  }

  writeln(U);
  writeln([quit]);
}
