
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#ifndef MAX_BUF
#define MAX_BUF 200
#endif

int main( int argc, char *argv[] )
{
  printf("test");
  FILE *fp;
  char currentPath[MAX_BUF];
  getcwd(currentPath, MAX_BUF);
  char output[1035];

  /* Open the command for reading. */
  sprintf(currentPath, "%s/%s", currentPath, "get-btn.sh");
  fp = popen(currentPath, "r");
  if (fp == NULL) {
    printf("Failed to run command\n" );
    exit(1);
  }

  /* Read the output a line at a time - output it. */
  while (fgets(output, sizeof(output), fp) != NULL) {
    printf("%s", output);
    // TODO: Trigger USB Communcation and send to pipe
  }

  /* close */
  pclose(fp);

  return 0;
}
