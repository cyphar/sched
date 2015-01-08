#include <stdio.h>
#include <sys/time.h>

/* Here follows a "simple" scheduler (simple to implement, less-than-simple to
 * use) written for use on an Arduino to schedule a memory-limited number of
 * pseudo-asynchronous scheduled events (no premptive scheduling is implemented,
 * so blocking functions will block everything else -- BREAK DOWN YOUR FUNCTIONS. */

/* feel free to change this, but it is proportional to time and space complexity for a bunch of operations */
#define TASK_BUFFER_SIZE 100

enum task_flag_t {
	NOOP,      /* do not execute, it may be overwritten by the next register call */
	IDLE,      /* run at the next idle tick (time related variables are ignored, but should be set to 0) */
	ONCE,      /* run once at (the specified time + the time of registering the task) milliseconds */
	PERIODIC,  /* run every (specified time) milliseconds (starting at the first tick after being registered). */
};

/* NOTE: Here be dragons if you start modifying internal stuff.
 *       There is some magic here that we will try to document.
 *       If you didn't write it, don't touch it. */

/* generic "task" structure */
/* in order for two task structures to be considered equivalent, all attributes must be equal */
/* any changes made to a _* attribute may (and probably will) be overwritten by the scheduler (or, if you're lucky, crash the scheduler) */
struct task_t {
	/* the task function pointer to be run with the given argument */
	void (*task)(void *);
	void *task_arg;

	/* callbacks (passed a pointer to the task struct) */
	void (*callback_add)(struct task_t *);    /* when registered */
	void (*callback_before)(struct task_t *); /* before execution, when scheduled to run */
	void (*callback_after)(struct task_t *);  /* after execution, when scheduled to run */
	void (*callback_rm)(struct task_t *);     /* when deregistered (regardless of whether it ran or not) */

	/* a time-related variable, which has a different meaning depending on the flag */
	long mtime;

	/* represents the "next" scheduled time it is meant to run (allows for period tasks) */
	long _next_mtime;

	/* flag for a task, used to provide context for the mtime element */
	enum task_flag_t flag;
};

/* overall scheduler structure */
struct sched_t {
	struct task_t registered[TASK_BUFFER_SIZE];
};

/* global scheduler instance */
struct sched_t scheduler;

/* helper function to properly clear a task */
void __task_clear(struct task_t *task) {
	task->task = NULL;
	task->task_arg = NULL;
	task->callback_add = NULL;
	task->callback_before = NULL;
	task->callback_after = NULL;
	task->callback_rm = NULL;
	task->mtime = 0;
	task->_next_mtime = 0;
	task->flag = NOOP;
}

/* initialises the task register buffer */
void tasks_init(struct sched_t *sched) {
	int i;
	for(i = 0; i < TASK_BUFFER_SIZE; i++) {
		__task_clear(&sched->registered[i]);
	}
}

/* returns the current time in microseconds with some arbitrary start point */
long __tasks_get_mtime(void) {
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
int __tasks_next_slot(struct sched_t *sched) {
	int i;

	for(i = 0; i < TASK_BUFFER_SIZE; i++) {
		/* NOOPs and NULL task function pointers are treated as blanks */
		if(sched->registered[i].task == NULL || sched->registered[i].flag == NOOP) {
			return i;
		}
	}

	return -1;
}

#if !defined(bool)
#	define bool short
#	define true 1
#	define false 0
#endif

/* (internal) returns whether the two task structures are equivalent */
bool __tasks_equivalent(struct task_t left, struct task_t right) {
	return left.task == right.task &&
		   left.task_arg == right.task_arg &&
		   left.callback_add == right.callback_add &&
		   left.callback_before == right.callback_before &&
		   left.callback_after == right.callback_after &&
		   left.callback_rm == right.callback_rm &&
		   left.mtime == right.mtime &&
		   /* XXX: this makes deregistering periodic functions impossible */
		   left._next_mtime == right._next_mtime &&
		   left.flag == right.flag;
}

/* (~internal) deregisters the given task pointer */
void _tasks_deregister(struct task_t *task) {
	/* run callback */
	if(task->callback_rm)
		task->callback_rm(task);

	/* clear task */
	__task_clear(task);
}

/* (~internal) finds first match of the task and returns its pointer */
/* returns NULL if the task could not be found */
struct task_t *_tasks_find(struct sched_t *sched, struct task_t task) {
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
int tasks_deregister(struct sched_t *sched, struct task_t task) {
	struct task_t *taskp = _tasks_find(sched, task);

	if(!taskp)
		return -1;

	_tasks_deregister(taskp);
	return 0;
}

/* (~internal) registers the given task (a single task /can/ be registered more than once) */
/* NOTE: no treatment of _* attributes is done by this function */
/* returns 0 if no errors,
 * else -1 if ENOMEM */
int _tasks_register(struct sched_t *sched, struct task_t task) {
	int i = __tasks_next_slot(sched);
	if(i < 0) {
		return -1;
	}

	/* run the add callback */
	if(task.callback_add)
		task.callback_add(&task);

	/* register the task */
	sched->registered[i] = task;
	return 0;
}

/* registers the given task (a single task /can/ be registered more than once) */
/* returns 0 if no errors,
 * else -1 if ENOMEM,
 *      -2 if invalid task */
int tasks_register(struct sched_t *sched, struct task_t task) {
	long unow = __tasks_get_mtime();

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
	return _tasks_register(sched, task);
}

/* (internal) executes a task (and it's callbacks) as well as re-registering PERIODIC tasks */
void __task_execute(struct sched_t *sched, struct task_t *task, long unow) {
	/* execute the callbacks and main task */
	if(task->callback_before)
		task->callback_before(task);

	if(task->task)
		task->task(task->task_arg);

	if(task->callback_after)
		task->callback_after(task);

	/* make a copy of the task data */
	struct task_t new_task = *task;

	/* deregister task */
	_tasks_deregister(task);

	/* deal with registering tasks */
	switch(new_task.flag) {
		case PERIODIC:
			/* register next task to run */
			/* XXX: what happens if we ENOMEM? */
			new_task._next_mtime = unow + new_task.mtime;
			_tasks_register(sched, new_task);
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
void tasks_tick(struct sched_t *sched) {
	long unow = __tasks_get_mtime();
	bool idle = true;

	/* run through regular tasks */
	int i;
	for(i = 0; i < TASK_BUFFER_SIZE; i++) {
		struct task_t *task = &sched->registered[i];
		/* check if the task has been scheduled to run now or in the past */
		if(__TASK_IS_REGULAR(*task) && task->_next_mtime <= unow) {
			/* execute and set the idle tick flag to false */
			__task_execute(sched, task, unow);
			idle = false;
		}
	}

	/* this is an idle tick -- run through idle tasks */
	if(idle) {
		for(i = 0; i < TASK_BUFFER_SIZE; i++) {
			struct task_t *task = &sched->registered[i];
			if(__TASK_IS_IDLE(*task))
				__task_execute(sched, task, unow);
		}
	}
}
