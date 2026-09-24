#include <time.h>
#include <pthread.h>
#include <assert.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>
#include <stdbool.h>

// #define BUSY

typedef struct task
{
    bool busy;
    char *filename;
} task;

task workertask = {0};

pthread_cond_t workReady_cv = PTHREAD_COND_INITIALIZER;
pthread_mutex_t worker_lock = PTHREAD_MUTEX_INITIALIZER;

void printfileinfo(char *filename);
void produce_work(char *filename);
void *worker_func(void *tid);

int main(int argc, char **argv)
{

    pthread_t worker;

    if (pthread_create(&worker, NULL, worker_func, NULL) != 0)
    {
        fprintf(stderr, "Could not create worker thread.\n");
        exit(-1);
    }

    printf("main: about to start giving the tasks to the worker...\n");

    while (1)
    {
        produce_work("./pthreadex.c");
        usleep(rand() % 100000);
        produce_work("./Makefile");
        usleep(rand() % 100000);
        produce_work("./pthreadex");
        sleep(1);
    }
}

void produce_work(char *filename)
{
    pthread_mutex_lock(&worker_lock);
    if (workertask.busy)
    {
        printf("sorry, worker is busy: %s will have to wait\n", filename);
        pthread_mutex_unlock(&worker_lock);
        return;
    }
    workertask.busy = true;
    workertask.filename = filename;
    pthread_mutex_unlock(&worker_lock);
#ifndef BUSY
    pthread_cond_signal(&workReady_cv); // wake up the worker
#endif
}

void *worker_func(void *arg)
{
    printf("worker started!\n");

    // forever, get work and do it.
    while (1)
    {
        pthread_mutex_lock(&worker_lock);
        if (workertask.busy)
        {
            printfileinfo(workertask.filename);
            workertask.busy = false;
        }
        else
        {
#ifndef BUSY
            pthread_cond_wait(&workReady_cv, &worker_lock);
#endif
        }
        pthread_mutex_unlock(&worker_lock);
    }
}

void printfileinfo(char *filename)
{
    struct stat finfo;

    if (stat(filename, &finfo) != 0)
    {
        printf("worker: couldn't stat \"%s\"", filename);
    }
    else
    {
        printf("worker: %s\n"
               "\tsize: %ld bytes\n"
               "\tblocks: %ld\n"
               "\tuser-id: %d\n\n",
               filename, finfo.st_size, finfo.st_blocks, finfo.st_uid);
    }
}
