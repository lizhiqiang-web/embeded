#ifndef MYTHREAD_H
#define MYTHREAD_H
#include <QThread>
extern "C"{
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <termios.h>
#include <stdlib.h>
#include <netdb.h>
#include "serial_init.h"
}

class MyThread :public QThread
{
    Q_OBJECT
public:
    MyThread();
signals:
    void IdSignal(QString str);


protected:
    void run() override;

};

#endif // MYTHREAD_H
