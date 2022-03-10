#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#ifndef MAX_BUF
#define MAX_BUF 200
#endif



const uint8_t INIT_SEQ[] = {
    0,   20,                         // board ID 0
    135, 0,  100,                    // serial delay of 100
    122, 0,  10,                     // 10 samples per packet
    123, 0,                          // increment
    124, 3,                          // downsample 3
    125, 1,                          // 1 tick wait
    136, 2,  32,  0,  0,   255, 200, // io expander on
    136, 2,  32,  1,  0,   255, 200, //  io expander on
    136, 2,  33,  0,  0,   255, 200, // io expander on
    136, 2,  33,  1,  0,   255, 200, // io expander on
    136, 2,  32,  18, 240, 255, 200, // init
    136, 2,  32,  19, 15,  255, 200, // init ADC
    136, 2,  33,  18, 0,   255, 200, // init
    136, 2,  33,  19, 0,   255, 200, // init
    131, 8,  0,                      // ADC offset
    131, 6,  16,                     // binary offset
    131, 4,  36,                     // 300Ohm termination A
    131, 5,  36,                     // 300Ohm termination B
    131, 1,  0,                      // disable multiplexing
    136, 3,  96,  80, 136, 22,  0,   // channel 0 calibration
    136, 3,  96,  82, 136, 22,  0,   // channel 1 calibration
    136, 3,  96,  84, 136, 22,  0,   // channel 2 calibration
    136, 3,  96,  86, 136, 22,  0};  // channel 3 calibration

const uint8_t SAMPLE_SEQ[] = {100, 10};


int openOscilloscope(char * oscopeDevicePath) {
  int fd = open(oscopeDevicePath, O_SYNC | O_RDWR);
  if (fd < 0) {
    switch (errno) {
    case EACCES:
      fprintf(stderr, "bad access of $TTY\n");
      break;
    case ENOENT:
      fprintf(stderr, "$TTY does not exist\n");
      break;
    default:
      fprintf(stderr, "errno %d\n", errno);
      break;
    }
    exit(errno);
  }
  return fd;
}

int initOscilloscope(int fd) {
  for (long unsigned int ii = 0; ii < sizeof(INIT_SEQ); ii++) {
    int w = write(fd, &INIT_SEQ[ii], 1);
    assert(w == 1 && "failed to open $TTY");
    usleep(10000);
  }
  return 1;
}

int requestData(int inFD, int outFD) {
  int writeSample = write(inFD, SAMPLE_SEQ, sizeof(SAMPLE_SEQ));
  assert(writeSample == 2 && "failed to request sample");
  uint8_t read_buf[4000];
  int readSample = read(inFD, read_buf, sizeof(read_buf));
  assert(readSample == sizeof(read_buf));

  int w = write(outFD, read_buf, sizeof(read_buf));
  assert(w == sizeof(read_buf));
  return 1;
}


int main( int argc, char *argv[] )
{
  assert(argc == 4 && "usage: triggerData $oscope_device_path $output_fifo $script_path");

  FILE *fp;
  char cwd[MAX_BUF];
  char pathToScript[MAX_BUF];
  getcwd(cwd, MAX_BUF);
  sprintf(pathToScript, "%s/%s", cwd, argv[3]);

  //pathToScript = *argv[3];
  fp = popen(pathToScript, "r");
  if (fp == NULL) {
    printf("Failed to run command\n" );
    exit(1);
  }

  char * oscilloscopePath = argv[1];
  char * outpipePath = argv[2];

  int oscilloscopeFD = openOscilloscope(oscilloscopePath);
  initOscilloscope(oscilloscopeFD);

  int outputFD = open(outpipePath, O_RDONLY);

  char output[100];
  /* Read the output a line at a time - output it. */
  while (fgets(output, 1000, fp)) {
    // E.G. output[] = "BTN_3 1
    int button_state = (int)output[6] - 48;
    int button_num = (int)output[4] - 48;
    if (button_num == 1 && button_state == 0) {
      requestData(oscilloscopeFD, outputFD);
    }
  }
  /* close */
  pclose(fp);

  return 0;
}
