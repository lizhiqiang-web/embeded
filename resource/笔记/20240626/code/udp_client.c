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
    char recvbuf[1024] = {0};
    char sendbuf[1024] = {0};
    socklen_t addrlen = sizeof(my_addr);
    
    while(1)
    {
        printf("请输入你要发送的信息:");
        fflush(stdout);
        scanf("%s",sendbuf);
        int r = sendto(sockfd,sendbuf, strlen(sendbuf) + 1,0,(struct sockaddr *)&my_addr,addrlen);
        if(-1 == r)
        {
            perror("sendto error");
            break;
        }
        //如果发送的是quit  那么我自己也退出
        if(strcmp(sendbuf,"quit") == 0)
        {
            printf("发送的是quit，服务器会退出，自己也退出了\n");
            break;
        }

        //接收服务器发过来的消息
        r = recvfrom(sockfd,recvbuf,1023,0,NULL,NULL);
        if(-1 == r)
        {
            perror("recvfrom error");
            break;
        }
        printf("客户端收到的消息:%s\n",recvbuf);
        
    }
    //4关闭套接字
    close(sockfd);
    return 0;
}





