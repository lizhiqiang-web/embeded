#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

//内卷
int main()
{
	//打开驱动
	int fd_led = open("/dev/led_drv",O_RDWR);
	if(-1 == fd_led)
	{
		perror("open led_drv error");
		return -1;
	}
	//打开驱动
	int fd_beep = open("/dev/pwm",O_RDWR);
	if(-1 == fd_beep)
	{
		perror("open pwm error");
		return -1;
	}
	//操作 往这个驱动里面写入就可以了
	char buf_led[2];
	char buf_beep[1];
	while(1)
	{
		buf_beep[0] = 1;
		buf_led[0] = 1;
		buf_led[1] = 8;//第一个灯
		write(fd_beep,buf_beep,1);//蜂鸣器就响了
		write(fd_led,buf_led,2);
		buf_led[1] = 9;
		write(fd_led,buf_led,2);
		buf_led[1] = 10;
		write(fd_led,buf_led,2);//三个灯就亮了
		
		sleep(1);
		
		buf_beep[0] = 0;
		buf_led[0] = 0;
		buf_led[1] = 8;//第一个灯
		write(fd_beep,buf_beep,1);//蜂鸣器就不响了
		write(fd_led,buf_led,2);
		buf_led[1] = 9;
		write(fd_led,buf_led,2);
		buf_led[1] = 10;
		write(fd_led,buf_led,2);//三个灯就不亮了
				
		sleep(1);
		
	}
	
	close(fd_led);
	close(fd_beep);
	return 0;
}










