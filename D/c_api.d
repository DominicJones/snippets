import std.stdio, std.c.stdlib;

struct DataCp {
  int[] idx;
}

extern (C)
struct Data {
  int* idx;
  int idxLen;
}

extern (C)
void procData(ref Data data)
{
  writeln("entered D function");

  /// allocate array
  data.idx = cast(typeof(data.idx))
    malloc(data.idxLen * (*data.idx).sizeof);

  /// initialize elements
  for (int i=0; i<data.idxLen; ++i) {
    data.idx[i] = i^^i;
  }

  /// reference array with D array
  DataCp dataCp;
  dataCp.idx = data.idx[0..data.idxLen];

  /// modify elements via D array
  foreach (ref e; dataCp.idx) {
    e = -e;
  }

  writeln("exiting D function");
}
