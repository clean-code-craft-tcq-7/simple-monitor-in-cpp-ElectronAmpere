#include "./monitor.h"
#include "vitals.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int monitorVitalsStatus(float temperature, float pulseRate, float spo2) {

  int result = vitalTemperatureCheck(temperature);
  result &= vitalPulseCheck(pulseRate);
  result &= vitalOxygenCheck(spo2);
  return (result);
}

int monitorVitalsReportStatus(const Report_t *vitalReport) {
  int result = monitorVitalsStatus(vitalReport->temperature,
                                   vitalReport->pulseRate, vitalReport->spo2);
  result &= vitalBloodSugarCheck(vitalReport->bloodSugar);
  result &= vitalBloodPressureCheck(vitalReport->bloodPressure);
  result &= vitalRespiratoryRateCheck(vitalReport->respiratoryRate);

  return (result);
}

char *processVital(vitalsConfig_t *config, vitalsHandler_t *handle) {
  if (!config || !handle) {
    return strdup("Error: Invalid vital configuration or handler");
  }

  // Calculate tolerance and warning thresholds
  calculateTolerance(config, handle);

  // Convert reported value to base unit
  convertToBaseUnit(config, handle);

  // Check for breaches or warnings
  handle->breachType = checkVitalBreach(handle);

  // Return formatted status message
  char *buffer = (char *)malloc(256);
  if (!buffer) {
    return nullptr;
  }
  snprintf(buffer, 256,
           "Vital: %s\nReported: %.2f %s\nBase Value: %.2f %s\nStatus: %s",
           handle->name, handle->report_value, handle->report_unit,
           handle->base_value, config->base_unit, getBreachMessage(handle));
  return buffer;
}
