#pragma once

/* Range of all the vitals */
#define VITALS_TEMPERATURE_MIN_DEGF (95.0f)
#define VITALS_TEMPERATURE_MAX_DEGF (102.0f)
#define VITALS_PULSE_MIN_COUNT (60.0f)
#define VITALS_PULSE_MAX_COUNT (100.0f)
#define VTIALS_SPO2_MIN_PERCENT (90.0f)

/* Alert cycles */
#define VITALS_ALERT_MAX_CYCLE (6)
#define VITALS_ALERT_HOLD_SECONDS (1)

/* Monitors */
int vitalsOk(float temperature, float pulseRate, float spo2);
