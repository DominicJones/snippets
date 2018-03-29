#include <stdio.h>
#include <stdlib.h>

struct Data {
  int* idx;
  int idxLen;
};

char rt_init(long long);
char rt_term(long long);
void procData(struct Data*);

void main() {
  int i;
  struct Data *data;
  data = malloc(sizeof(struct Data));

  data->idxLen = 5;

  /// call D functions
  rt_init(0); /// initialize core.runtime
  procData(data);
  rt_term(0); /// terminate core.runtime

  printf("for [0..%d)\n", data->idxLen);
  for (i = 0; i < data->idxLen; ++i) {
    printf("-%d^%d = %d\n", i, i, data->idx[i]);
  }
}
