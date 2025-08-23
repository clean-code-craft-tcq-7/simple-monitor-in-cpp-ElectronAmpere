#pragma once

#include "./alerts.h"
#include "./vitals.h"

/* Monitors 1.0 */
int monitorVitalsStatus(float temperature, float pulseRate, float spo2);

/* Monitors 2.0 */
int monitorVitalsReportStatus(Report_t *vitalReport);
