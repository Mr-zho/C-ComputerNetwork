#include "threadPool.h"
#include <stdbool.h>
#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <errno.h>
#include <error.h>
#include <stdlib.h>
#include <string.h>

/* 状态码 */
enum STATUS_CODE
{
    ON_SUCCESS,
    MALLOC_ERROR,
    NULL_PTR,
    INVALID_ACCESS,
};

#define TIME_INTERVAL   5
#define DEFAULT_THREAD_NUM  10
#define PER_NUMBER      3


/* 工作线程 */
static void * thread_worker(void *arg)
{
    pthread_detach(pthread_self());

    ThreadPool *pool = (ThreadPool *)arg;

    while (true)
    {
        /* 加锁 */
        pthread_mutex_lock(&pool->mutex);
        while (pool->queueSize == 0 && (pool->shutdown == 0))
        {
            /* 等待条件变量 */
            pthread_cond_wait(&pool->queueBusy, &pool->mutex);

            /* 判断是不是要销毁线程 */
            if (pool->exitNum > 0)
            {
                /* 离开的线程数减一. */
                pool->exitNum--;
                if (pool->alive_thread_num > pool->min_thread_num)
                {
                    pthread_mutex_unlock(&(pool->mutex));
                    
                    /* 活着的线程数减一. */
                    pool->alive_thread_num--;

                    printf("thread id is %ld exit...file:%s, func:%s, lines:%d\n", pthread_self(), __FILE__, __FUNCTION__,__LINE__);
                
                    /* 方案1: 清理线程号 */
                    pthread_exit(NULL);
                }   
            }
        }

        /* 判断线程池是否被关闭 */
        if (pool->shutdown)
        {
            pthread_mutex_unlock(&(pool->mutex));
            /* 活着的线程数减一. */
            pool->alive_thread_num--;
            printf("thread id is %ld exit...file:%s, func:%s, lines:%d\n", pthread_self(), __FILE__, __FUNCTION__,__LINE__);

            pthread_exit(NULL);
        }

        /* 程序到这个地方 有任务 */
        /* 从任务队列中取任务 */
        Task_t newTask;
        newTask.func = pool->queue[pool->queueFront].func;
        newTask.arg = pool->queue[pool->queueFront].arg;
        /* 移动队头位置 */
        pool->queueFront = (pool->queueFront + 1) % pool->queueCapacity;
        /* 任务队列 任务数减一. */
        pool->queueSize--;

        /* 解锁 */
        pthread_mutex_unlock(&pool->mutex);
        pthread_cond_signal(&pool->queueFree);


        printf("thread id is %ld start working...\n", pthread_self());
        pthread_mutex_lock(&pool->busyMutex);
        pool->busy_thread_num++;
        pthread_mutex_unlock(&pool->busyMutex);


        /* 处理任务 */
        newTask.func(newTask.arg);

        pthread_mutex_lock(&pool->busyMutex);
        pool->busy_thread_num--;
        pthread_mutex_unlock(&pool->busyMutex);
        printf("thread id is %ld end working...\n", pthread_self());
#if 0   
        /* todo... */
        if (pool->shutdown == 1 && pool->queueSize != 0)
        {
            pthread_cancel(pthread_self());
        }
#endif
    }

    pthread_exit(NULL);
}

/* 判断线程是否存在 */
static bool threadIsAlive(pthread_t tid)
{
    int res = pthread_kill(tid, 0);
    if (res == ESRCH)
    {
        return false;
    }
    return true;
}


/* 任务调度 */
static void * thread_manage(void *arg)
{
    /* 线程分离 */
    pthread_detach(pthread_self());

    ThreadPool *pool = (ThreadPool *)arg;

    while (pool->shutdown != 1)
    {
        sleep(TIME_INTERVAL);

        /* 取出线程池中的 任务队列的任务个数 */
        pthread_mutex_lock(&pool->mutex);
        int queueSize = pool->queueSize;
        /* 存活的线程数 */
        int alive_thread_num = pool->alive_thread_num;
        pthread_mutex_unlock(&pool->mutex);

        /* 当前忙的线程 */
        pthread_mutex_lock(&pool->busyMutex);
        int busy_thread_num = pool->busy_thread_num;
        pthread_mutex_unlock(&pool->busyMutex);


        /* 什么时候需要增加线程池中的线程数? */
        /* 任务队列的任务远远大于 活着的线程数 */

        /* 添加线程 */
        /* 任务个数 > 存活的线程数 && 存活线程数 < 最大的线程数 */
        if (queueSize > alive_thread_num - busy_thread_num && alive_thread_num < pool->max_thread_num)
        {
            pthread_mutex_lock(&(pool->mutex));
            int count = 0;
            for (int idx = 0; idx < pool->max_thread_num && count < PER_NUMBER; idx++)
            {
                /* 方案2. 探测 */
                if (pool->threadIds[idx] == 0 || !threadIsAlive(pool->threadIds[idx]));
                {
                    pthread_create(&pool->threadIds[idx], NULL, thread_worker, pool);
                    printf("thread id is %ld join pool..., file:%s, func:%s, lines:%d\n", pool->threadIds[idx], __FILE__, __FUNCTION__,__LINE__);

                    count++;
                    /* 活着的线程数 */
                    pool->alive_thread_num++;
                }
            }
            pthread_mutex_unlock(&(pool->mutex));
        }


        /* 什么时候需要减少线程池中的线程数? */
        /* 任务队列的任务远远 小于 活着的线程数 */

        /* 减少线程 */
        if ((queueSize + (queueSize >> 1)) < alive_thread_num && alive_thread_num > pool->min_thread_num)
        {
            pthread_mutex_lock(&(pool->mutex));
            pool->exitNum = PER_NUMBER;
            for (int idx = 0; idx < PER_NUMBER; idx++)
            {
                pthread_cond_signal(&(pool->queueBusy));
            }
            pthread_mutex_unlock(&(pool->mutex));
        }
    }

    /* 线程退出 */
    pthread_exit(NULL);
}

