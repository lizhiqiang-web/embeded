#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

//内卷
int main()
{
	//打开驱动
	int fd = open("/dev/led_drv",O_RDWR);
	if(-1 == fd)
	{
		perror("open led_drv error");
		return -1;
	}
	
	//操作 往这个驱动里面写入就可以了
	char buf[2];
	buf[0] = 0;//亮灯
	buf[1] = 10;//D8
	write(fd,buf,2);
		/*buf[1] = 9;//D8
		write(fd,buf,2);
		buf[1] = 10;//D8
		write(fd,buf,2);
		sleep(1);*/
	close(fd);
	return 0;
}




