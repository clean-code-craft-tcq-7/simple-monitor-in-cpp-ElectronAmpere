#include "./monitor.h"

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
