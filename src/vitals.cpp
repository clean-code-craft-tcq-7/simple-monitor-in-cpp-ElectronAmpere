#include "./vitals.h"
#include "./alerts.h"
#include <cmath>

bool isValidFloat(float value) {
  return !std::isnan(value) && !std::isinf(value);
}

/* Vitals 1.0 */
int vitalTemperatureCheck(float temperature) {
  VITALITY_CHECKER(temperature, VITALS_TEMPERATURE_MIN_DEGF,
                   VITALS_TEMPERATURE_MAX_DEGF, "Temperature is critical!\n");
}

int vitalPulseCheck(float pulseRate) {
  VITALITY_CHECKER(pulseRate, VITALS_PULSE_MIN_COUNT, VITALS_PULSE_MAX_COUNT,
                   "Pulse Rate is out of range!\n");
}

int vitalOxygenCheck(float spo2) {
  if (!isValidFloat(spo2) || spo2 < VITALS_SPO2_MIN_PERCENT) {
    vitalsAlert("Oxygen Saturation out of range!\n");
    return 0;
  }
  return 1;
}

/* Vitals 2.0 */
int vitalBloodSugarCheck(float bloodSugar) {
  VITALITY_CHECKER(bloodSugar, VITALS_BLOODSUGAR_MIN, VITALS_BLOODSUGAR_MAX,
                   "Blood Sugar is out of range!\n");
}

int vitalBloodPressureCheck(float bloodPressure) {
  VITALITY_CHECKER(bloodPressure, VITALS_BLOODPRESSURE_MIN,
                   VITALS_BLOODPRESSURE_MAX,
                   "Blood Pressure is out of range!\n");
}

int vitalRespiratoryRateCheck(float respiratoryRate) {
  VITALITY_CHECKER(respiratoryRate, VITALS_RESPIRATORYRATE_MIN,
                   VITALS_RESPIRATORYRATE_MAX,
                   "Respiratory Rate is out of range!\n");
}
