/*
 * Copyright (c) 2025 Hochschule Darmstadt
 *
 * This software is provided exclusively for the practical part of the
 * course 'Embedded Systems' at 'Darmstadt University of Applied Sciences
 * (Hochschule Darmstadt, h_da)'.
 *
 * Any use, distribution, or modification outside this context is prohibited
 * without explicit permission.
 *
 * AUTHORS: David Heiß, Manfred Pester, Jens-Peter Akelbein
 * FILE: motion.c
 * CONTENTS: implementation for all functions of motion module
 */

 /*
#include <motion.h>
#include <state.h>

motion_control_t motion_control;

// PID interne Zustände 
static float integral = 0.0f;
static int32_t last_error = 0;

void motion_init(void)
{
    motion_control.duty = 0.0f;
    motion_control.direction = MOTION_DIR_STOP;
    motion_control.error_cm = 0;
    motion_control.active = false;

    integral = 0.0f;
    last_error = 0;
}

void motion_loop(void)
{
    int32_t dist_cm = (int32_t)value.sonic.distance; 
    int32_t goal_cm = (int32_t)((state.goal+1) * 10); 
    int32_t error = dist_cm - goal_cm; 
    motion_control.error_cm = error;


    int32_t abs_error = (error >= 0) ? error : -error;


// Deadzone
if (abs_error <= MOTION_DEADZONE_CM)
{
    motion_control.direction = MOTION_DIR_STOP;
    motion_control.duty = 0.0f;
    motion_control.active = false;
    value.motion = 0.0f;
    integral = 0.0f;
    last_error = 0;
    return;
}

//Richtung
motion_control.direction =
    (error > 0) ? MOTION_DIR_FORWARD : MOTION_DIR_BACKWARD;

// PID nur nahe Ziel
int32_t pid_error = error;
if (pid_error >  40) pid_error =  40;
if (pid_error < -40) pid_error = -40;

// Richtungswechsel → Integral reset
if ((pid_error > 0 && last_error < 0) ||
    (pid_error < 0 && last_error > 0))
{
    integral = 0.0f;
}

float p = MOTION_KP * pid_error;

integral += pid_error;
if (integral > 200) integral = 200;
if (integral < -200) integral = -200;
float i = MOTION_KI * integral;

float d = MOTION_KD * (pid_error - last_error);
last_error = pid_error;
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


float output = p + i + d;
if (output < 0) output = -output;

if (output > MOTION_MAX_DUTY) output = MOTION_MAX_DUTY;

motion_control.duty = output;
motion_control.active = true;

value.motion =
    (motion_control.direction == MOTION_DIR_FORWARD)
    ? output * 100.0f
    : -output * 100.0f;
}
*/

/*
 * Copyright (c) 2025 Hochschule Darmstadt
 *
 * This software is provided exclusively for the practical part of the
 * course 'Embedded Systems' at 'Darmstadt University of Applied Sciences
 * (Hochschule Darmstadt, h_da)'.
 *
 * Any use, distribution, or modification outside this context is prohibited
 * without explicit permission.
 *
 * AUTHORS: David Heiß, Manfred Pester, Jens-Peter Akelbein
 * FILE: motion.c
 * CONTENTS: implementation for all functions of motion module
 */

#include <motion.h>
//#include <sample.h>
#include <state.h>
#include <sam.h>
#include <drive.h>

#define DUTY_CYCLE_FREQUENCY 100

// Proportional Term Coefficient
#define PROPORTIONAL_GAIN 0.0017 

// Derivative Term Coefficient
#define DERIVATIVE_GAIN 0.007

#define DISTURBANCE_OFFSET 0.0

// Last speed in loop (in mm/sec)
static float current_speed = 0.0;

#define STATIC_BACKWARDS_DUTYCYCLE 0.4

int frequency = 100;

// Initialization steps for motion specific
void motion_init(void) {
    // Any necessary initialization code for the motion module
}

// Loop for regular tasks related to motion functions
void motion_loop(void) {
    // Handle state changes
    switch (state.motion) {
        case MOTION_STATE_ON: // Enable the PWM output on the ENA pin (PIOC25)
            PIOC_PDR = (1 << 25);
            break;
        case MOTION_STATE_OFF: // Disable PIO control for peripheral use
            PIOC_PER = (1 << 25);
            break;
        case MOTION_STATE_INVALID:
        default:
            break;
    }

    // Get the current and last samples
    struct sample_t current_sample = sample_series_get(distance_raw, 0);
    struct sample_t last_sample = sample_series_get(distance_raw, 1);

    // Calculate the change in distance and time
    float delta_distance = (current_sample.value - last_sample.value) / 1000.0; // convert mm to meters
    float delta_time = (current_sample.usec - last_sample.usec) / 1000.0; // convert usec to msec

    // Calculate the current speed
    if (delta_time > 0) {
        current_speed = delta_distance / delta_time;
    }

    // Calculate duty using the PD controller
    float duty_cycle = PD_controller(gap_target, current_sample.value / 1000.0); // convert mm to meters

    if(gap_target > current_sample.value / 1000.0){
        direction = false;
    }else{
        direction = true; 
    }

    if(direction == false){
        duty_cycle = STATIC_BACKWARDS_DUTYCYCLE;
        frequency = 140;
    }

    drive_duty(frequency, duty_cycle);

}

// PD controller function
float PD_controller(float target_distance, float current_distance) {
    float error = target_distance - current_distance;

    // Ensure error is positive
    if (error < 0) {
        error = -error;
    }

    // Proportional part
    float proportional_term = PROPORTIONAL_GAIN * error;

    // Derivative part
    float derivative_term = DERIVATIVE_GAIN * current_speed;

    // PD combined result
    float pd_total = proportional_term - derivative_term;

    // Add disturbances
    float pd_with_disturbance = pd_total + DISTURBANCE_OFFSET;

    return pd_with_disturbance;
}

