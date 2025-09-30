#pragma once
#include <string>

/* Alert cycles */
#define VITALS_ALERT_MAX_CYCLE (6)
#define VITALS_ALERT_HOLD_SECONDS (1)

typedef void (*delayAlertDisplay_ptr)(long long);

#define TEMPERATURE_ALERT_ENG ("Temperature is critical!\n")
#define TEMPERATURE_ALERT_DE ("Die Temperatur ist entscheidend!\n")

#define PULSE_ALERT_ENG ("Pulse Rate is out of range!\n")
#define PULSE_ALERT_DE                                                         \
  ("Pulsfrequenz liegt außerhalb des zulässigen Bereichs!\n")

#define SPO2_ALERT_ENG ("Oxygen Saturation out of range!\n")
#define SPO2_ALERT_DE                                                          \
  ("Sauerstoffsättigung außerhalb des zulässigen Bereichs!\n")

#define BLOODSUGAR_ALERT_ENG ("Blood Sugar is out of range!\n")
#define BLOODSUGAR_ALERT_DE ("Blutzucker liegt außerhalb des Bereichs!\n")

#define BLOODPRESSURE_ALERT_ENG ("Blood Pressure is out of range!\n")
#define BLOODPRESSURE_ALERT_DE                                                 \
  ("Der Blutdruck liegt außerhalb des zulässigen Bereichs!\n")

#define RESPIRATORYRATE_ALERT_ENG ("Respiratory Rate is out of range!\n")
#define RESPIRATORYRATE_ALERT_DE                                               \
  ("Atemfrequenz liegt außerhalb des zulässigen Bereichs!\n")

#define ALERT_IN_ENGLISH (0)
#define ALERT_IN_GERMAN (1)
#define ALERT_LANG (ALERT_IN_ENGLISH)

#if (ALERT_LANG == ALERT_IN_ENGLISH)
#define TEMPERATURE_ALERT (PULSE_ALERT_ENG)
#define PULSE_ALERT (PULSE_ALERT_ENG)
#define SPO2_ALERT (SPO2_ALERT_ENG)
#define BLOODSUGAR_ALERT (BLOODSUGAR_ALERT_ENG)
#define BLOODPRESSURE_ALERT (BLOODPRESSURE_ALERT_ENG)
#define RESPIRATORYRATE_ALERT (RESPIRATORYRATE_ALERT_ENG)
#else
#define TEMPERATURE_ALERT (PULSE_ALERT_DE)
#define PULSE_ALERT (PULSE_ALERT_DE)
#define SPO2_ALERT (SPO2_ALERT_DE)
#define BLOODSUGAR_ALERT (BLOODSUGAR_ALERT_DE)
#define BLOODPRESSURE_ALERT (BLOODPRESSURE_ALERT_DE)
#define RESPIRATORYRATE_ALERT (RESPIRATORYRATE_ALERT_DE)
#endif

void vitalUpdateAlertDelay(delayAlertDisplay_ptr func_ptr);
int vitalsAlert(const std::string &alertMessage);
void vitalAlertDelayDisplay(long long durationInSeconds);
