#pragma once

#include "./alerts.h"
#include "./vitals.h"
#include "./vitals_monitor.h"

/* Monitors 1.0 */
int monitorVitalsStatus(float temperature, float pulseRate, float spo2);

/* Monitors 2.0 */
int monitorVitalsReportStatus(const Report_t *vitalReport);

/* Monitors 3.0 */
char *processVital(vitalsConfig_t *config, vitalsHandler_t *handle);
