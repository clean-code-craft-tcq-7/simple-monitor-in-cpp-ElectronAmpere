#include "./alerts.h"
#include <chrono>
#include <iostream>
#include <thread>

using std::cout, std::flush, std::this_thread::sleep_for;
using std::chrono::seconds;

static delayAlertDisplay_ptr delayAlert = &vitalAlertDelayDisplay;

/* Monitors 1.0 */
void vitalUpdateAlertDelay(delayAlertDisplay_ptr func_ptr) {
  delayAlert = func_ptr;
}

void vitalAlertDelayDisplay(long long durationInSeconds) {
  sleep_for(seconds(durationInSeconds));
}

int vitalsAlert(const std::string &alertMessage) {
  cout << alertMessage;
  for (int i = 0; i < VITALS_ALERT_MAX_CYCLE; i++) {
    cout << "\r* " << flush;
    delayAlert(VITALS_ALERT_HOLD_SECONDS);
    cout << "\r *" << flush;
    delayAlert(VITALS_ALERT_HOLD_SECONDS);
  }
  return 1;
}
