struct Terminal
{
  auto opBinary(string op, R)(const ref R r)
  {
     enum l_adr = 0; // proposed: this.scope.offsetof + this.offsetof;
     enum r_adr = 4; // proposed: r.scope.offsetof + r.offsetof;

     return Binary!(op, typeof(this), R, l_adr, r_adr)(this, r);
  }
}

struct Binary(string op, L, R, int l_adr, int r_adr)
{
  L l;
  R r;
}

void main()
{
  Terminal c0;
  Terminal c1;
  pragma(msg, typeof(c0 + c1)); // 'Binary!("+", Terminal, Terminal, 0, 4)'

  // pragma(msg, typeof(transform(c0 + c0))); // becomes:  "2 * c0"
  // pragma(msg, typeof(transform(c0 + c1))); // becomes: "c0 + c1"
}
