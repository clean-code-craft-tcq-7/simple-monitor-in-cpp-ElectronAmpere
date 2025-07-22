#include "./monitor.h"
#include <assert.h>
#include <thread>
#include <chrono>
#include <iostream>
#include <string>
using std::cout, std::flush, std::this_thread::sleep_for, std::chrono::seconds, std::string;

int vitalsAlert(string alertMessage) {
  cout << alertMessage;
  for (int i = 0; i < 6; i++) {
    cout << "\r* " << flush;
    sleep_for(seconds(1));
    cout << "\r *" << flush;
    sleep_for(seconds(1));
  }
  return 1;
}

int vitalTemperatureCheck(float temperature) {
  if (temperature > 102 || temperature < 95) {
    vitalsAlert("Temperature is critical!\n");
    return 0;
  }
  return 1;
}

int vitalPulseCheck(float pulseRate) {
  if (pulseRate < 60 || pulseRate > 100) {
    vitalsAlert("Pulse Rate is out of range!\n");
    return 0;
  }
  return 1;
}

int vitalOxygenCheck(float spo2) {
  if (spo2 < 90) {
    vitalsAlert("Oxygen Saturation out of range!\n");
    return 0;
  }
  return 1;
}

int vitalsOk(float temperature, float pulseRate, float spo2) {
  int result = 1;
  result &= vitalTemperatureCheck(temperature);
  result &= vitalPulseCheck(pulseRate);
  result &= vitalOxygenCheck(spo2);
  return (result);
}
