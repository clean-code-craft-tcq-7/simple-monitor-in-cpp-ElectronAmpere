#include "./monitor.h"
#include <assert.h>
#include <chrono>
#include <iostream>
#include <string>
#include <thread>

using std::cout, std::flush, std::this_thread::sleep_for;
using std::chrono::seconds, std::string;

static delayAlertDisplay_ptr delayAlert = &vitalAlertDelayDisplay;

void vitalUpdateAlertDelay(delayAlertDisplay_ptr func_ptr) {
  delayAlert = func_ptr;
}

void vitalAlertDelayDisplay(long long durationInSeconds) {
  sleep_for(seconds(durationInSeconds));
}

int vitalsAlert(string alertMessage) {
  cout << alertMessage;
  for (int i = 0; i < VITALS_ALERT_MAX_CYCLE; i++) {
    cout << "\r* " << flush;
    delayAlert(VITALS_ALERT_HOLD_SECONDS);

    cout << "\r *" << flush;
    delayAlert(VITALS_ALERT_HOLD_SECONDS);
  }
  return 1;
}

int vitalTemperatureCheck(float temperature) {
  if (temperature > VITALS_TEMPERATURE_MAX_DEGF ||
      temperature < VITALS_TEMPERATURE_MIN_DEGF) {
    vitalsAlert("Temperature is critical!\n");
    return 0;
  }
  return 1;
}

int vitalPulseCheck(float pulseRate) {
  if (pulseRate < VITALS_PULSE_MIN_COUNT ||
      pulseRate > VITALS_PULSE_MAX_COUNT) {
    vitalsAlert("Pulse Rate is out of range!\n");
    return 0;
  }
  return 1;
}

int vitalOxygenCheck(float spo2) {
  if (spo2 < VTIALS_SPO2_MIN_PERCENT) {
    vitalsAlert("Oxygen Saturation out of range!\n");
    return 0;
  }
  return 1;
}

int vitalsOk(float temperature, float pulseRate, float spo2) {
  int result = vitalTemperatureCheck(temperature);
  result &= vitalPulseCheck(pulseRate);
  result &= vitalOxygenCheck(spo2);
  return (result);
}
