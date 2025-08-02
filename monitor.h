#pragma once

/* Range of all the vitals */
#define VITALS_TEMPERATURE_MIN_DEGF (95)
#define VITALS_TEMPERATURE_MAX_DEGF (102)
#define VITALS_PULSE_MIN_COUNT (60)
#define VITALS_PULSE_MAX_COUNT (100)
#define VTIALS_SPO2_MIN_PERCENT (90)

/* Alert cycles */
#define VITALS_ALERT_MAX_CYCLE (6)
#define VITALS_ALERT_HOLD_SECONDS (1)

/* Checkers */
int vitalsAlert(string alertMessage);
int vitalTemperatureCheck(float temperature);
int vitalPulseCheck(float pulseRate);
int vitalOxygenCheck(float spo2);

/* Monitors */
int vitalsOk(float temperature, float pulseRate, float spo2);
