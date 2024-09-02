#ifndef __BMP_H__
#define __BMP_H__



//初始化lcd
void LcdInit(void);
//画点函数  你需要给我x y 坐标  还需要一个color才可以画出这个点
void Display(int x,int y,unsigned int color);
//画一个矩形
void DisplayJuxing(int w,int h,int x0,int y0,unsigned int color);
//销毁屏幕
void LcdDestory(void);

//处理bmp图片  将图片文件的路径名给传进来
void DisplayBmpPicture(const char * bmppicture);








#endif


