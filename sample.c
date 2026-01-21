#include <sample.h>

// maximum loop latency by communication via serial is measured as 45 msec
// ultrasonic sensor measures new samples with 40 Hz => 4 samples in 100 ms can be queued
#define SAMPLE_QUEUE_SIZE 4

// data structures of this module

// buffer queue for new samples
static struct sample_t sample_queue[SAMPLE_QUEUE_SIZE];
// raw data samples of distance measurements
struct sample_series_t distance_raw_data;
// set pointer as reference for using the sample_series outside of this module
struct sample_series_t *distance_raw = &distance_raw_data;

///////////////////////////////////////////////////////////////////////////////
// functions related to sample_queue

// initialize sample_queue - empty queue
static void sample_queue_init(void)
{
    // sample_queue is empty at program start
    for ( int index=0; index < SAMPLE_QUEUE_SIZE; index++ )
        { sample_queue[index].valid = false; }
}

// check whether the sample queue is empty
static bool sample_queue_empty(void)
{
    // check whether the queue is empty - no valid sample in the array
    for ( int index=0; index <SAMPLE_QUEUE_SIZE; index++ )
    {
        // if there is a valid sample the queue is not empty
        if ( sample_queue[index].valid == true ) { return false; }
    }
    
    // no valid entries found - the queue is empty
    return true;
}

// check whether the sample queue is full - a valid sample on the highest index of the queue
static bool sample_queue_full(void)
{
    return ( sample_queue[SAMPLE_QUEUE_SIZE-1].valid );
}

// find a position in the queue for inserting a sample
// position: index of the first invalid sample position after a valid sample or 0 for empty queue
// returns: true => position is found; false => queue is full
bool sample_queue_find_position(int *position)
{    
    // check whether the queue is empty
    if ( sample_queue_empty() )
    {
        *position = 0;
        return true;
    }

    // check whether the queue is full so no valid position for inserting another sample
    if ( sample_queue_full() )
    {
        *position = -1;
        return false;
    }

    // the queue is neither empty nor full:
    // look for the position in a partially filled queue where a sample can inserted after a valid sample
    int index = SAMPLE_QUEUE_SIZE-1;
    while ( true )
    {
        if ( sample_queue[index-1].valid )
        {
            *position = index;
            return true;
        }
        index--;
    }

    return false;
}

// insert a new sample (maybe called from interrupt)
bool sample_queue_insert(const struct sample_t sample)
{
    int position;

    // if there is a position for inserting a new sample
    if ( sample_queue_find_position(&position) )
    {
        sample_queue[position].value = sample.value;
        sample_queue[position].usec = sample.usec;
        sample_queue[position].valid = sample.valid;
    }
    return false;
}

// remove a sample (always called from application code)
bool sample_queue_remove(struct sample_t *const sample)
{
    // search for the leftmost valid sample
    for ( int index=0; index < SAMPLE_QUEUE_SIZE; index++)
    {
        // for a valid sample (skip invalid samples)
        if ( sample_queue[index].valid )
        {
            // copy sample data
            sample->value = sample_queue[index].value;
            sample->usec = sample_queue[index].usec;
            sample->valid = true;
            // remove sample from queue
            sample_queue[index].valid = false; 
            
            return true; // success
        }
    }

    return false; // failed
}

///////////////////////////////////////////////////////////////////////////////
// functions related to sample_series

// clear sample_series so having no samples
static void sample_series_init(struct sample_series_t *const series)
{
    // fill sample_series with invalid samples first
    series->current = 0;
    for ( int index=0; index < SAMPLE_SERIES_SIZE; index++ )
    {
        series->sample[index].value = 0;
        series->sample[index].usec = 0;
        series->sample[index].valid = false;
    }
}

// insert a sample into the specified sample_series
struct sample_t sample_series_get(const struct sample_series_t *const series, int index)
{
    struct sample_t sample;

    // interpret index as left to current sample in a ring order
    int position = ( SAMPLE_SERIES_SIZE + series->current - index ) % SAMPLE_SERIES_SIZE; 

    // copy sample an return
    sample.value = series->sample[position].value;
    sample.usec = series->sample[position].usec;
    sample.valid = series->sample[position].valid;
    return sample;
}

// insert a sample into the specified sample_series
void sample_series_put(struct sample_series_t *const series, const struct sample_t sample)
{
    // oldest sample will be overwritten
    series->current = ( series->current + 1 ) % SAMPLE_SERIES_SIZE;

    // copy sample data
    series->sample[series->current].value = sample.value;
    series->sample[series->current].usec = sample.usec;
    series->sample[series->current].valid = sample.valid;   
}

///////////////////////////////////////////////////////////////////////////////
// sample init/loop

// initialization steps for sample handling
void sample_init(void)
{
    // have an empty sample_queue
    sample_queue_init();

    // have an empty raw data sample series
    sample_series_init(distance_raw);
    return;
}

// loop for regular tasks related to sample handling
void sample_loop(void) 
{
    // if there are samples in the sample_queue
    while ( !sample_queue_empty() )
    {
        // put sample into sample_series
        struct sample_t sample;
        if ( sample_queue_remove(&sample) )
        {
            sample_series_put(distance_raw, sample);
        }
    }
    return;
}
