/* sched: a simple scheduler written in C.
 * Copyright (C) 2015 Aleksa Sarai
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#if !defined(__SCHED_H__)
#define __SCHED_H__

/* feel free to change this, but it is proportional to time and space complexity for a bunch of operations */
#if !defined(SCHED_BUFFER_SIZE)
#	define SCHED_BUFFER_SIZE 100
#endif

/* the amount of time in milliseconds that the scheduler must be idle in order for idle tasks to be run */
#if !defined(SCHED_IDLE_WINDOW)
#	define SCHED_IDLE_WINDOW 200
#endif

/* task type enum */
enum {
	NOOP,      /* do not execute, it may be overwritten by the next register call */
	IDLE,      /* run at the next idle tick (time related variables are ignored, but should be set to 0) */
	ONCE,      /* run once at (the specified time + the time of registering the task) milliseconds */
	PERIODIC,  /* run every (specified time) milliseconds (starting at the first tick after being registered). */
};

typedef void (*task_fp)(void *);

/* generic "task" structure */
/* in order for two task structures to be considered equivalent, all attributes must be equal */
/* any changes made to a _* attribute may (and probably will) be overwritten by the scheduler (or, if you're lucky, crash the scheduler) */
struct task_t {
	/* the task function pointer to be run with the given argument */
	task_fp task;
	void *task_arg;

	/* a time-related variable, which has a different meaning depending on the flag */
	long mtime;

	/* represents the "next" scheduled time it is meant to run (allows for period tasks) */
	long _next_mtime;

	/* flag for a task, used to provide context for the mtime element */
	int flag;
};

/* overall scheduler structure */
struct sched_t {
	struct task_t _tasks[SCHED_BUFFER_SIZE];
};

void task_clear(struct task_t *);
void sched_init(struct sched_t *);
int sched_deregister(struct sched_t *, struct task_t);
int sched_register(struct sched_t *, struct task_t);
void sched_tick(struct sched_t *);

#endif
