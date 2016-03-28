/* sched: a simple scheduler written in C.
 * Copyright (C) 2015 Aleksa Sarai
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "async.h"
#include "sched.h"

struct sched_t scheduler;

/* ====================== */
/* NOTE: ARDUINO EMULATOR */
/* ====================== */



void setup(void) {
	sched_init(&scheduler);

	struct tone_t *tone440 = malloc(sizeof(struct tone_t));
	tone_clear(tone440);
	tone440->pin = 13;
	tone440->amplitude = 0.5;
	tone440->frequency = 440;
	tone440->start = 0;
	tone440->end = 3000;
	async_tone(&scheduler, tone440);

	struct tone_t *tone880 = malloc(sizeof(struct tone_t));
	tone_clear(tone880);
	tone880->pin = 13;
	tone880->amplitude = 0.6;
	tone880->frequency = 880;
	tone880->start = 1000;
	tone880->end = 2000;
	async_tone(&scheduler, tone880);
}

void loop(void) {
	sched_tick(&scheduler);
}

int main(void) {
	setup();

	while(true)
		loop();

	return 0;
}
