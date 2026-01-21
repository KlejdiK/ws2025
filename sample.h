#ifndef _SAMPLE_H_
#define _SAMPLE_H_

#include <stdint.h>
#include <stdbool.h>

#include "constants.h"

///////////////////////////////////////////////////////////////////////////////
// a sample is an integer value with a timestamp
// declared as volatile as samples are handled both in interrupts handlers and application
struct sample_t
{
    volatile int32_t    value;      // value of the sample
    volatile uint32_t   usec;       // timestamp as microsecond when sample was created
    volatile bool       valid;      // current value is a valid new one
};

///////////////////////////////////////////////////////////////////////////////
// sample_queue - buffering samples after measurement
//  - stores a few samples so an interrupt can store new samples even if sample_loop()
//      is not called for some time
//  - organized as an array where samples are removed from the left and inserted right from
//      valid samples
//  - no index for insertion an removal must be maintained by the queue (like for ring buffers)

// insert a new sample
bool sample_queue_insert(const struct sample_t sample);
// remove oldest sample
bool sample_queue_remove(struct sample_t *const sample);

///////////////////////////////////////////////////////////////////////////////
// the number of samples being stored in a sample_series
#define SAMPLE_SERIES_SIZE 8

// sample_series - timeline of samples
// - a series of samples being measured
// - the oldest sample will always be overwritten
// - organized as a ring buffer
struct sample_series_t
{
    int             current;                        // index of the most recently inserted sample
    struct sample_t sample[SAMPLE_SERIES_SIZE];     // samples organized as a ring buffer in an array
};

// data_series storing raw data samples of distance measurements
extern struct sample_series_t *distance_raw;         

// get a sample out of the sample_series
// index: from 0 (most recent sample) up to SAMPLE_SERIES_SIZE-1 (oldest sample)
struct sample_t sample_series_get(const struct sample_series_t *const series, int index);

// put a new sample into the sample series so the oldest sample will be overwritten
void sample_series_put(struct sample_series_t *const series, const struct sample_t sample);

///////////////////////////////////////////////////////////////////////////////
// sample init/loop

// initialization steps for sample handling
void sample_init(void);

// loop for regular tasks related to sample handling
void sample_loop(void);

#endif //SAMPLE_H_

