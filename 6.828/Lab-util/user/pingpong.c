#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int
main(int argc, char *argv[]) {
  int fds[2], pid;
  char c[1];
  c[0] = 'a';
  if(pipe(fds) < 0){
    printf("pipe() failed\n");
    exit(1);
  }
  pid = fork();
  if (pid == 0) { // child
    read(fds[0], c, 1);
    printf("%d: received ping\n", getpid());
    write(fds[1], c, 1);
  } else if(pid > 0) { // parent
    write(fds[1], c, 1);
    read(fds[0], c, 1);
    printf("%d: received pong\n", getpid());
  } else {
    printf("fork() failed\n");
    exit(1);
  }

  exit(0);
}