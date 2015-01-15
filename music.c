/* sched: a simple scheduler written in C.
 * Copyright (C) 2015 Cyphar
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * 1. The above copyright notice and this permission notice shall be included in
 *    all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include <stdio.h>
#include <stdlib.h>

#include "sched.h"
#include "async.h"
#include "music.h"

#define NUM_CHORDS 3
#define SONG_LENGTH 38

struct tone_t CHORDS[NUM_CHORDS][SONG_LENGTH] = {
	/* Highs. */
	{
		{NULL, HIGH_PIN, -1.0,              AMPLITUDE, 0, TEMPO * 1.0},
		{NULL, HIGH_PIN, NOTE(C, OCTAVE_2), AMPLITUDE, 0, TEMPO * 0.25},
		{NULL, HIGH_PIN, -1.0,              AMPLITUDE, 0, TEMPO * 0.25},
		{NULL, HIGH_PIN, NOTE(C, OCTAVE_2), AMPLITUDE, 0, TEMPO * 1.0},
		{NULL, HIGH_PIN, NOTE(F, OCTAVE_1), AMPLITUDE, 0, TEMPO * 1.0},

		{NULL, HIGH_PIN, -1.0,              AMPLITUDE, 0, TEMPO * 1.0},
		{NULL, HIGH_PIN, NOTE(D, OCTAVE_2), AMPLITUDE, 0, TEMPO * 0.25},
		{NULL, HIGH_PIN, -1.0,              AMPLITUDE, 0, TEMPO * 0.25},
		{NULL, HIGH_PIN, NOTE(D, OCTAVE_2), AMPLITUDE, 0, TEMPO * 0.5},
		{NULL, HIGH_PIN, -1.0,              AMPLITUDE, 0, TEMPO * 0.5},
		{NULL, HIGH_PIN, NOTE(B, OCTAVE_1), AMPLITUDE, 0, TEMPO * 1.0},

		{NULL, HIGH_PIN, -1.0,              AMPLITUDE, 0, TEMPO * 1.0},
		{NULL, HIGH_PIN, NOTE(D, OCTAVE_2), AMPLITUDE, 0, TEMPO * 0.25},
		{NULL, HIGH_PIN, -1.0,              AMPLITUDE, 0, TEMPO * 0.25},
		{NULL, HIGH_PIN, NOTE(D, OCTAVE_2), AMPLITUDE, 0, TEMPO * 1.0},
		{NULL, HIGH_PIN, NOTE(F, OCTAVE_1), AMPLITUDE, 0, TEMPO * 1.0},
		{NULL, HIGH_PIN, -1.0,              AMPLITUDE, 0, TEMPO * 1.0},
		{NULL, HIGH_PIN, NOTE(B, OCTAVE_1), AMPLITUDE, 0, TEMPO * 0.25},

		{NULL, HIGH_PIN, -1.0,              AMPLITUDE, 0, TEMPO * 0.5},
		{NULL, HIGH_PIN, NOTE(D, OCTAVE_1), AMPLITUDE, 0, TEMPO * 0.5},
		{NULL, HIGH_PIN, -1.0,              AMPLITUDE, 0, TEMPO * 0.5},
		{NULL, HIGH_PIN, -1.0,              AMPLITUDE, 0, TEMPO * 0.5},
		{NULL, HIGH_PIN, -1.0,              AMPLITUDE, 0, TEMPO * 0.5},
		{NULL, HIGH_PIN, NOTE(A, OCTAVE_1), AMPLITUDE, 0, TEMPO * 1.5},

		{NULL, HIGH_PIN, -1.0,              AMPLITUDE, 0, TEMPO * 0.25},
		{NULL, HIGH_PIN, -1.0,              AMPLITUDE, 0, TEMPO * 0.25},
		{NULL, HIGH_PIN, NOTE(C, OCTAVE_2), AMPLITUDE, 0, TEMPO * 0.5},
		{NULL, HIGH_PIN, -1.0,              AMPLITUDE, 0, TEMPO * 0.5},
		{NULL, HIGH_PIN, -1.0,              AMPLITUDE, 0, TEMPO * 0.5},
		{NULL, HIGH_PIN, -1.0,              AMPLITUDE, 0, TEMPO * 0.5},
		{NULL, HIGH_PIN, NOTE(F, OCTAVE_1), AMPLITUDE, 0, TEMPO * 1.0},
		{NULL, HIGH_PIN, NOTE(D, OCTAVE_2), AMPLITUDE, 0, TEMPO * 1.0},
		{NULL, HIGH_PIN, NOTE(C, OCTAVE_2), AMPLITUDE, 0, TEMPO * 3.0},

		{NULL, HIGH_PIN, -1.0,              AMPLITUDE, 0, TEMPO * 0.25},
		{NULL, HIGH_PIN, -1.0,              AMPLITUDE, 0, TEMPO * 0.25},
		{NULL, HIGH_PIN, -1.0,              AMPLITUDE, 0, TEMPO * 0.25},
		{NULL, HIGH_PIN, -1.0,              AMPLITUDE, 0, TEMPO * 0.25},
		{NULL, HIGH_PIN, NOTE(C, OCTAVE_2), AMPLITUDE, 0, TEMPO * 4.0},
	},

	/* Mids. */
	{
		{NULL, MID_PIN, NOTE(A, OCTAVE_0), AMPLITUDE, 0, TEMPO * 1.0},
		{NULL, MID_PIN, NOTE(A, OCTAVE_1), AMPLITUDE, 0, TEMPO * 0.25},
		{NULL, MID_PIN, NOTE(B, OCTAVE_1), AMPLITUDE, 0, TEMPO * 0.25},
		{NULL, MID_PIN, NOTE(A, OCTAVE_1), AMPLITUDE, 0, TEMPO * 1.0},
		{NULL, MID_PIN, NOTE(E, OCTAVE_1), AMPLITUDE, 0, TEMPO * 1.0},

		{NULL, MID_PIN, NOTE(A, OCTAVE_0), AMPLITUDE, 0, TEMPO * 1.0},
		{NULL, MID_PIN, NOTE(A, OCTAVE_1), AMPLITUDE, 0, TEMPO * 0.25},
		{NULL, MID_PIN, NOTE(C, OCTAVE_2), AMPLITUDE, 0, TEMPO * 0.25},
		{NULL, MID_PIN, NOTE(A, OCTAVE_1), AMPLITUDE, 0, TEMPO * 0.5},
		{NULL, MID_PIN, NOTE(C, OCTAVE_2), AMPLITUDE, 0, TEMPO * 0.5},
		{NULL, MID_PIN, NOTE(F, OCTAVE_1), AMPLITUDE, 0, TEMPO * 1.0},

		{NULL, MID_PIN, NOTE(D, OCTAVE_1), AMPLITUDE, 0, TEMPO * 1.0},
		{NULL, MID_PIN, NOTE(B, OCTAVE_1), AMPLITUDE, 0, TEMPO * 0.25},
		{NULL, MID_PIN, NOTE(C, OCTAVE_1), AMPLITUDE, 0, TEMPO * 0.25},
		{NULL, MID_PIN, NOTE(B, OCTAVE_1), AMPLITUDE, 0, TEMPO * 1.0},
		{NULL, MID_PIN, NOTE(D, OCTAVE_1), AMPLITUDE, 0, TEMPO * 1.0},
		{NULL, MID_PIN, NOTE(E, OCTAVE_0), AMPLITUDE, 0, TEMPO * 1.0},
		{NULL, MID_PIN, NOTE(G, OCTAVE_1), AMPLITUDE, 0, TEMPO * 0.25},

		{NULL, MID_PIN, NOTE(A, OCTAVE_1), AMPLITUDE, 0, TEMPO * 0.5},
		{NULL, MID_PIN, NOTE(G, OCTAVE_1), AMPLITUDE, 0, TEMPO * 0.5},
		{NULL, MID_PIN, NOTE(A, OCTAVE_1), AMPLITUDE, 0, TEMPO * 0.5},
		{NULL, MID_PIN, NOTE(G, OCTAVE_1), AMPLITUDE, 0, TEMPO * 0.5},
		{NULL, MID_PIN, NOTE(A, OCTAVE_1), AMPLITUDE, 0, TEMPO * 0.5},
		{NULL, MID_PIN, NOTE(F, OCTAVE_1), AMPLITUDE, 0, TEMPO * 1.5},

		{NULL, MID_PIN, NOTE(G, OCTAVE_1), AMPLITUDE, 0, TEMPO * 0.25},
		{NULL, MID_PIN, NOTE(A, OCTAVE_1), AMPLITUDE, 0, TEMPO * 0.25},
		{NULL, MID_PIN, NOTE(A, OCTAVE_1), AMPLITUDE, 0, TEMPO * 0.5},
		{NULL, MID_PIN, NOTE(B, OCTAVE_1), AMPLITUDE, 0, TEMPO * 0.5},
		{NULL, MID_PIN, NOTE(A, OCTAVE_1), AMPLITUDE, 0, TEMPO * 0.5},
		{NULL, MID_PIN, NOTE(G, OCTAVE_1), AMPLITUDE, 0, TEMPO * 0.5},
		{NULL, MID_PIN, NOTE(D, OCTAVE_1), AMPLITUDE, 0, TEMPO * 1.0},
		{NULL, MID_PIN, NOTE(A, OCTAVE_1), AMPLITUDE, 0, TEMPO * 1.0},
		{NULL, MID_PIN, NOTE(G, OCTAVE_1), AMPLITUDE, 0, TEMPO * 3.0},

		{NULL, MID_PIN, NOTE(C, OCTAVE_2), AMPLITUDE, 0, TEMPO * 0.25},
		{NULL, MID_PIN, NOTE(D, OCTAVE_2), AMPLITUDE, 0, TEMPO * 0.25},
		{NULL, MID_PIN, NOTE(C, OCTAVE_2), AMPLITUDE, 0, TEMPO * 0.25},
		{NULL, MID_PIN, NOTE(B, OCTAVE_1), AMPLITUDE, 0, TEMPO * 0.25},
		{NULL, MID_PIN, NOTE(G, OCTAVE_1), AMPLITUDE, 0, TEMPO * 4.0},
	},

	/* Lows. */
	{
		{NULL, LOW_PIN, -1.0,              AMPLITUDE, 0, TEMPO * 1.0},
		{NULL, LOW_PIN, NOTE(F, OCTAVE_1), AMPLITUDE, 0, TEMPO * 0.25},
		{NULL, LOW_PIN, -1.0,              AMPLITUDE, 0, TEMPO * 0.25},
		{NULL, LOW_PIN, NOTE(F, OCTAVE_1), AMPLITUDE, 0, TEMPO * 1.0},
		{NULL, LOW_PIN, NOTE(C, OCTAVE_1), AMPLITUDE, 0, TEMPO * 1.0},

		{NULL, LOW_PIN, -1.0,              AMPLITUDE, 0, TEMPO * 1.0},
		{NULL, LOW_PIN, NOTE(F, OCTAVE_1), AMPLITUDE, 0, TEMPO * 0.25},
		{NULL, LOW_PIN, -1.0,              AMPLITUDE, 0, TEMPO * 0.25},
		{NULL, LOW_PIN, NOTE(F, OCTAVE_1), AMPLITUDE, 0, TEMPO * 0.5},
		{NULL, LOW_PIN, -1.0,              AMPLITUDE, 0, TEMPO * 0.5},
		{NULL, LOW_PIN, NOTE(D, OCTAVE_1), AMPLITUDE, 0, TEMPO * 1.0},

		{NULL, LOW_PIN, -1.0,              AMPLITUDE, 0, TEMPO * 1.0},
		{NULL, LOW_PIN, NOTE(F, OCTAVE_1), AMPLITUDE, 0, TEMPO * 0.25},
		{NULL, LOW_PIN, -1.0,              AMPLITUDE, 0, TEMPO * 0.25},
		{NULL, LOW_PIN, NOTE(F, OCTAVE_1), AMPLITUDE, 0, TEMPO * 1.0},
		{NULL, LOW_PIN, NOTE(B, OCTAVE_0), AMPLITUDE, 0, TEMPO * 1.0},
		{NULL, LOW_PIN, -1.0,              AMPLITUDE, 0, TEMPO * 1.0},
		{NULL, LOW_PIN, NOTE(E, OCTAVE_1), AMPLITUDE, 0, TEMPO * 0.25},

		{NULL, LOW_PIN, -1.0,              AMPLITUDE, 0, TEMPO * 0.5},
		{NULL, LOW_PIN, NOTE(E, OCTAVE_1), AMPLITUDE, 0, TEMPO * 0.5},
		{NULL, LOW_PIN, -1.0,              AMPLITUDE, 0, TEMPO * 0.5},
		{NULL, LOW_PIN, NOTE(E, OCTAVE_1), AMPLITUDE, 0, TEMPO * 0.5},
		{NULL, LOW_PIN, -1.0,              AMPLITUDE, 0, TEMPO * 0.5},
		{NULL, LOW_PIN, NOTE(C, OCTAVE_1), AMPLITUDE, 0, TEMPO * 1.5},

		{NULL, LOW_PIN, -1.0,              AMPLITUDE, 0, TEMPO * 0.25},
		{NULL, LOW_PIN, -1.0,              AMPLITUDE, 0, TEMPO * 0.25},
		{NULL, LOW_PIN, NOTE(E, OCTAVE_1), AMPLITUDE, 0, TEMPO * 0.5},
		{NULL, LOW_PIN, -1.0,              AMPLITUDE, 0, TEMPO * 0.5},
		{NULL, LOW_PIN, -1.0,              AMPLITUDE, 0, TEMPO * 0.5},
		{NULL, LOW_PIN, -1.0,              AMPLITUDE, 0, TEMPO * 0.5},
		{NULL, LOW_PIN, -1.0,              AMPLITUDE, 0, TEMPO * 1.0},
		{NULL, LOW_PIN, NOTE(F, OCTAVE_1), AMPLITUDE, 0, TEMPO * 1.0},
		{NULL, LOW_PIN, NOTE(F, OCTAVE_1), AMPLITUDE, 0, TEMPO * 3.0},

		{NULL, LOW_PIN, -1.0,              AMPLITUDE, 0, TEMPO * 0.25},
		{NULL, LOW_PIN, -1.0,              AMPLITUDE, 0, TEMPO * 0.25},
		{NULL, LOW_PIN, -1.0,              AMPLITUDE, 0, TEMPO * 0.25},
		{NULL, LOW_PIN, -1.0,              AMPLITUDE, 0, TEMPO * 0.25},
		{NULL, LOW_PIN, NOTE(E, OCTAVE_1), AMPLITUDE, 0, TEMPO * 4.0},
	},
};

void music_register(struct sched_t *sched) {
	int i;
	for(i = 0; i < NUM_CHORDS; i++) {
		int j;
		long offset = 0;
		for(j = 0; j < SONG_LENGTH; j++) {
			/* get pointer to tone and length value */
			struct tone_t *tone = &CHORDS[i][j];
			long length = tone->end;

			/* update the length data stored in the tone */
			tone->start += offset;
			tone->end += offset;

			/* tones with a frequency <0 are "placeholders" to align the timing */
			if(tone->frequency > 0.0)
				async_tone(sched, tone);

			/* update the offset */
			offset += length;
		}
	}
}
