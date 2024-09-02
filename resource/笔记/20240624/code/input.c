#include <stdio.h>
#include <linux/input.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>

typedef enum//方向枚举
{
    DIR_ERROR,//错误的值
    DIR_TOUCH,//点击
    DIR_LEFT,//左
    DIR_RIGHT,//右
    DIR_UP,//上
    DIR_DOWN//下
}TouchDirection;



TouchDirection Touch_Dir = DIR_ERROR;//全局的滑动方向


//全局的触摸屏文件描述符
int Touch_Fd = -1;

int music_i = 1;








//触摸屏的初始化
void TouchInit(void)
{
    //1 打开这个触摸屏
    Touch_Fd = open("/dev/input/event0",O_RDWR);
    if(-1 == Touch_Fd)
    {
        perror("open event0 error");
        exit(1);
    }
}


//我要播放音乐   这里要用线程去播放
void * PlayMusic(void * arg)
{
    srand((unsigned int)time(NULL));//利用系统时间产生一组随机数
    char buf[128] = {0};
    while(1)
    {
        if(Touch_Dir == DIR_UP)
        {
            music_i++;
            Touch_Dir = DIR_ERROR;
        }
        else if(Touch_Dir == DIR_DOWN)
        {
            music_i--;
            Touch_Dir = DIR_ERROR;
        }
        else//随机
        {
            music_i = rand() % 3 + 1;//这里就是1 ~ 3中间的一个
        }
        if(music_i >= 3)
            music_i = 3;
        if(music_i <= 1)
            music_i = 1;//我只有三首歌   在范围之内才能播放

        sprintf(buf,"madplay -Q %d.mp3",music_i);
        printf("现在播放的是 %d.mp3\n",music_i);
        system(buf);
        sleep(1);

    }
}




//触摸屏的操作
void Touch()
{
    //维护的结构体
    struct input_event ev;
    //触摸屏操作应该是一个死循环
    int flag = 0;
    int ev_x0,ev_y0,ev_x,ev_y;
    while(1)
    {
        //从触摸屏里面获取数据
        int r = read(Touch_Fd,&ev,sizeof(ev));//Touch_Fd是阻塞的  如果没有数据这个read就会卡死
        if(r != sizeof(ev))//你读错了
        {
            //你需要回去重新
            flag++;
            //连续错误n次 还是错的  这个玩意儿真的是坏的
            if(flag == 10)
            {
                perror("这个触摸屏真的烂了，请检查或者重启系统尝试\n");
                exit(2);
            }
            continue;//没有达到阈值  就继续尝试
        }
        flag = 0;
        //能到这里就表明是正确读取 我们就可以分析数据了
        //printf("type:%d  code:%d  value:%d\n",ev.type,ev.code,ev.value);
        //先获取坐标
        if(ev.type == EV_ABS)//现在获取坐标
        {
            if(ev.code == ABS_X)//x轴
            {
                ev_x = ev.value;
            }
            else if(ev.code == ABS_Y)//y轴
            {
                ev_y = ev.value;
            }
        }
        if(ev.type == 0x01 && ev.code == BTN_TOUCH && ev.value == 0x01)//手是按下去的时候
        {
            //手按下去的时候前面获取到的那组坐标就是起始坐标
            ev_x0 = ev_x;
            ev_y0 = ev_y;
        }
        else if(ev.type == 0x01 && ev.code == BTN_TOUCH && ev.value == 0x00)//手抬起来了
        {
            //手抬起来你才有方向
            if(ev_x0 == ev_x && ev_y0 == ev_y)
            {
                printf("点击\n");//什么时候是长按就在这里
                Touch_Dir = DIR_TOUCH;
            }
            else//滑动了
            {
                if(ev_x >= ev_x0 && abs(ev_x - ev_x0) >= abs(ev_y - ev_y0))
                {
                    printf("right\n");
                    Touch_Dir = DIR_RIGHT;
                }
                else if(ev_x <= ev_x0 && abs(ev_x - ev_x0) >= abs(ev_y - ev_y0))
                {
                    printf("left\n");
                    Touch_Dir = DIR_LEFT;
                }
                else if(ev_y <= ev_y0 && abs(ev_x - ev_x0) <= abs(ev_y - ev_y0))
                {
                    printf("up\n");
                    Touch_Dir = DIR_UP;
                    //上下滑动的时候就讲madplay杀死
                    system("killall -9 madplay");
                }
                else
                {
                    printf("down\n");
                    Touch_Dir = DIR_DOWN;
                    system("killall -9 madplay");
                }
            }
        }



    }


}





int main()
{
    //开辟一个线程 去跑音乐的代码
    pthread_t thread;
    if(pthread_create(&thread,NULL,PlayMusic,NULL) != 0)
    {
        perror("create playmusic thread error");
        exit(3);
    }
    else
    {
        //将这个线程分离
        pthread_detach(thread);
    }


    TouchInit();
    Touch();


    return 0;
}




