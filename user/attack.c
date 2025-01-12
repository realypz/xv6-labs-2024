#include "kernel/types.h"
#include "kernel/fcntl.h"
#include "user/user.h"
#include "kernel/riscv.h"

int
main(int argc, char *argv[])
{
  // your code here.  you should write the secret to fd 2 using write
  // (e.g., write(2, secret, 8)
  int n_page = 17;
  char *end = sbrk(n_page * PGSIZE);
  end += (n_page - 1) * PGSIZE;
  write(2, end + 32, 8);
  exit(1);
}
