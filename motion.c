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
#include <constants.h>
#include <motion.h>
#include <state.h>

motion_control_t motion_control = {
    .duty = 0.0f,
    .direction = MOTION_DIR_STOP,
    .error_cm = 0,
    .active = false
};

void motion_init(void){
    motion_control.duty = 0.0f;
    motion_control.direction = MOTION_DIR_STOP;
    motion_control.error_cm = 0;
    motion_control.active = false;
}


float motion_calculate_duty(int32_t error_cm)
{
    error_cm = (error_cm < 0) ? -error_cm : error_cm;

    // 1. Wenn wir in der Deadzone sind, stoppen
    if (error_cm <= MOTION_DEADZONE_CM) {
        return 0.0f;
    }

    // 2. Proportionale Berechnung:
    // Je größer der Fehler, desto höher der Duty.
    // Beispiel: MOTION_KP = 0.01 bedeutet bei 100cm Fehler -> 1.0 (100%)
    float duty = error_cm * MOTION_KP;

    if (duty > MOTION_MAX_DUTY) {
        duty = MOTION_MAX_DUTY;
    } else if (duty < MOTION_MIN_DUTY) {
        duty = MOTION_MIN_DUTY;
    }

    return duty;
}

/*
float motion_calculate_duty(int32_t error_cm)
{
    float abs_error;
    if(error_cm > 0){
        abs_error = (float)error_cm; 
    } else {
        abs_error = (float)(-error_cm);
    }

    float duty = MOTION_KP * abs_error;

    if(duty < MOTION_MIN_DUTY){
        duty = MOTION_MIN_DUTY;
    }
    if(duty > MOTION_MAX_DUTY){
        duty = MOTION_MAX_DUTY;
    }
    if (abs_error > 2 * MOTION_DEADZONE_CM)
    {
        if (duty < MOTION_MIN_DUTY)
        duty = MOTION_MIN_DUTY;
    }
    else
    {
        if ( duty < 0.05f)
            duty = 0.0f;
    }

    return duty;
}
    
*/
    /*
motion_direction_t motion_calculate_direction(int32_t error_cm)
{
    if (error_cm >= -MOTION_DEADZONE_CM && error_cm <= MOTION_DEADZONE_CM)
        return MOTION_DIR_STOP;

    if (error_cm > 0)
        return MOTION_DIR_FORWARD;
        
    return MOTION_DIR_BACKWARD;    
}
    */

motion_direction_t motion_calculate_direction(int32_t error_cm) {
    // Wenn der Fehler innerhalb der Toleranz liegt -> Stop
    error_cm = (error_cm < 0) ? -error_cm : error_cm;
    if (error_cm <= MOTION_DEADZONE_CM) {
        return MOTION_DIR_STOP;
    }

    // Vorwärts oder Rückwärts basierend auf Vorzeichen des Fehlers
    // Hinweis: Prüfe hier, ob error > 0 "wegfahren" oder "annähern" bedeutet 
    // In diesem Beispiel: current > target -> error ist positiv -> Vorwärts fahren
    return (error_cm > 0) ? MOTION_DIR_FORWARD : MOTION_DIR_BACKWARD;
}

void motion_loop(void)
{
    if(!value.sonic.new) return;
    value.sonic.new = false;
    
    if(state.motion != MOTION_STATE_ON) {
        motion_control.duty = 0.0f;
        motion_control.direction = MOTION_DIR_STOP;
        value.motion = 0.0f; // neu
        return;
    }

    // Ziel berechnen (z.B. state.goal=2 -> 30cm)
    int32_t current_dist = (int32_t)value.sonic.distance;
    int32_t target_dist  = (int32_t)((state.goal + 1) * 10);
    
    // Fehler berechnen
    int32_t error_cm = current_dist - target_dist;
    motion_control.error_cm = error_cm;

    // Richtung bestimmen
    motion_control.direction = motion_calculate_direction(error_cm);

    // 3. Richtung und Duty bestimmen
    motion_control.direction = motion_calculate_direction(error_cm);
    
    if (motion_control.direction == MOTION_DIR_STOP) {
        motion_control.duty = 0.0f;
        value.motion = 0.0f;
    } else {
        motion_control.duty = motion_calculate_duty(error_cm);
        
        // 4. Anzeige-Wert skalieren (-100.0 bis 100.0)
        float display_value = motion_control.duty * 100.0f;
        
        if (motion_control.direction == MOTION_DIR_BACKWARD) {
            value.motion = -display_value;
        } else {
            value.motion = display_value;
        }
    }
    
