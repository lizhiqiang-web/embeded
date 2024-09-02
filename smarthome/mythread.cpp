#include "mythread.h"

MyThread::MyThread()
{

}
void MyThread::run()
{
    int n=0;
    char ID[13];
    int fd=init_serial("/dev/ttySAC1",9600);
    while(1)
    {

        int ret = read(fd,ID,13);

        if(ret==-1)
         {

            perror("read faulse");

         }

         if(ret!=0)
         {      n++;
                for(int i=0;i<13;i++)
                {
                    printf("%c",ID[i]);
                }
                printf("\n%d\n",n);
         }
         if(n==2)
         {
             n=0;
             QString str_id;
             for(int i=1;i<11;i++)
             {
                 str_id+=ID[i];
             }

             emit IdSignal(str_id);

         }
     }

}


