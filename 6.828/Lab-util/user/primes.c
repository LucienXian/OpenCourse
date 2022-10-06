#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

void readsource(int fd);
void sink(int fd);
void cal(int read_fd, int write_fd, int prime);

int
main(int argc, char *argv[]) {
  int fds[2], pid;
  if(pipe(fds) < 0) {
    printf("pipe() failed\n");
    exit(1);
  }

  if(pipe(fds) < 0){
    printf("pipe() failed\n");
    exit(1);
  }
  pid = fork();
  if (pid == 0) { // child
    close(fds[0]); // close read
    readsource(fds[1]);
  } else if(pid > 0) { // parent
    close(fds[1]);
    sink(fds[0]);
  } else {
    printf("fork() failed\n");
    exit(1);
  }
  exit(0);
}

void readsource(int fd) {
  int i;
  for (i = 2; i <= 35; ++i) {
    write(fd, &i, sizeof(i));
  }
  close(fd);
}

void sink(int fd) {
  int fds[2], pid;
  int prime;
  if (read(fd, &prime, sizeof(prime))) {
    printf("prime %d\n", prime);
    if(pipe(fds) < 0){
      printf("pipe() failed\n");
      exit(1);
    }
    pid = fork();
    if (pid == 0) { // child
      close(fds[0]);
      cal(fd, fds[1], prime); // read from inherit parent
    } else {
      close(fd);
      close(fds[1]); // close write
      sink(fds[0]);
    }
  }
}

void cal(int read_fd, int write_fd, int prime) {
  int p;
  while (read(read_fd, &p, sizeof(p))) {
    if (p % prime != 0) {
      write(write_fd, &p, sizeof(p));
    }
  }
  close(read_fd);
  close(write_fd);
}