#include <stdio.h>
#include <linux/videodev2.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include "yuv2rgb.h"

#define WIDTH 	640
#define HEIGHT 	480

int fd;//video fd 
int *pfb = NULL;//lcd addr
int terminate = 1;//结束标志

typedef struct UserBuf{
	void *start;
	int length;
}UserBuf;
UserBuf *usrbuf = NULL;	//用户结构，用于保存映射得来的 缓冲地址和长度
unsigned char rgb[WIDTH*HEIGHT*3];	//RGB数据缓冲区，用于保存YUV转成RGB后的数据

//屏幕初始化
void lcd_init();

//屏幕显示视频图像
void lcd_show_image();
inline void lcd_draw_point(int x,int y,int color);
void lcd_video_close();


//处理图像
void process_image(UserBuf ubuf);
//视频初始化：设备名通过命令行参数传递 如/dev/video7
void video_init(const char *devName);
//错误处理
void error_handle(const char *errmsg);
//循环获取视频帧图像
void get_frame();
//信号处理函数
void sig_handle(int signo);

/*******************主函数********************/
void *  V4L2Contorl(void * arg)
{
	
	//signal(SIGINT,sig_handle);
    //1.视频初始化 
    video_init("/dev/video7");
	//2.LCD初始化
	lcd_init();
	
	//3.获取图像并处理
	get_frame();
    
    //4.资源回收
	lcd_video_close();
    return NULL;
	
}
/*******************end 主函数 end********************/

//错误处理
void error_handle(const char *errmsg)
{
	perror(errmsg);
	exit(EXIT_FAILURE);
}

//视频初始化
void video_init(const char *devName)
{
	//1.打开设备
    fd = open(devName,O_RDWR);
    if(fd == -1)
		error_handle("open video dev error");
	
    printf("open dev success\n");
	
	//2.查询视频设备所支持的格式
    struct v4l2_fmtdesc fmt;
    fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    fmt.index =  0;
    int ret;
	while(1)
	{
		ret = ioctl(fd,VIDIOC_ENUM_FMT,&fmt);
		if(ret != 0)
			break;
		printf("format:%s\n",fmt.description);
		fmt.index++;
	}
	
	//3.设置视频格式
    struct v4l2_format s_fmt;
    s_fmt.fmt.pix.width = WIDTH;
    s_fmt.fmt.pix.height = HEIGHT;
    s_fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_YUYV;
    s_fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	printf("pix.width=%d\n",s_fmt.fmt.pix.width);
    ret = ioctl(fd,VIDIOC_S_FMT,&s_fmt);
    if(ret != 0)
		error_handle("set format error");
	
	printf("pix.width=%d\n",s_fmt.fmt.pix.width);



	//4.申请内核态缓冲
    struct v4l2_requestbuffers reqbuf;
    reqbuf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    reqbuf.count = 5;
    reqbuf.memory = V4L2_MEMORY_MMAP;

    ret = ioctl(fd,VIDIOC_REQBUFS,&reqbuf);
    if(ret != 0)
		error_handle("request buf error");
	
    printf("request buf success\n");
	
	//分配用户结构，用于保存映射得来的 缓冲地址和长度
	usrbuf = malloc(sizeof(UserBuf)*5);
	memset(usrbuf,0,sizeof(UserBuf)*5);
	
	//5.查询内核缓冲、映射到用户空间
	int i;
	for(i = 0; i < 5; i++)
	{
		struct v4l2_buffer buf;
		buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
		buf.memory = V4L2_MEMORY_MMAP;
		buf.index = i;

		ret = ioctl(fd,VIDIOC_QUERYBUF,&buf);
		if(ret != 0)
			error_handle("query buf error");
		
		//把内核空间分配好的缓冲映射到用户空间 
		usrbuf[i].length = buf.length;
		usrbuf[i].start = mmap(0,buf.length,
						PROT_READ | PROT_WRITE,
						MAP_SHARED,fd,buf.m.offset);
		if(usrbuf[i].start == MAP_FAILED)
			error_handle("map error");
		else
			printf("start = %p, len=%d\n",usrbuf[i].start,usrbuf[i].length);
	}
	
	//6.添加到采集队列
	for(i = 0; i < 5; i++)
	{
		struct v4l2_buffer buf;
		buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
		buf.memory = V4L2_MEMORY_MMAP;
		buf.index = i;
		
		ret = ioctl(fd,VIDIOC_QBUF,&buf);
		if(ret != 0)
			error_handle("add queue error");
	}
	
    //7.启动视频采集
    enum v4l2_buf_type type;
    type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    ret = ioctl(fd,VIDIOC_STREAMON,&type);
    if(ret != 0)
		error_handle("start error");
	
    printf("video init success\n\n");    
}

//循环获取视频帧图像
void get_frame()
{
	while(!terminate)
	{
		int ret;
		//8.从队列中取出缓冲
		struct v4l2_buffer buf;
		buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
		buf.memory = V4L2_MEMORY_MMAP;
		ret = ioctl(fd,VIDIOC_DQBUF,&buf);
		if(ret != 0)
			error_handle("dequeue error");
		
		//9.处理图像
		process_image(usrbuf[buf.index]);//采集到的数据是YUYV格式
										//我们需要将这个格式转换为rgb
			
		//10.把使用完的缓冲区再放回采集列
		ret = ioctl(fd,VIDIOC_QBUF,&buf);
		if(ret != 0)
			error_handle("dequeue error");
	}
}
//处理图像
void process_image(UserBuf ubuf)
{
	//这个函数在libyuv2rgb.so库里面 我们编译的时候需要加上这个库
	//arm-linux-gcc v_local.c -L./ -lyuv2rgb
	//直接将可执行代码放在开发板上面去执行的时候会提示找不到这个库
	//将libyuv2rgb.so 放在开发板的 /lib文件夹里面去就可以了
	YUV422toRGB888(WIDTH,HEIGHT,ubuf.start,rgb);//先转换  再显示
	lcd_show_image();//显示rgb
}

//屏幕初始化
void lcd_init()
{
	//打开屏幕设备
	int fd = open("/dev/fb0", O_RDWR);
	if (fd < 0)
	{
		error_handle("open fb0 error");
		exit(1);
	}
	
	//映射
	pfb = mmap(NULL, 800*480*4, PROT_WRITE|PROT_READ,MAP_SHARED, fd, 0);
	if(pfb == MAP_FAILED)
	{
		error_handle("map error");
		exit(2);
	}
	
}

//画点
void lcd_draw_point(int x,int y,int color)
{
	if(x < 800 && x >= 0 && y < 480 && 7 >= 0)
		*(pfb + 800*y + x) = color;
}
//屏幕显示视频图像
void lcd_show_image()
{
	unsigned char *p = rgb;
	int i,j;
	unsigned char r;
	unsigned char g;
	unsigned char b;
	unsigned int color;
	#if 1
	for(i = 0; i < HEIGHT; i++)
	{
		for(j = 0; j < WIDTH; j++)
		{
			b = *p++;
			g = *p++;
			r = *p++;
			color = (r << 16) | (g << 8) | b;
			lcd_draw_point(j,i,color);
		}
	}
	#endif	
}

//.资源回收
void lcd_video_close()
{
	int i;
	for(i = 0; i < 5; i++)
	{
		munmap(usrbuf[i].start,usrbuf[i].length);
	}
	munmap(pfb,800*480*4);
	free(usrbuf);
	close(fd);
	printf("************game over!************\n");
}



