#include <stdio.h>
#include "serial_init.h"
#include <unistd.h>


//全局串口1文件描述符
int Gy39_fd = -1;

//GY39初始化 接串口1
void Gy39Init(void)
{
    //初始化串口
    Gy39_fd = init_serial("/dev/ttySAC2",9600);

    //我们需要准备命令 发给这个串口1 你的串口1接的是GY39
    //那么这个命令就会发给GY39 它收到了就会响应
    //准备一个命令
    unsigned char gy39_cmd[3] = {0xa5,0x83,0x28};//这个命令只自动输出光照
    //将这个命令发给串口
    write(Gy39_fd,gy39_cmd,3);
    //接收9个字节
}

//GY39获取数据
void Gy39GetData(void)
{   

    unsigned char gy39_RecvData[24] = {0};
    int flag = 0;
    while(1)//接收数据
    {   
        int r = read(Gy39_fd,&gy39_RecvData[flag],1);
        if(r == 0)
        {
            continue;
        }
        flag++;
        //printf("%d %d %x\n",r,flag,gy39_RecvData[flag-1]);
        
        switch(flag)
        {
            case 1://获取了第一个字节  如果不是0x5a 就是错的
                if(gy39_RecvData[0] != 0x5a)
                {                 
                    flag = 0;//重来
                }
                break;
            case 2:
                if(gy39_RecvData[1] != 0x5a)
                {                 
                    flag = 0;//重来
                }
                break;
            case 3:
                if(gy39_RecvData[2] != 0x15)
                {                 
                    flag = 0;//重来
                }
                break;
                
            //后面需要继续判断  我只写了几个特征
            case 24://接收完了
                flag = 0;
                #if 0
                for(int i = 0;i < 24;i++)
                {
                    printf("0x%x ",gy39_RecvData[i]);
                }
                printf("\n");
                #endif
                unsigned char luxcheck = 0;

                for(int i = 0 ;i < 8;i++)//算光照强度的校验
                {
                    luxcheck += gy39_RecvData[i];
                }
                int Lux = -1;
                if(luxcheck == gy39_RecvData[8])//这才是校验成功
                {
                    Lux = (gy39_RecvData[4]<<24 | gy39_RecvData[5]<<16 | gy39_RecvData[6]<<8 | gy39_RecvData[7]) / 100;
                }
                else
                {
                    printf("Lux check error\n");
                }
                //将我们需要的光照强度给算出来
                int T = -1,P = -1,Hum = -1,H = -1;   
                unsigned char TPHumHcheck = 0;
                for(int i = 9 ;i < 23;i++)//算光照强度的校验
                {
                    TPHumHcheck += gy39_RecvData[i];
                }
                if(TPHumHcheck == gy39_RecvData[23])
                {
                    T = (gy39_RecvData[13]<<8 | gy39_RecvData[14]) / 100;
                    P = (gy39_RecvData[15]<<24 | gy39_RecvData[16]<<16 | gy39_RecvData[17]<<8 | gy39_RecvData[18]) / 100;
                    Hum = (gy39_RecvData[19]<<8 | gy39_RecvData[20]) / 100;
                    H = gy39_RecvData[21]<<8 | gy39_RecvData[22];
                }
                else
                {
                    printf("T/P/Hum/H check error\n");
                }
                printf("Lux:%d(lux) T:%d(℃) P:%d(pa) Hum=%d(%%) H:%d(m)\n",Lux,T,P,Hum,H);           
            default:
                break;
        }
        
        
        //我们将这些数据打印出来

    }


}




int main()
{
    Gy39Init();
    Gy39GetData();
    

    return 0;
}


