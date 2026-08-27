#include <stdlib.h>

int main( int argc, char *argv[] )  {
  
  int size = atoi(argv[1]);
  int elements = size * 1024 * 1024 / sizeof(int);
  
  calloc(elements, sizeof(int));
  
}