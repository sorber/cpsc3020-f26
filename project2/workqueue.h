// workqueue.h
#ifndef WORKQUEUE_H
#define WORKQUEUE_H

#include <stdlib.h>
#include <stdbool.h>

typedef struct wq wq_t;
typedef int wq_job_id_t;

typedef void *(*wq_job_fn)(void *arg);

wq_t *wq_create(size_t num_workers, size_t queue_capacity);

// blocking submit - waits until there is room on the queue, and then submis the task.
// returns the submitted task's id ()
wq_job_id_t wq_submit(wq_t *q, wq_job_fn fn, void *arg);

//
void wq_wait(wq_t *q, wq_job_id_t *ids, int numids);
void wq_shutdown(wq_t *q);

#endif