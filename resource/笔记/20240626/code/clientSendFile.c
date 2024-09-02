/*
    UDP客户端
*/
#include <stdio.h>
#include <sys/types.h>          /* See NOTES */
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>

//main函数传参  第一个参数为ip地址  第二个为端口
//./a.out 192.168.100.249 6789
int main(int argc,char *argv[])
{
    if(argc < 3)
    {
        printf("参数数量不够，例子：./a.out 192.168.100.249 6789\n");
        return -1;
    }
    //1创建套接字 
    int sockfd = socket(AF_INET,SOCK_DGRAM,0);
    if(-1 == sockfd)
    {
        perror("socket error");
        return -2;
    }
    //2准备服务器地址
    struct sockaddr_in my_addr;
    my_addr.sin_family = AF_INET;
    my_addr.sin_port = htons(atoi(argv[2]));
    my_addr.sin_addr.s_addr = inet_addr(argv[1]);
    
    //3给服务器发信息
    
    unsigned char sendbuf[1024] = {0};
    socklen_t addrlen = sizeof(my_addr);
    
	
	while(1)
	{
		//先录音
		printf("按回车键录音.......\n");
		getchar();
		printf("录音4秒\n");
		system("arecord -d4 -c1 -r16000 -traw -fS16_LE hehe.pcm");
		//打开音频文件
		int fd = open("hehe.pcm",O_RDWR);
		
		int filesize = lseek(fd,0X00,SEEK_END);
		lseek(fd,0X00,SEEK_SET);
		
		
		//发送文件大小给服务器
		sendto(sockfd,&filesize,4,0,(struct sockaddr *)&my_addr,addrlen);
		
		//接收服务器发过来的next
		recvfrom(sockfd,sendbuf,1024,0,NULL,NULL);
		if(strcmp(sendbuf,"next!!!"))
		{
			printf("服务器不知道发过来一个什么东西\n");
			return -4;
		}
		
		
		while(1)
		{
			//读取文件内容  将内容发送给服务器
			int r = read(fd,sendbuf,1024);
			if(r == 0)
			{
				//读完了  退出
				printf("发送文件完毕\n");
				break;
			}
			//将读取的文件内容发送给服务器
			r = sendto(sockfd,sendbuf,r,0,(struct sockaddr *)&my_addr,addrlen);
			if(-1 == r)
			{
				perror("sendto error");
				break;
			}

		}
		
		//这里就可以接收识别的信息

		
	}
    //4关闭套接字
    close(sockfd);
    return 0;
}





