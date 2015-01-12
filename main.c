#include <stdio.h>
#include <stdbool.h>

#include "sched.h"

struct sched_t scheduler;

/* ====================== */
/* NOTE: ARDUINO EMULATOR */
/* ====================== */

const bool on = true;
const bool off = false;

void led(void *st) {
	bool state = *(bool *) st;
	void *next = NULL;

	switch(state) {
		case true:
			next = (void *) &off;
			printf("explicit: on\n");
			break;
		case false:
			next = (void *) &on;
			printf("explicit: off\n");
			break;
	}

	struct task_t task = {
		.task = led,
		.task_arg = next,
		.callback_before = NULL,
		.callback_after = NULL,
		.callback_rm = NULL,
		.mtime = 500,
		.flag = ONCE,
	};
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

	struct task_t task = {
		.task = idle,
		.task_arg = NULL,
		.callback_before = NULL,
		.callback_after = NULL,
		.callback_rm = NULL,
		.mtime = 0,
		.flag = IDLE,
	};
	tasks_register(&scheduler, task);
}

void setup(void) {
	tasks_init(&scheduler);

	struct task_t task;

	task = (struct task_t){
		.task = led,
		.task_arg = (void *)&on,
		.callback_before = NULL,
		.callback_after = NULL,
		.callback_rm = NULL,
		.mtime = 500,
		.flag = ONCE,
	};
	tasks_register(&scheduler, task);

	task = (struct task_t){
		.task = toggle_led,
		.task_arg = NULL,
		.callback_before = NULL,
		.callback_after = NULL,
		.callback_rm = NULL,
		.mtime = 300,
		.flag = PERIODIC,
	};
	tasks_register(&scheduler, task);

	task = (struct task_t){
		.task = toggle_led,
		.task_arg = NULL,
		.callback_before = NULL,
		.callback_after = NULL,
		.callback_rm = NULL,
		.mtime = 350,
		.flag = PERIODIC,
	};
	tasks_register(&scheduler, task);

	task = (struct task_t){
		.task = idle,
		.task_arg = NULL,
		.callback_before = NULL,
		.callback_after = NULL,
		.callback_rm = NULL,
		.mtime = 0,
		.flag = IDLE,
	};

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
