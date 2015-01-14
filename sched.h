/* feel free to change this, but it is proportional to time and space complexity for a bunch of operations */
#define TASK_BUFFER_SIZE 100

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
	struct task_t registered[TASK_BUFFER_SIZE];
};

void task_clear(struct task_t *);
void sched_init(struct sched_t *);
int sched_deregister(struct sched_t *, struct task_t);
int sched_register(struct sched_t *, struct task_t);
void sched_tick(struct sched_t *);
