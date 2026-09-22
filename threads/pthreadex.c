#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>
#include <stdint.h>

#define BIG 1000000000L // 1 billion

pthread_mutex_t myfancylock = PTHREAD_MUTEX_INITIALIZER;
uint64_t sum = 0;

void add_up_to_big()
{
	for (uint64_t i = 0; i < BIG; i++)
	{
		// don't switch until...
		pthread_mutex_lock(&myfancylock);
		// critical section
		sum += i; // atomic
		pthread_mutex_unlock(&myfancylock);
		//..now!!
	}
}

void *mythreadfunc(void *arg)
{
	add_up_to_big();
	int *ptr_to_int = malloc(sizeof(int));
	*ptr_to_int = 47;
	return ptr_to_int;
}

int main(int argc, char **argv)
{

	pthread_t t1, t2;

	pthread_create(&t1, NULL, mythreadfunc, NULL);
	pthread_create(&t2, NULL, mythreadfunc, NULL);
	// add_up_to_big();
	// add_up_to_big();

	// pthread_cancel(t1);

	int *r1, *r2;
	pthread_join(t1, (void **)&r1);
	pthread_join(t2, (void **)&r2);
	printf("The result is %lu\n", sum);
	return 0;
}
