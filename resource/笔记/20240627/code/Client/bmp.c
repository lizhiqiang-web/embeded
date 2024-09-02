#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <sys/mman.h>

int fd_lcd = -1;//全局的lcd文件描述符
unsigned int * plcd = NULL;//全局lcd映射首地址





//初始化lcd
void LcdInit(void)
{
	//1 打开lcd
	fd_lcd = open("/dev/fb0",O_RDWR);
	if(-1 == fd_lcd)
	{
		perror("open fb0 error");
		exit(1);//退出程序
	}
	//将lcd映射到内存
	plcd = mmap(NULL,800*480*4,PROT_READ | PROT_WRITE,MAP_SHARED,fd_lcd,0);
	if(MAP_FAILED == plcd)
	{
		perror("mmap fb0 error");
		exit(2);//退出程序
	}
}

//画点函数  你需要给我x y 坐标  还需要一个color才可以画出这个点
void Display(int x,int y,unsigned int color)
{
	if(x >= 0 && x < 800 && y >= 0 && y < 480)
	{
		*(plcd + y * 800 + x) = color;
	}
	
}

//画一个矩形
void DisplayJuxing(int w,int h,int x0,int y0,unsigned int color)
{
	for(int y = 0;y < h;y++)//面
	{
		for(int x = 0;x < w;x++)//线
		{
			Display(x + x0,y + y0,color);//点
		}
	}
}




//销毁屏幕
void LcdDestory(void)
{
	munmap(plcd,800*480*4);
	close(fd_lcd);
}


//处理bmp图片  将图片文件的路径名给传进来
void DisplayBmpPicture(const char * bmppicture)
{
	//1 打开这个图片文件
	int fd = open(bmppicture,O_RDONLY);
	if(-1 == fd)
	{
		perror("bmppicture open error");
		return;
	}
	//2 处理图片文件
	int width = 0,height = 0,depth = 0;
	//读取图片信息
	lseek(fd,0x12,SEEK_SET);
	read(fd,&width,4);
	read(fd,&height,4);
	lseek(fd,0x1c,SEEK_SET);
	read(fd,&depth,2);
	
	//printf("%d  %d  %d\n",width,height,depth);
	//像素数组的大小 w * h * (d / 8)
	//每一行的末尾补充的字节数
	int n = (4 - width * depth / 8 % 4) % 4;

	//unsigned char colorbuf[(width * depth / 8 + n) * height]
	
	unsigned char * colorbuf = malloc((width * depth / 8 + n) * height);
	//读取像素数组
	lseek(fd,0x36,SEEK_SET);
	read(fd,colorbuf,(width * depth / 8 + n) * height);
	unsigned int color;
	unsigned char c_a = 0,c_r,c_g,c_b;
	int i = 0;//跳colorbuf数组的下标
	
	for(int y = height - 1;y >= 0;y--)//从下到上
	{
		for(int x = 0;x < width;x++)//从左往右
		{
			c_b = colorbuf[i++];
			c_g = colorbuf[i++];
			c_r = colorbuf[i++];		
			if(depth == 32)			
			{
				c_a = colorbuf[i++];
			}
			color = c_a<<24 | c_r<<16 | c_g<<8 | c_b;
			Display(x,y,color);
		}
		//跳过无用的字节
		i += n;
	}
	
	
	
	
	free(colorbuf);
	//3 关闭文件
	close(fd);
}












