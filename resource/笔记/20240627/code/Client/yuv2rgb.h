#ifndef YUV2RGB_H
#define YUV2RGB_H

/**
* @brief：YUYV422格式转换成 RGB888格式 
* @param  width:	YUV图像的宽
* @param  height:	YUV图像的高
* @param  src:		YUV图像的首地址 
* @param  dst:		RGB图像的存储地址 
* @return void：	无
 **/
void YUV422toRGB888(int width, int height, unsigned char *src, unsigned char *dst);



#endif











