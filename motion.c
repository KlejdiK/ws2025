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
#include <state.h>
#include <drive.h>

/* PD Parameter */
#define PROPORTIONAL_GAIN  0.02f
#define DERIVATIVE_GAIN    0.05f

#define STATIC_BACKWARDS_DUTY 0.4f
#define DEFAULT_FREQ 100
#define BACKWARD_FREQ 140

/* interne Zustände */
static float last_distance_cm = 0.0f;
static float current_speed = 0.0f;   // cm pro loop

void motion_init(void)
{
    last_distance_cm = 0.0f;
    current_speed = 0.0f;
}

void motion_loop(void)
{
    if (!value.sonic.new)
        return;

    value.sonic.new = false;

    if (state.motion != MOTION_STATE_ON)
    {
        drive_duty(DEFAULT_FREQ, 0.0f);
        return;
    }

    /* aktuelle Distanz in cm */
    float current_distance = (float)value.sonic.distance;

    /* Geschwindigkeit (Diskret, reicht für D-Anteil) */
    current_speed = current_distance - last_distance_cm;
    last_distance_cm = current_distance;

    /* Ziel: goal * 10 cm */
    float target_distance = (float)(state.goal * 10);

    /* PD-Regler */
    float duty = PD_controller(target_distance, current_distance);

    /* Richtung */
    if (current_distance > target_distance)
    {
        /* vorwärts */
        direction = true;
        drive_duty(DEFAULT_FREQ, duty);
        value.motion = duty * 100.0f;
    }
    else
    {
        /* rückwärts (Notfall / Überfahren) */
        direction = false;
        drive_duty(BACKWARD_FREQ, STATIC_BACKWARDS_DUTY);
        value.motion = -STATIC_BACKWARDS_DUTY * 100.0f;
    }
}

/* ---------- PD-Regler ---------- */
float PD_controller(float target_cm, float current_cm)
{
    float error = target_cm - current_cm;

    if (error < 0)
        error = -error;

    float p = PROPORTIONAL_GAIN * error;
    float d = DERIVATIVE_GAIN * current_speed;

    float out = p - d;

    /* Begrenzen */
    if (out < 0.0f) out = 0.0f;
    if (out > 0.6f) out = 0.6f;

    return out;
}


