#ifndef YW_THREAD_H
#define YW_THREAD_H
#include <stdio.h>
#include "serial_init.h"
#include <unistd.h>
#include <QThread>

class gy_thread:public QThread
{
    Q_OBJECT
public:
    gy_thread();
    int Gy39_fd = -1;

    //GY39初始化 接串口1
    void Gy39Init(void);
signals:
void send_gy(QString str1,QString str2,QString str3);

protected:
    void run() override;
};

#endif // YW_THREAD_H
