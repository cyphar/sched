/* sched: a simple scheduler written in C.
 * Copyright (C) 2015 Aleksa Sarai
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#if !defined(__TONE_H__)
#define __TONE_H__

struct tone_t {
	/* do not touch this, it is automatically set by the async_tone call */
	struct sched_t *_sched;

	/* tone data */
	int pin;
	float frequency;
	float amplitude; /* must be <= 0.9 */

	/* interval data -- relative to time of async_tone */
	long start;
	long end;
};

void tone_clear(struct tone_t *);
void async_tone(struct sched_t *, struct tone_t *);

#endif
