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

#define NOTE_C0  16.35
#define NOTE_D0  18.35
#define NOTE_E0  20.60
#define NOTE_F0  21.83
#define NOTE_G0  24.50
#define NOTE_A0  27.50
#define NOTE_B0  30.87
#define NOTE_C1  32.70
#define NOTE_D1  36.71
#define NOTE_E1  41.20
#define NOTE_F1  43.65
#define NOTE_G1  49.00
#define NOTE_A1  55.00
#define NOTE_B1  61.74
#define NOTE_C2  65.41
#define NOTE_D2  73.42
#define NOTE_E2  82.41
#define NOTE_F2  87.31
#define NOTE_G2  98.00
#define NOTE_A2  110.00
#define NOTE_B2  123.47
#define NOTE_C3  130.81
#define NOTE_D3  146.83
#define NOTE_E3  164.81
#define NOTE_F3  174.61
#define NOTE_G3  196.00
#define NOTE_A3  220.00
#define NOTE_B3  246.94
#define NOTE_C4  261.63
#define NOTE_D4  293.66
#define NOTE_E4  329.63
#define NOTE_F4  349.23
#define NOTE_G4  392.00
#define NOTE_A4  440.00
#define NOTE_B4  493.88
#define NOTE_C5  523.25
#define NOTE_D5  587.33
#define NOTE_E5  659.25
#define NOTE_F5  698.46
#define NOTE_G5  783.99
#define NOTE_A5  880.00
#define NOTE_B5  987.77
#define NOTE_C6  1046.50
#define NOTE_D6  1174.66
#define NOTE_E6  1318.51
#define NOTE_F6  1396.91
#define NOTE_G6  1567.98
#define NOTE_A6  1760.00
#define NOTE_B6  1975.53
#define NOTE_C7  2093.00
#define NOTE_D7  2349.32
#define NOTE_E7  2673.02
#define NOTE_F7  2793.83
#define NOTE_G7  3135.96
#define NOTE_A7  3520.00
#define NOTE_B7  3951.07
#define NOTE_C8  4186.01
#define NOTE_D8  4698.63
#define NOTE_E8  5274.04
#define NOTE_F8  5587.65
#define NOTE_G8  6271.93
#define NOTE_A8  7040.00
#define NOTE_B8  7902.13

/* black magic to allow for stuff like NOTE(A, OCTAVE0) === NOTE(A, 3) === NOTE_A3 */
#define GLUE(x, y, z) (x ## y ## z)

#define NOTE(letter, octave) (GLUE(NOTE_, letter, octave))
#define TEMPO (30.0 * 1000.0)
#define AMPLITUDE 0.5

#define LOW_PIN  8
#define MID_PIN  9
#define HIGH_PIN 10

#define OCTAVE_0 4
#define OCTAVE_1 5
#define OCTAVE_2 6

void music_register(struct sched_t *);
