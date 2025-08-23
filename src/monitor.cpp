#include "./monitor.h"

int vitalsOk(float temperature, float pulseRate, float spo2) {

  int result = vitalTemperatureCheck(temperature);
  result &= vitalPulseCheck(pulseRate);
  result &= vitalOxygenCheck(spo2);
  return (result);
}

int vitalsReportNormal(Report_t *vitalReport) {
  int result = vitalsOk(vitalReport->temperature, vitalReport->pulseRate,
                        vitalReport->spo2);
  result &= vitalBloodSugarCheck(vitalReport->bloodSugar);
  result &= vitalBloodPressureCheck(vitalReport->bloodPressure);
  result &= vitalRespiratoryRateCheck(vitalReport->respiratoryRate);

  return (result);
}
