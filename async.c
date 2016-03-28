/* sched: a simple scheduler written in C.
 * Copyright (C) 2015 Aleksa Sarai
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */


#include <stdio.h>
#include <sys/time.h>

#if defined(__ARDUINO__)
#	include <Arduino.h>
#endif

#include "sched.h"

struct tone_t {
	struct sched_t *_sched;

	int pin;
	float frequency;
	float amplitude; /* must be <= 0.9 */

	long start;
	long end;
};

void tone_clear(struct tone_t *tone) {
	tone->_sched = NULL;
	tone->pin = 0;
	tone->frequency = 0.0;
	tone->amplitude = 0.0;
	tone->start = 0;
	tone->end = 0;
}

static long __async_time(void) {
#if defined(__ARDUINO__)
	/* just use the arduino epoch */
	return millis();
#else
	/* for use on POSIX machines */
	struct timeval tv;
	gettimeofday(&tv, NULL);

	/* convert to milliseconds */
	return (tv.tv_sec * 1000) + (tv.tv_usec / 1000);
#endif
}

static int __to_mperiod(long freq) {
	return 1000.0 / (float) freq;
}

/* declarations */
static void __async_tone_on(struct tone_t *);
static void __async_tone_off(struct tone_t *);

static void __tone_on(struct tone_t *tone) {
#if defined(__ARDUINO__)
	/* Arduino magic. */
	digitalWrite(tone->pin, HIGH);
#endif
}

static void __async_tone_on(struct tone_t *tone) {
	long now = __async_time();

	/* re-schedule a tone at the correct offset when it starts prematurely */
	if(now < tone->start) {
		struct task_t task;
		task_clear(&task);
		task.task = (task_fp) __async_tone_on;
		task.task_arg = tone;
		task.mtime = tone->start - now;
		task.flag = ONCE;
		sched_register(tone->_sched, task);
		return;
	}

	/* do not run the on if the tone has ended or register the next task */
	if(now >= tone->end)
		return;

	/* calculate the period and duty cycle time for the tone */
	int mperiod = __to_mperiod(tone->frequency);
	int mduty = tone->amplitude * (float) mperiod;

	/* turn on the pin */
	__tone_on(tone);

	/* register tone_off task */
	struct task_t task;
	task_clear(&task);
	task.task = (task_fp) __async_tone_off;
	task.task_arg = tone;
	task.mtime = mduty;
	task.flag = ONCE;
	sched_register(tone->_sched, task);
}

static void __tone_off(struct tone_t *tone) {
#if defined(__ARDUINO__)
	/* Arduino magic. */
	digitalWrite(tone->pin, LOW);
#endif
}

static void __async_tone_off(struct tone_t *tone) {
	long now = __async_time();

	/* calculate the period and duty cycle time for the tone */
	int mperiod = __to_mperiod(tone->frequency);
	int mduty = (1.0 - tone->amplitude) * (float) mperiod;

	/* turn on the pin */
	__tone_off(tone);

	/* only schedule the next tone oscillation if the tone hasn't ended yet */
	if(now <= tone->end) {
		struct task_t task;
		task_clear(&task);
		task.task = (task_fp) __async_tone_on;
		task.task_arg = tone;
		task.mtime = mduty;
		task.flag = ONCE;
		sched_register(tone->_sched, task);
		return;
	}
}

void async_tone(struct sched_t *sched, struct tone_t *tone) {
	/* adjust start and end by time */
	long now = __async_time();
	tone->start += now;
	tone->end += now;

	/* set scheduler */
	tone->_sched = sched;

	/* set up task struct */
	struct task_t task;
	task_clear(&task);
	task.task = (task_fp) __async_tone_on;
	task.task_arg = tone;
	task.mtime = tone->start;
	task.flag = ONCE;
	sched_register(sched, task);
}
