#include "pstdint.h"
#include <stdio.h>

#define MALLOC malloc
#define FREE free

#define RING_BUFFER_DEFAULT_SLOT_COUNT 4

#ifdef DATA_POINTER_TYPE
#   undef DATA_POINTER_TYPE
#endif
#define DATA_POINTER_TYPE unsigned char

typedef void  (*thread_pool_job_func)(void*);
typedef void* (*thread_pool_yield_param_func)(int);

/*!
 * @brief Ring buffer object.
 */
struct ring_buffer_t
{
	uintptr_t job_slots;
	uintptr_t job_size_in_bytes;
	uintptr_t write_pos;
	uintptr_t read_pos;
	DATA_POINTER_TYPE* obj_buffer;
	DATA_POINTER_TYPE* flg_buffer;
};

/*!
 * @brief Flag buffer values.
 *
 * Meaning:
 * + FLAG_FREE_SLOT         : The corresponding read/write position is empty.
 *                            It is ready to accept data. Before writing data,
 *                            the flag must be changed to FLAG_WRITE_IN_PROGRESS.
 * + FLAG_WRITE_IN_PROGRESS : The corresponding read/write position is in the
 *                            process of being written to. Once writing is
 *                            finished, the flag must be changed to
 *                            FLAG_READ_ME.
 * + FLAG_FILLED_SLOT       : The corresponding read/write position contains
 *                            data ready for reading. Before reading data, the
 *                            flag must be changed to FLAG_READ_IN_PROGRESS.
 */
typedef enum ring_buffer_flags_e
{
	FLAG_FREE_SLOT = 0,
	FLAG_WRITE_IN_PROGRESS = 1,
	FLAG_FILLED_SLOT = 2,
} ring_buffer_flags_e;

typedef enum thread_pool_policy_e
{
	POLICY_SLEEP = 0,
	POLICY_SPIN = 1
} thread_pool_policy_e;

struct thread_pool_t;
struct thread_pool_worker_t
{
	pthread_t             thread;        /* thread handle */
	pthread_mutex_t       mutex;         /* worker mutex for wakeup condition */
	pthread_cond_t        wakeup_cv;     /* condition variables for waking up a worker thread - lock worker_mutex for access */
	struct ring_buffer_t  ring_buffer;   /* ring buffer for storing jobs, 1 for each thread */
	struct thread_pool_t* pool;          /* the pool that owns this worker */
};

struct thread_pool_t
{
	int             num_threads;         /* number of worker threads to spawn on resume */
	int             active_jobs;         /* number of active jobs - use atomics to modify */
	int             selected_worker;     /* index of the worker to give an incoming job - NOTE: doesn't wrap, use modulo - use atomics to modify */
	char            active;              /* whether or not the pool is active - use atomics to modify */
	char            never_sleep;         /* when non-zero, idle workers will spinlock */

	struct thread_pool_worker_t* worker; /* vector of workers */

	pthread_spinlock_t queue_lock;       /* used to safely increment write_pos when queuing a job */
	pthread_mutex_t    mutex;            /* pool mutex - used for active_jobs and signalling completed jobs */
	pthread_cond_t     job_finished_cv;  /* condition variable for waking up threads waiting on finished jobs - lock mutex for access */
};

struct thread_pool_job_t
{
	thread_pool_job_func func;
	void* data;
};

struct thread_pool_job_range_t
{
	thread_pool_job_func job;
	thread_pool_yield_param_func yield_param;
	int begin;
	int end;
};

uint32_t
get_number_of_cores();

struct thread_pool_t*
thread_pool_create(uint32_t num_threads, uint32_t buffer_size_in_bytes);

void
thread_pool_destroy(struct thread_pool_t* pool);

void
thread_pool_queue(struct thread_pool_t* pool, thread_pool_job_func func, void* data);

void
thread_pool_queue_range(struct thread_pool_t* pool,
						thread_pool_job_func job,
						thread_pool_yield_param_func yield_param,
						int begin, int end);

void
thread_pool_set_idle_worker_policy(struct thread_pool_t* pool,
								   thread_pool_policy_e policy);

void
thread_pool_suspend(struct thread_pool_t* pool);

void
thread_pool_resume(struct thread_pool_t* pool);

void
thread_pool_wait_for_jobs(struct thread_pool_t* pool);
