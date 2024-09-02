#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

//内卷
int main()
{
	//打开驱动
	int fd = open("/dev/pwm",O_RDWR);
	if(-1 == fd)
	{
		perror("open led_drv error");
		return -1;
	}
	
	//操作 往这个驱动里面写入就可以了
	char buf[1];
	while(1)
	{
		buf[0] = 1;//响
		write(fd,buf,1);
		
		sleep(1);//秒级延时  usleep();//微秒级延时 
		
		buf[0] = 0;//不响
		write(fd,buf,1);
		sleep(1);
	}
	
	close(fd);
	return 0;
}




