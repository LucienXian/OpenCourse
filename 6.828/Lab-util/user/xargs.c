#include "kernel/types.h"
#include "kernel/stat.h"
#include "kernel/param.h"
#include "user/user.h"
#include "kernel/fs.h"

void split(char *src, char* dest) {

}

int
main(int argc, char *argv[])
{
  char *argvs[MAXARG];
  char param[512], c;
  int param_index = 0;
  int i;
  for (i = 1; i < argc; i++) {
    argvs[i-1] = argv[i];
  }
  argvs[argc - 1] = param;
  while (read(0, &c, 1)) {
    if (c == ' ' || c == '\n') {
      // fork and exec
      param[param_index] = 0;
      if (fork() > 0) { // parent
        wait(0);
        param_index = 0;
      } else {
        exec(argvs[0], argvs);
      }
    } else {
      param[param_index++] = c;
    }
  }
  exit(0);
}