#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <sys/ustat.h>
#include <unistd.h>

#define ARGS_CHECK(argc,num) {if(argc != num){fprintf(stderr,"Args error : The arguments do not meet the requirements!\n");return -1;}}

#define ERR_CHECK(ret,val,msg){if(ret == val){perror(msg);exit(-1);}}

void * threadFunc(void *arg){
    printf("This is a new thread ,tid = %lu\n",pthread_self());
    return NULL;
}

void test0()
{

}

int main(int argc,char* argv[])
{
    printf("pid = %d,tid = %lu\n",getpid(),pthread_self());
    pthread_t tid;
    int ret = pthread_create(&tid,NULL,threadFunc,NULL);
    sleep(1);
    printf("tid = %lu\n",tid);
    test0();
    return 0;
}


