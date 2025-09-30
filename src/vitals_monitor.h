#ifndef __VITALS_MONITOR_H__
#define __VITALS_MONITOR_H__

#ifdef __cplusplus
extern "C" {
#endif

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

void calculateTolerance(vitalsConfig_t *vital, vitalsHandler_t *handle);
void convertToBaseUnit(vitalsConfig_t *vital, vitalsHandler_t *handle);
breachType_t checkVitalBreach(vitalsHandler_t *handle);
const char *getBreachMessage(vitalsHandler_t *handle);
char *getVitalsConfigInfo(vitalsConfig_t *vital);
char *getVitalsHandlerInfo(vitalsHandler_t *vital);


#ifdef __cplusplus
}
#endif

#endif /* __VITALS_MONITOR_H__ */
