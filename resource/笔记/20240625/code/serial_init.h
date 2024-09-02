#ifndef __SERIAL_INIT__
#define __SERIAL_INIT__

#include <termios.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <stdio.h>


//初始化串口,把 open 和 初始化 结合在一起了
//baudrate:波特率  为了串口通信的同步的
//我们初始化的时候应该主动去匹配模块的波特率，这三个模块默认波特率都是9600
int init_serial(const char *file, int baudrate);


#endif