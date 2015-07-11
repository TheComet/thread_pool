Features & Implementation Details
=================================
The focus of this implementation was primarily on being able to process high
quantities of small jobs by defeating bottlenecks surrounding insertion times,
as well as removing some minor but nonetheless usability-breaking limitations
found in other thread pool implementations.

Being able to cope with high quantities of small jobs can greatly benefit
something like an ECS framework where jobs are small and plentiful.

In summary, this thread pool has the following outstanding features:
 + Very fast job insertion times
 + Support for job range insertions (mass job insertions) with no additional
   overhead
 + The ability to wait for all jobs to complete (and not just wait until the
   job queue is empty but jobs are still being processed)
 + The ability to suspend and resume the thread pool at any time
 + Customisable job queue size at runtime (but not during job execution)

Eliminating the Insertion Time Bottleneck
=========================================
You may have heard that thread pools only become beneficial when jobs have "at
least a few thousand clock cycles worth of work". This is only *partially*
true.

Thread pools become beneficial when the time it would take to execute a job
outweighs the time it takes to insert and wake up a worker thread to come pick
it up (as well as various synchronisation overheads for the main thread to pick
up the results again).



Job objects are copied (lock free) into the job queue rather than being
   allocated on the heap. During mass insertion tests the bottleneck proved
   not to be the actual insertion time, but the time it took to allocate memory
   for the job object itself. This bottleneck is removed by using a double ring
   buffer (more details below).
Instead of using a for-loop to insert all of your jobs into the job queue, why not split the for-loop into
   *n* number of for-loops and insert those into the job queue? This saves a
