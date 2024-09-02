/*
    UDP服务器
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
    //2绑定地址
    struct sockaddr_in my_addr;
    my_addr.sin_family = AF_INET;
    my_addr.sin_port = htons(atoi(argv[2]));
    my_addr.sin_addr.s_addr = inet_addr(argv[1]);
    int r = bind(sockfd,(struct sockaddr *)&my_addr,sizeof(my_addr));
    if(-1 == r)
    {
        perror("bind error");
        return -3;
    }
    //3接收客户端的信息  my_addr保存对方的ip地址
    char recvbuf[1024] = {0};
    char sendbuf[1024] = {0};
    socklen_t addrlen = sizeof(my_addr);
    while(1)
    {
        r = recvfrom(sockfd,recvbuf,1023,0,(struct sockaddr *)&my_addr,&addrlen);
        if(-1 == r)
        {
            perror("recvfrom error");
            break;
        }
        printf("服务器收到的信息:%s\n",recvbuf);//将收到的信息打印出来
        //如果收到的是quit  那么服务器就退出
        if(strcmp(recvbuf,"quit") == 0)
        {
            printf("服务器退出\n");
            break;
        }
        //如果没有失败  那么my_addr玩意儿里面存放的就是对方的ip地址
        //我们就可以给这个ip地址发送信息了
        //将收到的信息放在"chengyuanNB:"东西的后面
        sprintf(sendbuf,"%s%s","chengyuanNB:",recvbuf);
        r = sendto(sockfd,sendbuf, strlen(sendbuf) + 1,0,(struct sockaddr *)&my_addr,addrlen);
        if(-1 == r)
        {
            perror("sendto error");
            break;
        }
    }
    //4关闭套接字
    close(sockfd);
    return 0;
}





