struct tone_t {
	/* do not touch this, it is automatically set by the async_tone call */
	struct sched_t *_sched;

	/* tone data */
	int pin;
	float frequency;
	float amplitude; /* must be <= 0.9 */

	/* interval data */
	long start;
	long end;
};

void tone_clear(struct tone_t *);
void async_tone(struct sched_t *, struct tone_t *);
