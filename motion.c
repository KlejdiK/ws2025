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

// Richtung berechnen BEVOR wir den Absolutwert nehmen
motion_direction_t motion_calculate_direction(int32_t error_cm) {
    // Wenn innerhalb der Deadzone -> Stop
    if (error_cm >= -MOTION_DEADZONE_CM && error_cm <= MOTION_DEADZONE_CM) {
        return MOTION_DIR_STOP;
    }
    
    // error_cm > 0: aktuell weiter weg als Ziel → VORWÄRTS (näher ran)
    // error_cm < 0: aktuell näher als Ziel → RÜCKWÄRTS (weg)
    return (error_cm > 0) ? MOTION_DIR_FORWARD : MOTION_DIR_BACKWARD;
}

// Duty aus Absolutwert des Fehlers berechnen
float motion_calculate_duty(int32_t error_cm)
{
    // Absolutwert bilden
    int32_t abs_error = (error_cm < 0) ? -error_cm : error_cm;
    
    // In Deadzone? → Stopp
    if (abs_error <= MOTION_DEADZONE_CM) {
        return 0.0f;
    }
    
    // Proportionale Regelung: duty = Kp * Fehler
    float duty = abs_error * MOTION_KP;
    
    // Begrenzung auf MAX
    if (duty > MOTION_MAX_DUTY) {
        duty = MOTION_MAX_DUTY;
    }
    
    // WICHTIG: Keine MIN_DUTY mehr! Das verhindert sanftes Abbremsen
    // Nur bei sehr kleinen Werten auf 0 setzen
    if (duty < 0.05f) {
        duty = 0.0f;
    }
    
    return duty;
}

void motion_loop(void)
{
    if(!value.sonic.new) return;
    value.sonic.new = false;
    
    if(state.motion != MOTION_STATE_ON) {
        motion_control.duty = 0.0f;
        motion_control.direction = MOTION_DIR_STOP;
        value.motion = 0.0f;
        return;
    }
    
    // Aktuellen Abstand und Zielabstand ermitteln
    int32_t current_dist = (int32_t)value.sonic.distance;
    int32_t target_dist  = (int32_t)((state.goal + 1) * 10);
    
    // Fehler berechnen (POSITIV = zu weit weg, NEGATIV = zu nah)
    int32_t error_cm = current_dist - target_dist;
    motion_control.error_cm = error_cm;
    
    // 1. ERST Richtung bestimmen (mit Vorzeichen!)
    motion_control.direction = motion_calculate_direction(error_cm);
    
    // 2. DANN Duty berechnen (mit Absolutwert)
    if (motion_control.direction == MOTION_DIR_STOP) {
        motion_control.duty = 0.0f;
        value.motion = 0.0f;
    } else {
        motion_control.duty = motion_calculate_duty(error_cm);
        
        // 3. Anzeige-Wert: positiv = vorwärts, negativ = rückwärts
        float display_value = motion_control.duty * 100.0f;
        
        if (motion_control.direction == MOTION_DIR_BACKWARD) {
            value.motion = -display_value;
        } else {
            value.motion = display_value;
        }
    }
}
