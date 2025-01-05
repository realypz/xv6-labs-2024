#include "kernel/types.h"
#include "user/user.h"

int main(int argc, char *argv[])
{
  int pipe_parent_to_child[2];
  int pipe_child_to_parent[2];

  pipe(pipe_parent_to_child);
  pipe(pipe_child_to_parent);

  const int pid = fork();
  if (pid < 0)
  {
    fprintf(2, "fork failed\n");
    exit(1);
  }

  if (pid == 0) // In child process
  {
    close(pipe_parent_to_child[1]);
    char buf;
    read(pipe_parent_to_child[0], &buf, 1);
    if (buf == 'p')
    {
      printf("%d: received ping\n", getpid());
    }
    close(pipe_parent_to_child[0]);

    close(pipe_child_to_parent[0]);
    write(pipe_child_to_parent[1], "c", 1);
    close(pipe_child_to_parent[1]);

    exit(0);
  }
  else if (pid > 0) // In parent process
  {
    close(pipe_parent_to_child[0]);
    write(pipe_parent_to_child[1], "p", 1); // Parent sends one char (one byte) to child
    close(pipe_parent_to_child[1]);

    wait(0);

    close(pipe_child_to_parent[1]);
    char buf;
    read(pipe_child_to_parent[0], &buf, 1);
    if (buf == 'c')
    {
      printf("%d: received pong\n", getpid());
    }
    close(pipe_child_to_parent[0]);

    exit(0);
  }
  else
  {
    fprintf(2, "fork failed\n");
    exit(1);
  }

  exit(0);
}
