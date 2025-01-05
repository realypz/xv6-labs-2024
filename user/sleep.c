#include "kernel/types.h"
#include "user/user.h"

int main(int argc, char *argv[])
{
  if(argc != 2){
    fprintf(2, "usage: sleep <number>\n");
    exit(1);
  }

  const int sleep_ticks = atoi(argv[1]);
  sleep(sleep_ticks);

  exit(0);
}
