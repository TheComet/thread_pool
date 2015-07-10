#if defined(ENABLE_MULTITHREADING) || defined(ENABLE_MEMORY_EXPLICIT_MALLOC_FAILURES)
#	if defined(THREAD_POOL_PLATFORM_LINUX) || defined(THREAD_POOL_PLATFORM_MACOSX)
#		include <pthread.h>
#		define MUTEX pthread_mutex_t
#		define MUTEX_LOCK(x) pthread_mutex_lock(&(x));
#		define MUTEX_UNLOCK(x) pthread_mutex_unlock(&(x));
#		define MUTEX_INIT(x) do {                                           \
				pthread_mutexattr_t attr;                                       \
				pthread_mutexattr_init(&attr);                                  \
				pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);      \
				pthread_mutex_init(&(x), &attr);                                \
				pthread_mutexattr_destroy(&attr);                               \
			} while(0);
#           define MUTEX_DEINIT(x) pthread_mutex_destroy(&(x));

#       else /* defined(THREAD_POOL_PLATFORM_LINUX) || defined(THREAD_POOL_PLATFORM_MACOSX) */
#			include <Windows.h>
#			include <process.h>
#           define MUTEX HANDLE
#			define MUTEX_LOCK(x) WaitForSingleObject(x, INFINITE);
#			define MUTEX_UNLOCK(x) ReleaseMutex(x);
#			define MUTEX_INIT(x) do { x = CreateMutex(NULL, FALSE, NULL); } while(0);
#			define MUTEX_DEINIT(x) CloseHandle(x);
#       endif /* defined(THREAD_POOL_PLATFORM_LINUX) || defined(THREAD_POOL_PLATFORM_MACOSX) */

	static MUTEX mutex;

#   else /* defined(ENABLE_MULTITHREADING) || defined(ENABLE_MEMORY_EXPLICIT_MALLOC_FAILURES) */
#       define mutex
#       define MUTEX_LOCK(x)
#       define MUTEX_UNLOCK(x)
#       define MUTEX_INIT(x)
#       define MUTEX_DEINIT(x)
#   endif /* defined(ENABLE_MULTITHREADING) || defined(ENABLE_MEMORY_EXPLICIT_MALLOC_FAILURES) */
