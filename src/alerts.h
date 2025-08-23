#pragma once
#include <string>

/* Alert cycles */
#define VITALS_ALERT_MAX_CYCLE (6)
#define VITALS_ALERT_HOLD_SECONDS (1)

typedef void (*delayAlertDisplay_ptr)(long long);

void vitalUpdateAlertDelay(delayAlertDisplay_ptr func_ptr);
int vitalsAlert(std::string alertMessage);
void vitalAlertDelayDisplay(long long durationInSeconds);
