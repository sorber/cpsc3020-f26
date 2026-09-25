#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

#include "workqueue.h"
#include <pthread.h>

int counter = 0;
#define NUMTASKS 4
#define NUMTHREADS 5
#define CAPACITY 2

// colored output is fun
#define RED "\033[31m"
#define GREEN "\033[32m"
#define YELLOW "\033[33m"
#define BLUE "\033[34m"
#define RESET "\033[0m"

pthread_mutex_t __mainlock = PTHREAD_MUTEX_INITIALIZER;
pthread_t mainthread;

#define ARGVAL 47

void PASS(const char *msg)
{
    printf(GREEN "PASS:\t%s\n" RESET, msg);
}

void FAIL(const char *msg)
{
    printf(RED "\tFAIL:\t%s\n" RESET, msg);
}
void TEST(bool condition, const char *passmsg, const char *errmsg, bool fatal)
{
    if (!condition)
    {
        FAIL(errmsg);
        if (fatal)
            exit(EXIT_FAILURE); // no point continuing the test
    }
    else
    {
        PASS(passmsg);
    }
}

void *taskfunc(void *arg)
{
    int *iarg = (int *)arg;

    TEST((*iarg >= 0 && *iarg < NUMTASKS), "Arg passing looks reasonable", "Passed argument out of range.", false);
    if (pthread_equal(mainthread, pthread_self()))
    {
        TEST(false, "running tasks in new thread", "The queue is calling tasks from the main thread, rather than doing the work in worker threads.", true);
    }

    pthread_mutex_lock(&__mainlock);
    counter++;
    pthread_mutex_unlock(&__mainlock);

    free(arg);
    return NULL;
}

int main(int argc, char **argv)
{
    mainthread = pthread_self();

    wq_t *myworkqueue = wq_create(NUMTHREADS, CAPACITY);
    TEST(myworkqueue != NULL, "create succeeded", "wq_create returned NULL.", true);

    wq_job_id_t taskids[NUMTASKS];

    for (int i = 0; i < NUMTASKS; i++)
    {
        int *iarg = malloc(sizeof *iarg);
        *iarg = i;
        taskids[i] = wq_submit(myworkqueue, taskfunc, iarg);
        TEST(taskids[i] > 0, "taskid looks good", "wq_submit returned a task ID that was not positive.", false);
    }

    wq_wait(myworkqueue, taskids, NUMTASKS);

    if (counter == NUMTASKS)
    {
        PASS("counter matches the expected value.");
    }
    else if (counter > NUMTASKS)
    {
        FAIL("It looks like we ran too many tasks. Not sure what's going on, but it's not right.");
    }
    else if (counter > 0)
    {
        FAIL("Looks like we ran something, but too few tasks.");
    }
    else
    {
        FAIL("No tasks were executed.");
    }

    return EXIT_SUCCESS;
}