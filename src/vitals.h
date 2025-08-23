#pragma once
#include "./alerts.h"

/* Range of all the vitals */
#define VITALS_TEMPERATURE_MIN_DEGF (95.0f)
#define VITALS_TEMPERATURE_MAX_DEGF (102.0f)
#define VITALS_PULSE_MIN_COUNT (60.0f)
#define VITALS_PULSE_MAX_COUNT (100.0f)
#define VITALS_SPO2_MIN_PERCENT (90.0f)
#define VITALS_BLOODSUGAR_MIN (70.0f)
#define VITALS_BLOODSUGAR_MAX (110.0f)
#define VITALS_BLOODPRESSURE_MIN (90.0f)
#define VITALS_BLOODPRESSURE_MAX (150.0f)
#define VITALS_RESPIRATORYRATE_MIN (12.0f)
#define VITALS_RESPIRATORYRATE_MAX (20.0f)

/* Report Metrics */
typedef struct {
  float temperature;
  float pulseRate;
  float spo2;
  float bloodSugar;
  float bloodPressure;
  float respiratoryRate;
} Report_t;

#define VITALITY_CHECKER(vital, min, max, alertMessage)                        \
  if (!isValidFloat(vital) || vital < min || vital > max) {                    \
    vitalsAlert(alertMessage);                                                 \
    return 0;                                                                  \
  }                                                                            \
  return 1;

bool isValidFloat(float value);
int vitalTemperatureCheck(float temperature);
int vitalPulseCheck(float pulseRate);
int vitalOxygenCheck(float spo2);
int vitalBloodSugarCheck(float bloodSugar);
int vitalBloodPressureCheck(float bloodPressure);
int vitalRespiratoryRateCheck(float respiratoryRate);
