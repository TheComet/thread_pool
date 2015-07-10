#include <stdio.h>
#include "thread_pool/thread_pool.h"
#include <unistd.h>

static int counter = 0;

void work(void* discard)
{
	__sync_fetch_and_add(&counter, 1);
	volatile int i = 0;
	for(; i < 100; ++i)
	{
	}
}

void* yield_work_param(int index)
{
	return (void*)index;
}

int main()
{
	int i;
	struct thread_pool_t* pool = thread_pool_create(0, 0);
	thread_pool_queue_range(pool,
							(thread_pool_job_func)work,
							(thread_pool_yield_param_func)yield_work_param,
							0, 10000000);
	thread_pool_wait_for_jobs(pool);
	printf("%d\n", counter);
	thread_pool_queue_range(pool,
							(thread_pool_job_func)work,
							(thread_pool_yield_param_func)yield_work_param,
							0, 10000000);
	thread_pool_wait_for_jobs(pool);
	printf("%d\n", counter);
	thread_pool_destroy(pool);
	printf("%d\n", counter);

	return 0;
}