/* 线程池初始化 */
int threadPoolInit(ThreadPool * pool, int threadMinThread, int threadMaxThreads, int queueCapacity)
{
    if (pool == NULL)
    {
        return NULL_PTR;
    }

    if (threadMaxThreads <= 0)
    {
        threadMaxThreads = DEFAULT_THREAD_NUM;
    }
    pool->min_thread_num = threadMinThread;
    pool->max_thread_num = threadMaxThreads;

    /* 条件变量 和 锁 资源初始化 */
    pthread_mutex_init(&(pool->mutex), NULL);
    pthread_mutex_init(&(pool->busyMutex), NULL);
    pthread_cond_init(&(pool->queueBusy), NULL);
    pthread_cond_init(&(pool->queueFree), NULL);

    
    /* 初始化队列相关属性 */
    pool->queueSize = 0;
    pool->queueCapacity = queueCapacity;
    pool->queueFront = 0;
    pool->queueRear = 0;

    /* 任务队列初始化 */
    pool->queue = (Task_t *)malloc(sizeof(Task_t) * pool->queueCapacity);
    if (pool->queue == NULL)
    {
        perror("malloc error");
        exit(-1);
    }

    pool->threadIds = (pthread_t *)malloc(sizeof(pthread_t) * threadMaxThreads);
    if (pool->threadIds == NULL)
    {
        perror("malloc error");
        exit(-1);
    }
    /* 清除脏数据 */
    memset(pool->threadIds, 0, sizeof(pthread_t) * threadMaxThreads);


    int ret = pthread_create(&pool->managerId, NULL, thread_manage, pool);
    if (ret == -1)
    {
        perror("thread create error");
        exit(-1);
    }

    /* 初始化离开的线程数为0. */
    pool->exitNum = 0;
    /* 初始化的时候 忙碌的线程数为0. */
    pool->busy_thread_num = 0;
    pool->alive_thread_num = 0;
    for (int idx = 0; idx < threadMinThread; idx++)
    {
        ret = pthread_create(&pool->threadIds[idx], NULL, thread_worker, pool);
        if (ret == -1)
        {
            perror("thread create error");
            exit(-1);
        }
        /* 活着的线程数 */
        pool->alive_thread_num++;
    }

    /* 默认不关闭 */
    pool->shutdown = 0;

    return ON_SUCCESS;
}


/* 添加任务 */
int threadPoolAdd(ThreadPool * pool, void * (*func)(void * arg), void * arg)
{
    if (pool == NULL)
    {
        return NULL_PTR;
    }

    /* 上锁🔒 */
    pthread_mutex_lock(&pool->mutex);
    while (pool->queueSize == pool->queueCapacity)
    {
        pthread_cond_wait(&(pool->queueFree), &pool->mutex);
    }
    /* 程序到这里, 说明什么? */
    pool->queue[pool->queueRear].func = func;
    pool->queue[pool->queueRear].arg = arg;
    pool->queueRear = (pool->queueRear + 1) % pool->queueCapacity;
    /* 任务队列的元素个数++ */
    pool->queueSize++;

    /* 解锁 */
    pthread_mutex_unlock(&pool->mutex);
    pthread_cond_signal(&pool->queueBusy);

    return ON_SUCCESS;
}

/* 线程池的销毁 */
int threadPoolDestroy(ThreadPool * pool)
{
    if (pool == NULL)
    {
        return NULL_PTR;
    }

    /* 关闭线程池的标签 */
    pool->shutdown = 1;

    /* 发信号 */
    for (int idx = 0; idx < pool->alive_thread_num; idx++)
    {
        pthread_cond_signal(&(pool->queueBusy));
    }

#if 0
    for (int idx = 0; idx < pool->max_thread_num; idx++)
    {
        if (pool->threadIds[idx] != 0)
        {
            pthread_cancel(pool->threadIds[idx]);
        }
    }
#endif

    while (pool->alive_thread_num != 0)
    {
        sleep(3);
    }


    /* 释放 锁资源 */
    pthread_mutex_destroy(&(pool->mutex));
    pthread_mutex_destroy(&(pool->busyMutex));
    /* 释放 条件变量资源 */
    pthread_cond_destroy(&(pool->queueFree));
    pthread_cond_destroy(&(pool->queueBusy));

    
    /* 释放 任务队列 堆空间内存 */
    if (pool->queue)
    {
        free(pool->queue);
        pool->queue = NULL;
    }

    /* 释放 工作线程 堆空间内存 */
    if (pool->threadIds)
    {
        free(pool->threadIds);
        pool->threadIds = NULL;
    }

    return ON_SUCCESS;
}
