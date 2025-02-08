#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>

typedef struct shareRes_s {
    int flag;
    pthread_mutex_t mutex;
    pthread_cond_t cond;
} shareRes_t;

void * customer(void *arg) {
    sleep(1);
    shareRes_t * pshareRes = (shareRes_t *)arg;
    pthread_mutex_lock(&pshareRes->mutex);
    while (pshareRes->flag != 1) {
        printf("1 is not ready!\n");
        pthread_cond_wait(&pshareRes->cond, &pshareRes->mutex);
    }

    pshareRes->flag = 0;
    printf("I got 1!\n");
    pthread_mutex_unlock(&pshareRes->mutex);
    pthread_exit(NULL);
}

void * customer1(void *arg) {
    sleep(2);
    shareRes_t * pshareRes = (shareRes_t *)arg;
    pthread_mutex_lock(&pshareRes->mutex);
    while (pshareRes->flag != 2) {
        printf("2 is not ready!\n");
        pthread_cond_wait(&pshareRes->cond, &pshareRes->mutex);
    }

    pshareRes->flag = 0;
    printf("I got 2!\n");
    pthread_mutex_unlock(&pshareRes->mutex);
    pthread_exit(NULL);
}

void * customer2(void *arg) {
    sleep(3);
    shareRes_t * pshareRes = (shareRes_t *)arg;
    pthread_mutex_lock(&pshareRes->mutex);
    while (pshareRes->flag != 3) {
        printf("3 is not ready!\n");
        pthread_cond_wait(&pshareRes->cond, &pshareRes->mutex);
    }

    pshareRes->flag = 0;
    printf("I got 3!\n");
    pthread_mutex_unlock(&pshareRes->mutex);
    pthread_exit(NULL);
}

int main(int argc, char* argv[]) {
    shareRes_t pshareRes;
    pshareRes.flag = 0;
    pthread_mutex_init(&pshareRes.mutex, NULL);
    pthread_cond_init(&pshareRes.cond, NULL);

    pthread_t tid1, tid2, tid3;
    pthread_create(&tid1, NULL, customer, &pshareRes);
    pthread_create(&tid2, NULL, customer1, &pshareRes);
    pthread_create(&tid3, NULL, customer2, &pshareRes);

    sleep(4);
    int i;
    while (1) {
        printf("Please input a already num (input 0 to exit): \n");
        if (scanf("%d", &i) != 1) {
            // 处理输入不是数字的情况
            printf("Invalid input. Please enter a valid number.\n");
            while (getchar() != '\n'); // 清除输入缓冲区
            continue;
        }
        if (i == 0) {
            break;
        }
        printf("I producer %d\n", i);
        pthread_mutex_lock(&pshareRes.mutex);
        pshareRes.flag = i;
        pthread_cond_broadcast(&pshareRes.cond);
        pthread_mutex_unlock(&pshareRes.mutex);
    }

    pthread_join(tid1, NULL);
    pthread_join(tid2, NULL);
    pthread_join(tid3, NULL);

    // 销毁互斥锁和条件变量
    pthread_mutex_destroy(&pshareRes.mutex);
    pthread_cond_destroy(&pshareRes.cond);

    return 0;
}
