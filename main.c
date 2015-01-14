#include <stdio.h>
#include <stdbool.h>

#include "sched.h"

struct sched_t scheduler;

/* ====================== */
/* NOTE: ARDUINO EMULATOR */
/* ====================== */

bool on = true;
bool off = false;

void led(bool *state) {
	bool *next = NULL;

	switch(*state) {
		case true:
			next = &off;
			printf("explicit: on\n");
			break;
		case false:
			next = &on;
			printf("explicit: off\n");
			break;
	}

	struct task_t task;
	task_clear(&task);
	task.task = (task_fp) led,
	task.task_arg = next,
	task.mtime = 500,
	task.flag = ONCE,
	tasks_register(&scheduler, task);
}

void toggle_led(void *_) {
	printf("toggle: on\n");
	printf("toggle: off\n");
}

void idle(void *_) {
	/*printf("idling!\n");*/

	/* wait 100ms when idling */
	/*
	long mnow = __tasks_get_mtime();
	while(__tasks_get_mtime() < mnow + 5)
		;
	*/

	struct task_t task;
	task_clear(&task);
	task.task = (task_fp) idle,
	task.mtime = 5,
	task.flag = IDLE,
	tasks_register(&scheduler, task);
}

void setup(void) {
	struct task_t task;
	tasks_init(&scheduler);

	task_clear(&task);
	task.task = (task_fp) led,
	task.task_arg = &on,
	task.mtime = 500,
	task.flag = ONCE,
	tasks_register(&scheduler, task);

	task_clear(&task);
	task.task = toggle_led,
	task.task_arg = NULL,
	task.mtime = 300,
	task.flag = PERIODIC,
	tasks_register(&scheduler, task);

	task_clear(&task);
	task.task = toggle_led,
	task.task_arg = NULL,
	task.mtime = 350,
	task.flag = PERIODIC,
	tasks_register(&scheduler, task);

	task_clear(&task);
	task.task = idle,
	task.task_arg = NULL,
	task.mtime = 0,
	task.flag = IDLE,
	tasks_register(&scheduler, task);
}

void loop(void) {
	tasks_tick(&scheduler);
}

int main(void) {
	setup();

	while(true)
		loop();

	return 0;
}
