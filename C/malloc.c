#include <stdio.h>
#include <stdlib.h>


int size;
double * data;


void fill(double ** buffer, int length)
{
  *buffer = (double *) malloc(sizeof(double) * length);

  for (int it = 0; it != length; ++it)
    (*buffer)[it] = 1;
}


int main()
{
  size = 3;
  fill(&data, size);

  for (int it = 0; it != size; ++it)
    printf("%g\n", data[it]);
}
