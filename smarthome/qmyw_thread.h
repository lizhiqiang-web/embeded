#ifndef QMYW_THREAD_H
#define QMYW_THREAD_H
extern "C"{
#include <stdio.h>

#include "serial_init.h"

#include <unistd.h>
}
#include <QThread>

class qmyw_thread:public QThread
{
    Q_OBJECT
public:
    int shuzhi=0;
    qmyw_thread();
signals:
void send_yw(int num);
protected:
    void run() override;
};


#endif // QMYW_THREAD_H
