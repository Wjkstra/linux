/*  TCP通信模型
 *
 *  客户端              服务端
 *  socket              socket
 *    |                    |
 *    |                  bind
 *    |                    |
 *    |                 listen
 *    |                    |
 *  connect----------------|
 *    |                 accept
 *    |                    |
 *  send------------------recv
 *    |                    |
 *  recv------------------send
 *    |                    |
 *  close               close
 *
 * */

/*
 * 客户端设计
 * 功能：
 *      1、接收数据
 *      2、发送数据
 *      3、断开连接
 * */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/select.h>
#include <sys/time.h>
#include <memory.h>
#include <string.h>


// ./server ip port


int main(int argc,char* argv[]){
    if(argc != 3){
        printf("Please input arguments!\n");
        exit(0);
    }

    int sockfd = socket(AF_INET,SOCK_STREAM,0);
    struct sockaddr_in serverAddr;

    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(atoi(argv[2]));
    serverAddr.sin_addr.s_addr = inet_addr(argv[1]);

    int ret = connect(sockfd,(struct sockaddr*)&serverAddr,sizeof(serverAddr));

    fd_set rdset;

    char buf[1024];
    while(1){
        FD_ZERO(&rdset);
        FD_SET(STDIN_FILENO,&rdset);
        FD_SET(sockfd,&rdset);
        select(sockfd + 1,&rdset,NULL,NULL,NULL);
        if(FD_ISSET(STDIN_FILENO,&rdset)){
            memset(buf,0,sizeof(buf));
            read(STDIN_FILENO,buf,sizeof(buf));
            /* cat(buf,buf,(char*)&serverAddr.sin_port); */
            send(sockfd,buf,strlen(buf),0);
        }
        if(FD_ISSET(sockfd,&rdset)){
            memset(buf,0,sizeof(buf));
            ssize_t sret = recv(sockfd,buf,sizeof(buf),0);
            if(sret == 0){
                printf("connection timeout .......\n");
                break;
            }
            printf("%s\n",buf);
        }
    }

}
