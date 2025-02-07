#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include "threadCheck.h"

typedef struct shareRes_s{
    pthread_mutex_t mutex;
    pthread_cond_t cond;
    int ticketNum;
}shareRes_t,*pshareRes_t;

void *sellTicket1(void *arg){
    sleep(1);
    pshareRes_t pshared = (pshareRes_t)arg;
    while(1){
        pthread_mutex_lock(&pshared->mutex);
        if(pshared->ticketNum > 0){
            printf("Before 1 sells tickets,ticketNum = %d\n",pshared->ticketNum);
            --pshared->ticketNum;
            if(pshared->ticketNum ==0){
                pthread_cond_signal(&pshared->cond);
            }
            usleep(500000);
            printf("After 1 sells tickets,ticketNum = %d\n",pshared->ticketNum);
            pthread_mutex_unlock(&pshared->mutex);
            usleep(200000);
        }
        else{
            pthread_mutex_unlock(&pshared->mutex);
            break;
        }
    }
    pthread_exit(NULL);
}

void *sellTicket2(void *arg){
    sleep(1);
    pshareRes_t pshared = (pshareRes_t)arg;
    while(1){
        pthread_mutex_lock(&pshared->mutex);
        if(pshared->ticketNum > 0){
            printf("Before 2 sells tickets,ticketNum = %d\n",pshared->ticketNum);
            --pshared->ticketNum;
            if(pshared->ticketNum == 0){
                pthread_cond_signal(&pshared->cond);
            }
            usleep(500000);
            printf("After 2 sells tickets,ticketNum = %d\n",pshared->ticketNum);
            pthread_mutex_unlock(&pshared->mutex);
            usleep(200000);
        }else{
            pthread_mutex_unlock(&pshared->mutex);
            break;
        }
    }
    pthread_exit(NULL);
}

void *setTicket(void *arg){
    pshareRes_t pshared = (pshareRes_t)arg;
    pthread_mutex_lock(&pshared->mutex);
    if(pshared->ticketNum > 0){
        printf("Set is waiting\n");
        int ret = pthread_cond_wait(&pshared->cond,&pshared->mutex);
    }
    printf("add tickets\n");
    pshared->ticketNum = 10;
    pthread_mutex_unlock(&pshared->mutex);
    pthread_exit(NULL);
}

int main(int argc,char* argv[])
{
    shareRes_t shared;
    shared.ticketNum = 20;
    int ret;
    ret = pthread_mutex_init(&shared.mutex,NULL);
    THREAD_ERR_CHECK(ret,"pthread_mutex_init");
    ret = pthread_cond_init(&shared.cond,NULL);
    THREAD_ERR_CHECK(ret,"pthread_cond_init");

    pthread_t tid1,tid2,tid3;
    pthread_create(&tid3,NULL,setTicket,(void*)&shared);
    pthread_create(&tid1,NULL,sellTicket1,(void*)&shared);
    pthread_create(&tid2,NULL,sellTicket2,(void*)&shared);
    pthread_join(tid1,NULL);
    pthread_join(tid2,NULL);
    pthread_join(tid3,NULL);

    pthread_cond_destroy(&shared.cond);
    pthread_mutex_destroy(&shared.mutex);
    return 0;
}


