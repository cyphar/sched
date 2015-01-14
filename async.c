#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

#include "sched.h"

struct tone_t {
	struct sched_t *_sched;

	int pin;
	float frequency;
	float amplitude; /* must be <= 0.9 */

	long _start;
	long _end;
};

static void __tone_clear(struct tone_t *tone) {
	tone->_sched = NULL;
	tone->pin = 0;
	tone->frequency = 0.0;
	tone->amplitude = 0.0;
	tone->_start = 0;
	tone->_end = 0;
}

static long __async_time(void) {
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

static int __to_mperiod(long freq) {
	return 1000.0 / (float) freq;
}

/* declarations */
static void __async_tone_on(struct tone_t *);
static void __async_tone_off(struct tone_t *);

static void __tone_on(struct tone_t *tone) {
#if 0
	/* Arduino magic. */
	digitalWrite(tone->pin, HIGH);
#endif
	fprintf(stderr, "turned pin %d to HIGH.\n", tone->pin);
}

static void __async_tone_on(struct tone_t *tone) {
	long now = __async_time();

	/* re-schedule a tone at the correct offset when it starts prematurely */
	if(now < tone->_start) {
		struct task_t task;
		task_clear(&task);
		task.task = (void (*)(void *)) __async_tone_on;
		task.task_arg = tone;
		task.mtime = tone->_start - now;
		task.flag = ONCE;
		sched_register(tone->_sched, task);
		return;
	}

	/* do not run the on if the tone has ended or register the next task */
	if(now >= tone->_end) {
		return;
	}

	/* calculate the period and duty cycle time for the tone */
	int mperiod = __to_mperiod(tone->frequency);
	int mduty = tone->amplitude * (float) mperiod;

	/* turn on the pin */
	__tone_on(tone);

	/* register tone_off task */
	struct task_t task;
	task_clear(&task);
	task.task = (void (*)(void *)) __async_tone_off;
	task.task_arg = tone;
	task.mtime = mduty;
	task.flag = ONCE;
	sched_register(tone->_sched, task);
}

static void __tone_off(struct tone_t *tone) {
#if 0
	/* Arduino magic. */
	digitalWrite(tone->pin, LOW);
#endif
	fprintf(stderr, "turned pin %d to LOW.\n", tone->pin);
}

static void __async_tone_off(struct tone_t *tone) {
	long now = __async_time();

	/* calculate the period and duty cycle time for the tone */
	int mperiod = __to_mperiod(tone->frequency);
	int mduty = (1.0 - tone->amplitude) * (float) mperiod;

	/* turn on the pin */
	__tone_off(tone);

	/* only schedule the next tone oscillation if the tone hasn't ended yet */
	if(now < tone->_end) {
		struct task_t task;
		task_clear(&task);
		task.task = (void (*)(void *)) __async_tone_on;
		task.task_arg = tone;
		task.mtime = mduty;
		task.flag = ONCE;
		sched_register(tone->_sched, task);
		return;
	}
}

/* XXX(cyphar): decide to use global or malloc'd. */

void async_tone_offset(struct sched_t *sched, int pin, float frequency, float amplitude, long length, long offset) {
	/* set up tone struct */
	struct tone_t *tone = malloc(sizeof(struct tone_t));
	tone->_sched = sched;
	tone->pin = pin;
	tone->frequency = frequency;
	tone->amplitude = amplitude;
	tone->_start = __async_time() + offset;
	tone->_end = tone->_start + length;

	/* set up task struct */
	struct task_t task;
	task_clear(&task);
	task.task = (void (*)(void *)) __async_tone_on;
	task.task_arg = tone;
	task.mtime = offset;
	task.flag = ONCE;
	sched_register(sched, task);
}

void async_tone(struct sched_t *sched, int pin, float frequency, float amplitude, long length) {
	async_tone_offset(sched, pin, frequency, amplitude, length, 0);
}
