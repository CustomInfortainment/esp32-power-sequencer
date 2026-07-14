#ifndef SNIFFING_H
#define SNIFFING_H

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <termios.h>

#include "global.h"
#include "profiling.h"

#define SERIAL_PORT_PATH "/dev/cu.usbmodem2056369243431"

//링버퍼 구현부, sniffing.c에 선언되어 있음
extern RingBuffer* canframeRingBuf;

extern const char MODULE_PORT_PATH;

void sniff_init();
void sniff_data_recv();

void sniff_stop();

#endif