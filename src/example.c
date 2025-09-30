/******************************************************************************

Welcome to GDB Online.
GDB online is an online compiler and debugger tool for C, C++, Python, Java,
PHP, Ruby, Perl, C#, OCaml, VB, Swift, Pascal, Fortran, Haskell, Objective-C,
Assembly, HTML, CSS, JS, SQLite, Prolog. Code, Compile, Run and Debug online
from anywhere in world.

*******************************************************************************/
#include <stdio.h>
#include <string.h>

typedef enum {
  VITAL_HIGH_BREACHED = -2,
  VITAL_HIGH_WARNING = -1,
  VITAL_NORMAL = 0,
  VITAL_LOW_WARNING = 1,
  VITAL_LOW_BREACHED = 2,
} breachType_t;

typedef struct {
  const char *name;
  float report_value;
  const char *report_unit;
  float base_value;
  float tolerance_calculated;
  float upper_limit;
  float upper_warning;
  float lower_warning;
  float lower_limit;
  breachType_t breachType;
} vitalsHandler_t;

typedef struct {
  const char *name;
  const char *base_unit;
  float tolerance_percent;
  float upper_limit;
  float lower_limit;
} vitalsConfig_t;

void printVitalsConfig(vitalsConfig_t *vital) {
  printf("---------------\n");
  printf("Name: %s\n", vital->name);
  printf("Base Unit: %s\n", vital->base_unit);
  printf("Tolerance Percentage: %f\n", vital->tolerance_percent);
  printf("Upper Limit: %f\n", vital->upper_limit);
  printf("Lower Limit: %f\n", vital->lower_limit);
}

void printVitalsHandler(vitalsHandler_t *vital) {
  printf("---------------\n");
  printf("Name: %s\n", vital->name);
  printf("Report Value: %f\n", vital->report_value);
  printf("Report Unit: %s\n", vital->report_unit);
  printf("Base Value: %f\n", vital->base_value);
  printf("Tolerance Calculated: %f\n", vital->tolerance_calculated);
  printf("Upper Warning: %f\n", vital->upper_warning);
  printf("Lower Warning: %f\n", vital->lower_warning);
  printf("Breach Type: %d\n", vital->breachType);
}

void calculateTolerance(vitalsConfig_t *vital, vitalsHandler_t *handle) {
  handle->tolerance_calculated =
      (vital->upper_limit * (vital->tolerance_percent / 100.0));
  handle->upper_warning = vital->upper_limit - handle->tolerance_calculated;
  handle->lower_warning = vital->lower_limit + handle->tolerance_calculated;
}

void convertToBaseUnit(vitalsConfig_t *vital, vitalsHandler_t *handle) {
  if (strcmp(vital->name, handle->name) == 0) {
    if (strcmp(handle->report_unit, vital->base_unit) != 0) {
      if (strcmp(vital->base_unit, "F") == 0) {
        // C to F
        handle->base_value = (1.80 * handle->report_value) + 32;
      } else {
        // F to C
        handle->base_value = (handle->report_value - 32) / 1.80;
      }
    } else {
      handle->base_value = handle->report_value;
    }
  }
}

int main() {

  vitalsConfig_t vitalSetting = {
      .name = "temperature",
      .base_unit = "F",
      .tolerance_percent = 1.5,
      .upper_limit = 102.0,
      .lower_limit = 95.0,
  };
  vitalsHandler_t vitalhandle = {
      .name = "temperature",
      .report_value = 35.0,
      .report_unit = "C",
      .tolerance_calculated = 0.0,
      .upper_warning = 0.0,
      .lower_warning = 0.0,
      .breachType = VITAL_NORMAL,
  };

  calculateTolerance(&vitalSetting, &vitalhandle);
  convertToBaseUnit(&vitalSetting, &vitalhandle);
  printVitalsConfig(&vitalSetting);
  printVitalsHandler(&vitalhandle);
  return 0;
}
