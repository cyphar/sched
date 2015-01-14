#include <stdio.h>
#include <stdbool.h>
#include <sys/time.h>

#include "sched.h"

/* Here follows a "simple" scheduler (simple to implement, less-than-simple to
 * use) written for use on an Arduino to schedule a memory-limited number of
 * pseudo-asynchronous scheduled events (no premptive scheduling is implemented,
 * so blocking functions will block everything else -- BREAK DOWN YOUR FUNCTIONS. */

/* TODO: Use multiple stacks with task_delay() to allow for dynamic context
 * switching between several running tasks that are blocking or scheduled to run. */

/* NOTE: Here be dragons if you start modifying internal stuff.
 *       There is some magic here that we will try to document.
 *       If you didn't write it, don't touch it. */

/* helper function to properly clear a task */
void task_clear(struct task_t *task) {
	task->task = NULL;
	task->task_arg = NULL;
	task->mtime = 0;
	task->_next_mtime = 0;
	task->flag = NOOP;
}

/* initialises the scheduler */
void sched_init(struct sched_t *sched) {
	int i;
	for(i = 0; i < TASK_BUFFER_SIZE; i++) {
		task_clear(&sched->registered[i]);
	}
}

/* returns the current time in microseconds with some arbitrary start point */
static long __sched_get_mtime(void) {
#if 0
	/* just use the arduino epoch */
	return millis();
#endif
	/* for use on POSIX machines */
	struct timeval tv;
	gettimeofday(&tv, NULL);

	/* convert to milliseconds */
	return (tv.tv_sec * 1000) + (tv.tv_usec / 1000);
}

/* (internal) get the next available slot in the set of registered tasks */
/* returns 0 if no errors,
 * else -1 if ENOMEM */
static int __sched_next_slot(struct sched_t *sched) {
	int i;

	for(i = 0; i < TASK_BUFFER_SIZE; i++) {
		/* NOOPs and NULL task function pointers are treated as blanks */
		if(sched->registered[i].task == NULL || sched->registered[i].flag == NOOP) {
			return i;
		}
	}

	return -1;
}

/* (internal) returns whether the two task structures are equivalent */
static bool __tasks_equivalent(struct task_t left, struct task_t right) {
	return left.task == right.task &&
		   left.task_arg == right.task_arg &&
		   left.mtime == right.mtime &&
		   /* XXX: this makes deregistering periodic functions impossible */
		   /*left._next_mtime == right._next_mtime &&*/
		   left.flag == right.flag;
}

/* (~internal) deregisters the given task pointer */
void _sched_deregister(struct task_t *task) {
	/* clear task */
	task_clear(task);
}

/* (~internal) finds first match of the task and returns its pointer */
/* returns NULL if the task could not be found */
struct task_t *_sched_find(struct sched_t *sched, struct task_t task) {
	int i;

	for(i = 0; i < TASK_BUFFER_SIZE; i++) {
		struct task_t *taskp = &sched->registered[i];
		if(__tasks_equivalent(*taskp, task)) {
			return taskp;
		}
	}

	return NULL;
}

/* deregister a task from the scheduled tasks (only the first one found is deregistered */
int sched_deregister(struct sched_t *sched, struct task_t task) {
	struct task_t *taskp = _sched_find(sched, task);

	if(!taskp)
		return -1;

	_sched_deregister(taskp);
	return 0;
}

/* (~internal) registers the given task (a single task /can/ be registered more than once) */
/* NOTE: no treatment of _* attributes is done by this function */
/* returns 0 if no errors,
 * else -1 if ENOMEM */
int _sched_register(struct sched_t *sched, struct task_t task) {
	int i = __sched_next_slot(sched);
	if(i < 0) {
		return -1;
	}

	/* register the task */
	sched->registered[i] = task;
	return 0;
}

/* registers the given task (a single task /can/ be registered more than once) */
/* returns 0 if no errors,
 * else -1 if ENOMEM,
 *      -2 if invalid task */
int sched_register(struct sched_t *sched, struct task_t task) {
	long unow = __sched_get_mtime();

	switch(task.flag) {
		case IDLE:
			/* clear out time data */
			task.mtime = 0;
			task._next_mtime = 0;
			break;
		case ONCE:
			/* make mtime relative to the time of registration */
			task._next_mtime = unow + task.mtime;
			break;
		case PERIODIC:
			/* set to run on the first tick after registration */
			task._next_mtime = 0;
			break;
		case NOOP:
		default:
			return -2;
	}

	/* pass to internal register */
	return _sched_register(sched, task);
}

/* (internal) executes a task as well as re-registering PERIODIC tasks */
static void __sched_execute(struct sched_t *sched, struct task_t *task) {
	/* get current time */
	long unow = __sched_get_mtime();

	/* execute task */
	if(task->task)
		(*task->task)(task->task_arg);

	/* make a copy of the task data */
	struct task_t new_task = *task;

	/* deregister task */
	_sched_deregister(task);

	/* deal with registering tasks */
	switch(new_task.flag) {
		case PERIODIC:
			/* register next task to run */
			/* XXX: what happens if we ENOMEM? */
			new_task._next_mtime = unow + new_task.mtime;
			_sched_register(sched, new_task);
			break;
		case NOOP:
		case IDLE:
		case ONCE:
		default:
			/* do not re-register */
			break;
	}
}

/* flag checking macros */
#define __TASK_IS_NOOP(task) ((task).flag == NOOP)
#define __TASK_IS_IDLE(task) ((task).flag == IDLE)
#define __TASK_IS_REGULAR(task) (!(__TASK_IS_NOOP(task) || __TASK_IS_IDLE(task)))

/* executes one "tick" of the task scheduler */
void sched_tick(struct sched_t *sched) {
	long unow = __sched_get_mtime();
	bool idle = true;

	/* run through regular tasks */
	int i;
	for(i = 0; i < TASK_BUFFER_SIZE; i++) {
		struct task_t *task = &sched->registered[i];
		/* check if the task has been scheduled to run now or in the past */
		if(__TASK_IS_REGULAR(*task) && task->_next_mtime <= unow) {
			/* execute and set the idle tick flag to false */
			__sched_execute(sched, task);
			idle = false;
		}
	}

	/* this is an idle tick -- run through idle tasks */
	/* XXX: do some form of check to see if the scheduler is *actually* idle for a good period of time */
	if(idle) {
		for(i = 0; i < TASK_BUFFER_SIZE; i++) {
			struct task_t *task = &sched->registered[i];
			if(__TASK_IS_IDLE(*task))
				__sched_execute(sched, task);
		}
	}
}
