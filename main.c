#include <stdio.h>
#include "thread_pool/thread_pool.h"
#include <unistd.h>
#include <time.h>
#include <pthread.h>

static int counter = 0;

void work(void* discard)
{
	__sync_add_and_fetch(&counter, 1);
}

void* yield_work_param(int index)
{
	return NULL;
}

int main()
{
	int i;
	clock_t start;
	struct thread_pool_t* pool = thread_pool_create(0, 0);
	thread_pool_set_idle_worker_policy(pool, POLICY_SPIN);

	start = clock();
	for(i = 0; i != 100000; ++i)
		thread_pool_queue(pool, (thread_pool_job_func)work, NULL);
	printf("Elapsed time: %.2f.\n", (clock() - start) * 0.000001);
	thread_pool_wait_for_jobs(pool);
	thread_pool_destroy(pool);

	printf("%d\n", counter);

	return 0;
}
