#include <stdio.h>
#include "serial_init.h"
#include <unistd.h>

void z_mq_getdata()
{   
    int timeout1=0;
    int timeout2=0;
    int shuzhi=0;
    char cmd[]={0xff,0x01,0x86,0x00,0x00,0x00,0x00,0x00,0x79};
    char zdata[9]={0};
    int z_mq_fd = init_serial("/dev/ttySAC3",9600);
    while (1)
    {   
        shuzhi = 0;
        usleep(500000);
        write(z_mq_fd,cmd,9);
        usleep(20000);
        while((read(z_mq_fd,zdata,9)!=9)&&(timeout2<1000))
        {
            timeout2++;
        }
        if(timeout2>=1000)
        {   
            timeout2=0;
            timeout1++;
            if(timeout1>15)
            {   
                perror("dev problem\n");
                break;
            }
            perror("read time out\n");
            continue;
        }

        if(zdata[0]==0xff&&zdata[1]==0x86)
        {
            shuzhi = zdata[2]<<8|zdata[3];
            printf("mq=%d\n",shuzhi);
        }
    }
}

int main(int argc,char *argv[])
{

    z_mq_getdata();

    return 0;
}