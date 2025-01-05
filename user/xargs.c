#include "kernel/types.h"
#include "kernel/param.h"
#include "user/user.h"

int main(int argc, char *argv[])
{
  char buf[512];

  const int standard_input = 0;
  read(standard_input, buf, sizeof(buf));

  char *args_new[MAXARG];

  for (int i = 1; i < argc; i++)
  {
    args_new[i-1] = argv[i];
  }

  int arg_index = argc - 1;
  char *p = buf;
  while (arg_index < MAXARG && *p != '\0')
  {
    args_new[arg_index] = p;
    arg_index++;
    while (*p != '\n')
    {
      p++;
    }

    // Now *p must be `\n`
    // Change it to null character,
    // and move to next character
    *p = 0;

    if (fork() == 0)
    {
      exec(args_new[0], args_new);
    }
    wait(0);

    // Restore arg_index for each new command, as requirement
    // "invoke the command on each line of input." says.
    arg_index = argc - 1;

    p++;
  }

  exit(0);
}
