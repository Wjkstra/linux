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
 * 服务端设计
 * 功能：
 *      1、建立连接
 *      2、转发数据
 *      3、关闭指定客户端连接
 * */

// ./server ip port

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/select.h>
#include <sys/time.h>
#include <memory.h>
#include <time.h>

typedef struct Conn_s{
    int isConnected;
    int netfd;
    time_t lastActive;
}Conn_s;

void cat(char* buf,int idx);

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

    int reuse = 1;//允许重用
    int ret = setsockopt(sockfd,SOL_SOCKET,SO_REUSEADDR,&reuse,sizeof(reuse));

    ret = bind(sockfd,(struct sockaddr*)&serverAddr,sizeof(serverAddr));
    listen(sockfd,50);

    fd_set monitorSet;//监听集合
    fd_set readySet;//就绪集合
    FD_ZERO(&monitorSet);
    FD_SET(sockfd,&monitorSet);

    Conn_s client[1024];
    for(int i = 0;i < 1024;++i){
        client[i].isConnected = 0;
    }

    int curidx = 0;

    int fdToiindx[1024];
    for(int i = 0;i < 1024;++i){
        fdToiindx[i] = -1;
    }

    time_t now;

    char buf[4096] = {0};

    while(1){
        memcpy(&readySet,&monitorSet,sizeof(fd_set));
        struct timeval timeout;
        timeout.tv_sec = 1 ;
        timeout.tv_usec = 0;
        select(1024,&readySet,NULL,NULL,&timeout);
        now = time(NULL);
        /* printf("now : %s\n",ctime(&now)); */
        //有新客户端建立连接，将信息插入到client
        if(FD_ISSET(sockfd,&readySet)){
            client[curidx].isConnected = 1;
            client[curidx].netfd = accept(sockfd,NULL,NULL);
            client[curidx].lastActive = time(NULL);
                printf("\nnow : %s\n",ctime(&now));
            printf("%d is connected, netfd : %d, %s\n",client[curidx].netfd,ctime(&client[curidx].lastActive));
            fdToiindx[client[curidx].netfd] = curidx;
            FD_SET(client[curidx].netfd,&monitorSet);
            ++curidx;
        }
        for(int i = 0;i < curidx;++i){
            if(client[i].isConnected == 1 && FD_ISSET(client[i].netfd,&readySet)){
                memset(buf,0,sizeof(buf));
                ssize_t sret = recv(client[i].netfd,buf,sizeof(buf),0);
                if(sret == 0){//客户端断开连接
                    FD_CLR(client[i].netfd,&monitorSet);
                    close(client[i].netfd);
                    fdToiindx[client[i].netfd] = -1;
                    client[i].isConnected = 0;
                    printf("\nnow : %s\n",ctime(&now));
                    printf("%d is disabled\n",client[i].netfd);
                    continue;
                }
                client[i].lastActive = time(NULL);
                printf("%d new message, netfd : %d, %s\n",client[i].netfd,ctime(&client[i].lastActive));

                for(int j = 0;j < curidx;++j){
                    if(client[j].isConnected == 1 && j != i){
                        cat(buf,i);
                        send (client[j].netfd,buf,strlen(buf),0);
                    }
                }
            }
        }
        for(int i = 0;i < curidx;++i){
            if(client[i].isConnected == 1 && now - client[i].lastActive > 20){
                FD_CLR(client[i].netfd,&monitorSet);
                close(client[i].netfd);
                fdToiindx[client[i].netfd] = -1;
                client[i].isConnected = 0;
            }
        }
    }
}

void cat (char* buf,int idx){
    char num[20];
    sprintf(num,"%d:",idx);
    int len = strlen(num) + strlen(buf);
    char tmp[len + 1];
    strcpy(tmp,num);
    strcat(tmp,buf);
    strcpy(buf,tmp);
}
