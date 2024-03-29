#ifndef _THREAD_POOL_H__
#define _THREAD_POOL_H__

#include <pthread.h>

typedef struct task_t
{
    void * (*func)(void *arg);
    void * arg;
} Task_t;

typedef struct threadpool
{
    Task_t * queue;
    /* 任务队列的容量 */
    int queueCapacity;
    /* 任务队列的任务个数 */
    int queueSize;
    /* 任务队列的队头元素 */
    int queueFront;
    /* 任务队列的队尾元素 */
    int queueRear;

    /* 线程号 */
    pthread_t *threadIds;   /* 工作的线程*/
    pthread_t managerId;    /* 管理的线程 */
    int busy_thread_num;    /* 干活的线程数 */
    int alive_thread_num;   /* 活着的线程数 */    
    int min_thread_num;     /* 最小的线程数 */
    int max_thread_num;     /* 最大的线程数 */

    pthread_mutex_t mutex;          /* 锁资源 */
    pthread_mutex_t busyMutex;      /* 忙 锁🔒 */
    pthread_cond_t queueBusy;     /* 有任务 */
    pthread_cond_t queueFree;    /* 没有任务 */

    int exitNum;                /* 关闭的线程数 */
    int shutdown;               /* 关闭的标志位 */
} ThreadPool;


/* 线程池初始化 */
int threadPoolInit(ThreadPool * pool, int threadMinThread, int threadMaxThreads, int queueCapacity);


/* 添加任务 */
int threadPoolAdd(ThreadPool * pool, void * (*func)(void * arg), void * arg);


/* 线程池的销毁 */
int threadPoolDestroy(ThreadPool * pool);


#endif //_THREAD_POOL_H__