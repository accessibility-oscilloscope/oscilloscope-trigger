
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
  printf(currentPath);

  /* Open the command for reading. */
  printf("%s/%s", currentPath, "get-btn.sh");
  sprintf(currentPath, "%s/%s", currentPath, "get-btn.sh");
  fp = popen(currentPath, "r");
  if (fp == NULL) {
    printf("Failed to run command\n" );
    exit(1);
  }

  printf("reading");
  /* Read the output a line at a time - output it. */
  while (fgets(output, sizeof(output), fp) != NULL) {
    printf("%s", output);
  }

  /* close */
  pclose(fp);

  return 0;
}
