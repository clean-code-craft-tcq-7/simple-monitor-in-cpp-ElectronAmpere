#include "./vitals_monitor.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

void calculateTolerance(vitalsConfig_t *vital, vitalsHandler_t *handle) {
  if (!vital || !handle || strcmp(vital->name, handle->name) != 0) {
    return;
  }
  handle->tolerance_calculated = (vital->upper_limit * (vital->tolerance_percent / 100.0));
  handle->upper_warning = vital->upper_limit - handle->tolerance_calculated;
  handle->lower_warning = vital->lower_limit + handle->tolerance_calculated;
  handle->upper_limit = vital->upper_limit;
  handle->lower_limit = vital->lower_limit;
}

void convertToBaseUnit(vitalsConfig_t *vital, vitalsHandler_t *handle) {
  if (!vital || !handle || strcmp(vital->name, handle->name) != 0) {
    handle->base_value = handle->report_value; // Default to report value if invalid
    return;
  }
  if (strcmp(handle->report_unit, vital->base_unit) != 0) {
    if (strcmp(vital->base_unit, "F") == 0 && strcmp(handle->report_unit, "C") == 0) {
      // Convert Celsius to Fahrenheit
      handle->base_value = (1.8 * handle->report_value) + 32;
    } else if (strcmp(vital->base_unit, "C") == 0 && strcmp(handle->report_unit, "F") == 0) {
      // Convert Fahrenheit to Celsius
      handle->base_value = (handle->report_value - 32) / 1.8;
    } else {
      handle->base_value = handle->report_value; // Unknown unit, use report value
    }
  } else {
    handle->base_value = handle->report_value; // Same unit, no conversion needed
  }
}

breachType_t checkVitalBreach(vitalsHandler_t *handle) {
  if (!handle) {
    return VITAL_NORMAL; // Default to normal if invalid
  }
  if (handle->base_value > handle->upper_limit) {
    return VITAL_HIGH_BREACHED;
  } else if (handle->base_value >= handle->upper_warning) {
    return VITAL_HIGH_WARNING;
  } else if (handle->base_value < handle->lower_limit) {
    return VITAL_LOW_BREACHED;
  } else if (handle->base_value <= handle->lower_warning) {
    return VITAL_LOW_WARNING;
  }
  return VITAL_NORMAL;
}

const char *getBreachMessage(vitalsHandler_t *handle) {
  if (!handle) {
    return "Invalid vital data";
  }
  if (strcmp(handle->name, "temperature") == 0) {
    switch (handle->breachType) {
      case VITAL_HIGH_BREACHED:
        return "ALARM: Hyperthermia detected!";
      case VITAL_HIGH_WARNING:
        return "WARNING: Approaching hyperthermia";
      case VITAL_LOW_BREACHED:
        return "ALARM: Hypothermia detected!";
      case VITAL_LOW_WARNING:
        return "WARNING: Approaching hypothermia";
      default:
        return "Temperature is normal";
    }
  } else if (strcmp(handle->name, "pulse") == 0) {
    switch (handle->breachType) {
      case VITAL_HIGH_BREACHED:
        return "ALARM: High pulse rate detected!";
      case VITAL_HIGH_WARNING:
        return "WARNING: Approaching high pulse rate";
      case VITAL_LOW_BREACHED:
        return "ALARM: Low pulse rate detected!";
      case VITAL_LOW_WARNING:
        return "WARNING: Approaching low pulse rate";
      default:
        return "Pulse rate is normal";
    }
  } else if (strcmp(handle->name, "spo2") == 0) {
    switch (handle->breachType) {
      case VITAL_HIGH_BREACHED:
        return "ALARM: High SPO2 detected!";
      case VITAL_HIGH_WARNING:
        return "WARNING: Approaching high SPO2";
      case VITAL_LOW_BREACHED:
        return "ALARM: Low SPO2 detected!";
      case VITAL_LOW_WARNING:
        return "WARNING: Approaching low SPO2";
      default:
        return "SPO2 is normal";
    }
  }
  return "Unknown vital parameter";
}

char *getVitalsConfigInfo(vitalsConfig_t *vital) {
  if (!vital) {
    return strdup("Invalid vital configuration");
  }
  char *buffer = (char *)malloc(256);
  if (!buffer) {
    return nullptr;
  }
  snprintf(buffer, 256,
           "Name: %s\nBase Unit: %s\nTolerance Percentage: %.2f%%\nUpper Limit: %.2f\nLower Limit: %.2f",
           vital->name, vital->base_unit, vital->tolerance_percent, vital->upper_limit, vital->lower_limit);
  return buffer;
}

char *getVitalsHandlerInfo(vitalsHandler_t *vital) {
  if (!vital) {
    return strdup("Invalid vital handler");
  }
  char *buffer = (char *)malloc(512);
  if (!buffer) {
    return nullptr;
  }
  snprintf(buffer, 512,
           "Name: %s\nReport Value: %.2f %s\nBase Value: %.2f\nTolerance Calculated: %.2f\n"
           "Upper Limit: %.2f\nUpper Warning: %.2f\nLower Warning: %.2f\nLower Limit: %.2f\n"
           "Breach Type: %d\nStatus: %s",
           vital->name, vital->report_value, vital->report_unit, vital->base_value,
           vital->tolerance_calculated, vital->upper_limit, vital->upper_warning,
           vital->lower_warning, vital->lower_limit, vital->breachType, getBreachMessage(vital));
  return buffer;
}
