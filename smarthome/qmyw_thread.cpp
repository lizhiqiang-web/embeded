#include "qmyw_thread.h"

qmyw_thread::qmyw_thread()
{

}

void qmyw_thread::run()//检测烟雾的线程
{
    int timeout1=0;//超时计数
    int timeout2=0;//超时计数

    char cmd[]={0xff,0x01,0x86,0x00,0x00,0x00,0x00,0x00,0x79};//烟雾传感器输入命令
    char zdata[9]={0};
    int z_mq_fd = init_serial("/dev/ttySAC3",9600);//打开串口读取数据
        while (1)
        {
            shuzhi = 0;
            usleep(500000);
            write(z_mq_fd,cmd,9);
            usleep(20000);
            while((read(z_mq_fd,zdata,9)!=9)&&(timeout2<1000))//不断读取数据
            {
                timeout2++;
            }
            if(timeout2>=1000)//超时计数1
            {
                timeout2=0;
                timeout1++;
                if(timeout1>15)//超时计数2
                {
                    perror("dev problem\n");
                    break;
                }
                perror("read time out\n");
                continue;
            }
            if((zdata[0]==0xff)&&(zdata[1]==0x86))//校验数据计数数值
            {
                shuzhi = zdata[2]<<8|zdata[3];
                printf("mq=%d\n",shuzhi);
                emit send_yw(shuzhi);

            }
        }
}
