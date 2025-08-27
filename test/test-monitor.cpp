#include "./test_monitor.h"
#include <cmath>
#include <cstddef>
#include <functional>
#include <limits>
#include <regex>
#include <string>
#include <vector>

// Sweep Tests for all vitals
struct SweepParam {
  std::function<bool(float)> checkFunc;
  float min, max;
  const char *alert;
  std::vector<float> belowValues, aboveValues;
};

namespace EdgeCaseFloats {
inline float NaN() { return std::numeric_limits<float>::quiet_NaN(); }
inline float Inf() { return std::numeric_limits<float>::infinity(); }
inline float NegInf() { return -std::numeric_limits<float>::infinity(); }
} // namespace EdgeCaseFloats

// For parameterized sweeps
class VitalSweepTest : public MonitorTest,
                       public ::testing::WithParamInterface<SweepParam> {};

// Macros for generalized vital checks without ambiguity
// Utility to call a function with a tuple of arguments
#define CALL_FUNC(func, args) func args

// Handles expected = 1 (no alert)
#define _CHECK_VITAL_EXPECT_1() EXPECT_EQ(GetCapturedOutput(), "");

// Handles expected = 0 (one or more alerts)
#define _CHECK_VITAL_EXPECT_0(...)                                             \
  ExpectMultipleAlerts(GetCapturedOutput(), __VA_ARGS__)

// Main macro to test vital checks
#define CHECK_VITAL(function, args, expected, ...)                             \
  ResetOutput();                                                               \
  EXPECT_EQ(CALL_FUNC(function, args), expected);                              \
  _CHECK_VITAL_EXPECT_##expected(__VA_ARGS__)

// Like CHECK_VITAL, but expects false return
#define CHECK_VITAL_FALSE(function, args, expected, ...)                       \
  ResetOutput();                                                               \
  EXPECT_FALSE(CALL_FUNC(function, args));                                     \
  _CHECK_VITAL_EXPECT_##expected(__VA_ARGS__)

// Macro to count occurrences of a pattern in a string
#define CHECK_POSITION_COUNTER(position, output, pattern, counter, limit)      \
  position = 0;                                                                \
  counter = 0;                                                                 \
  while ((position = output.find(pattern, position)) != std::string::npos) {   \
    ++counter;                                                                 \
    position += strlen(pattern);                                               \
  }

INSTANTIATE_TEST_SUITE_P(BloodSugar, VitalSweepTest,
                         ::testing::Values(SweepParam{
                             vitalBloodSugarCheck,
                             VITALS_BLOODSUGAR_MIN,
                             VITALS_BLOODSUGAR_MAX,
                             BLOODSUGAR_ALERT,
                             {10.0f, 20.0f, 30.0f, 60.0f},
                             {120.0f, 200.0f, 300.0f}}));

INSTANTIATE_TEST_SUITE_P(BloodPressure, VitalSweepTest,
                         ::testing::Values(SweepParam{
                             vitalBloodPressureCheck,
                             VITALS_BLOODPRESSURE_MIN,
                             VITALS_BLOODPRESSURE_MAX,
                             BLOODPRESSURE_ALERT,
                             {10.0f, 20.0f, 30.0f, 80.0f},
                             {160.0f, 200.0f, 300.0f}}));

INSTANTIATE_TEST_SUITE_P(RespiratoryRate, VitalSweepTest,
                         ::testing::Values(SweepParam{
                             vitalRespiratoryRateCheck,
                             VITALS_RESPIRATORYRATE_MIN,
                             VITALS_RESPIRATORYRATE_MAX,
                             RESPIRATORYRATE_ALERT,
                             {0.0f, 5.0f, 10.0f},
                             {25.0f, 30.0f, 40.0f}}));

// Helper to verify multiple alerts in captured output
void ExpectMultipleAlerts(const std::string &output,
                          const std::vector<const char *> &alerts) {
  for (const char *alert : alerts) {
    EXPECT_TRUE(output.find(alert) != std::string::npos);
  }
}

TEST_P(VitalSweepTest, RangeSweep) {
  auto param = GetParam();

  auto expectOut = [this](auto func, float value, const char *alert) {
    ResetOutput();
    EXPECT_FALSE(func(value));
    EXPECT_TRUE(GetCapturedOutput().find(alert) != std::string::npos);
  };

  for (float value : param.belowValues) {
    expectOut(param.checkFunc, value, param.alert);
  }

  EXPECT_TRUE(param.checkFunc(param.min));
  EXPECT_TRUE(param.checkFunc(param.max));

  for (float value : param.aboveValues) {
    expectOut(param.checkFunc, value, param.alert);
  }
}

TEST_F(MonitorTest, OkWhenAllVitalsInRange) {
  EXPECT_TRUE(monitorVitalsStatus(98.4f, 73.0f, 97.0f));
  EXPECT_TRUE(monitorVitalsStatus(98.1f, 70.0f, 98.0f));
  EXPECT_TRUE(monitorVitalsStatus(98.4f, 73.0f, 97.0f));
  EXPECT_EQ(GetCapturedOutput(), "");
}

// Tests for individual vitals
TEST_F(MonitorTest, FalseWhenAnyVitalOutOfRange) {
  CHECK_VITAL(monitorVitalsStatus, (104.0f, 73.0f, 97.0f), 0,
              {TEMPERATURE_ALERT});
  CHECK_VITAL(monitorVitalsStatus, (98.1f, 120.0f, 98.0f), 0, {PULSE_ALERT});
  CHECK_VITAL(monitorVitalsStatus, (98.4f, 73.0f, 80.0f), 0, {SPO2_ALERT});
}

TEST_F(MonitorTest, TemperatureRangeSweep) {
  CHECK_VITAL(monitorVitalsStatus, (10.0f, 72.0f, 97.0f), 0,
              {TEMPERATURE_ALERT});
  EXPECT_TRUE(monitorVitalsStatus(VITALS_TEMPERATURE_MIN_DEGF, 72.0f, 97.0f));
  EXPECT_TRUE(monitorVitalsStatus(VITALS_TEMPERATURE_MAX_DEGF, 73.0f, 98.0f));
  CHECK_VITAL(monitorVitalsStatus, (110.0f, 72.0f, 97.0f), 0,
              {TEMPERATURE_ALERT});
}

TEST_F(MonitorTest, PulseRangeSweep) {
  CHECK_VITAL(monitorVitalsStatus, (98.4f, 10.0f, 97.0f), 0, {PULSE_ALERT});
  EXPECT_TRUE(monitorVitalsStatus(98.4f, VITALS_PULSE_MIN_COUNT, 98.0f));
  EXPECT_TRUE(monitorVitalsStatus(98.4f, VITALS_PULSE_MAX_COUNT, 98.0f));
  CHECK_VITAL(monitorVitalsStatus, (98.4f, 110.0f, 97.0f), 0, {PULSE_ALERT});
}

TEST_F(MonitorTest, SPO2RangeSweep) {
  CHECK_VITAL(monitorVitalsStatus, (98.4f, 72.0f, 10.0f), 0, {SPO2_ALERT});
  EXPECT_TRUE(monitorVitalsStatus(98.4f, 72.0f, VITALS_SPO2_MIN_PERCENT));
  EXPECT_TRUE(monitorVitalsStatus(98.4f, 72.0f, 95.0f));
  EXPECT_TRUE(monitorVitalsStatus(98.4f, 72.0f, 100.0f));
}

TEST_F(MonitorTest, VitalTemperatureCheck) {
  CHECK_VITAL(vitalTemperatureCheck, (98.0f), 1);
  CHECK_VITAL(vitalTemperatureCheck, (50.0f), 0, {TEMPERATURE_ALERT});
  CHECK_VITAL(vitalTemperatureCheck, (94.0f), 0, {TEMPERATURE_ALERT});
}

TEST_F(MonitorTest, VitalPulseCheck) {
  CHECK_VITAL(vitalPulseCheck, (72.0f), 1);
  CHECK_VITAL(vitalPulseCheck, (50.0f), 0, {PULSE_ALERT});
  CHECK_VITAL(vitalPulseCheck, (110.0f), 0, {PULSE_ALERT});
}

TEST_F(MonitorTest, VitalOxygenCheck) {
  CHECK_VITAL(vitalOxygenCheck, (97.0f), 1);
  CHECK_VITAL(vitalOxygenCheck, (80.0f), 0, {SPO2_ALERT});
}

TEST_F(MonitorTest, VitalsAlertOutputAndCycles) {
  ResetOutput();
  EXPECT_EQ(vitalsAlert("Test Alert!\n"), 1);
  std::string output = GetCapturedOutput();
  EXPECT_TRUE(output.find("Test Alert!") != std::string::npos);

  size_t starCount = 0;
  size_t spaceStarCount = 0;
  size_t position = 0;

  CHECK_POSITION_COUNTER(position, output, "\r* ", starCount, 3);
  CHECK_POSITION_COUNTER(position, output, "\r *", spaceStarCount, 3);

  EXPECT_EQ(starCount, VITALS_ALERT_MAX_CYCLE);
  EXPECT_EQ(spaceStarCount, VITALS_ALERT_MAX_CYCLE);
}

TEST_F(MonitorTest, VitalUpdateAlertDelay) {
  static int delayCallCount = 0;
  delayCallCount = 0;

  auto customDelay = [](long long seconds) {
    EXPECT_EQ(seconds, VITALS_ALERT_HOLD_SECONDS);
    ++delayCallCount;
  };
  vitalUpdateAlertDelay(customDelay);

  ResetOutput();
  vitalsAlert("Delay Test!\n");
  EXPECT_EQ(delayCallCount, 2 * VITALS_ALERT_MAX_CYCLE);
}

TEST_F(MonitorTest, EdgeCasesWithInvalidValues) {
  float nan = std::numeric_limits<float>::quiet_NaN();
  float inf = std::numeric_limits<float>::infinity();
  float neg_inf = -inf;

  CHECK_VITAL(monitorVitalsStatus, (nan, 72.0f, 97.0f), 0, {TEMPERATURE_ALERT});
  CHECK_VITAL(monitorVitalsStatus, (98.4f, inf, 97.0f), 0, {PULSE_ALERT});
  CHECK_VITAL(monitorVitalsStatus, (98.4f, 72.0f, neg_inf), 0, {SPO2_ALERT});
  CHECK_VITAL(monitorVitalsStatus, (98.4f, nan, 97.0f), 0, {PULSE_ALERT});
  CHECK_VITAL(monitorVitalsStatus, (inf, inf, inf), 0,
              {TEMPERATURE_ALERT, PULSE_ALERT, SPO2_ALERT});
}

TEST_F(MonitorTest, MultipleAlertsInOneCheck) {
  CHECK_VITAL(monitorVitalsStatus, (104.0f, 110.0f, 80.0f), 0,
              {TEMPERATURE_ALERT, PULSE_ALERT, SPO2_ALERT});
}

/* Vitals 2.0 */
TEST_F(MonitorTest, VitalBloodSugarCheck) {
  CHECK_VITAL(vitalBloodSugarCheck, (80.0f), 1);
  CHECK_VITAL(vitalBloodSugarCheck, (120.0f), 0, {BLOODSUGAR_ALERT});
  CHECK_VITAL(vitalBloodSugarCheck, (60.0f), 0, {BLOODSUGAR_ALERT});
}

TEST_F(MonitorTest, VitalBloodPressureCheck) {
  CHECK_VITAL(vitalBloodPressureCheck, (120.0f), 1);
  CHECK_VITAL(vitalBloodPressureCheck, (160.0f), 0, {BLOODPRESSURE_ALERT});
  CHECK_VITAL(vitalBloodPressureCheck, (80.0f), 0, {BLOODPRESSURE_ALERT});
}

TEST_F(MonitorTest, VitalRespiratoryRateCheck) {
  CHECK_VITAL(vitalRespiratoryRateCheck, (16.0f), 1);
  CHECK_VITAL(vitalRespiratoryRateCheck, (25.0f), 0, {RESPIRATORYRATE_ALERT});
  CHECK_VITAL(vitalRespiratoryRateCheck, (10.0f), 0, {RESPIRATORYRATE_ALERT});
}

TEST_F(MonitorTest, VitalsReportNormalWhenAllInRange) {
  Report_t report = {98.4f, 73.0f, 97.0f, 80.0f, 120.0f, 16.0f};
  CHECK_VITAL(monitorVitalsReportStatus, (&report), 1);
}

TEST_F(MonitorTest, VitalsReportNormalFalseWhenAnyOutOfRange) {
  Report_t report_temp_high = {104.0f, 73.0f, 97.0f, 80.0f, 120.0f, 16.0f};
  CHECK_VITAL(monitorVitalsReportStatus, (&report_temp_high), 0,
              {TEMPERATURE_ALERT});

  Report_t report_blood_sugar_low = {98.4f, 73.0f, 97.0f, 60.0f, 120.0f, 16.0f};
  CHECK_VITAL(monitorVitalsReportStatus, (&report_blood_sugar_low), 0,
              {BLOODSUGAR_ALERT});

  Report_t report_blood_pressure_high = {98.4f, 73.0f,  97.0f,
                                         80.0f, 160.0f, 16.0f};
  CHECK_VITAL(monitorVitalsReportStatus, (&report_blood_pressure_high), 0,
              {BLOODPRESSURE_ALERT});

  Report_t report_respiratory_low = {98.4f, 73.0f, 97.0f, 80.0f, 120.0f, 10.0f};
  CHECK_VITAL(monitorVitalsReportStatus, (&report_respiratory_low), 0,
              {RESPIRATORYRATE_ALERT});
}

TEST_F(MonitorTest, VitalsReportNormalMultipleAlerts) {
  Report_t report_multi = {104.0f, 110.0f, 80.0f, 120.0f, 160.0f, 25.0f};

  CHECK_VITAL(monitorVitalsReportStatus, (&report_multi), 0,
              {TEMPERATURE_ALERT, PULSE_ALERT, SPO2_ALERT, BLOODSUGAR_ALERT,
               BLOODPRESSURE_ALERT, RESPIRATORYRATE_ALERT});
}

TEST_F(MonitorTest, NewVitalsEdgeCasesWithInvalidValues) {

  ExpectInvalidValueAlert(vitalBloodSugarCheck, EdgeCaseFloats::NaN(),
                          BLOODSUGAR_ALERT);
  ExpectInvalidValueAlert(vitalBloodPressureCheck, EdgeCaseFloats::Inf(),
                          BLOODPRESSURE_ALERT);
  ExpectInvalidValueAlert(vitalRespiratoryRateCheck, EdgeCaseFloats::NegInf(),
                          RESPIRATORYRATE_ALERT);
}

TEST_F(MonitorTest, VitalsReportNormalWithInvalidValues) {
  Report_t report_nan_bs = {98.4f,  73.0f, 97.0f, EdgeCaseFloats::NaN(),
                            120.0f, 16.0f};
  CHECK_VITAL_FALSE(monitorVitalsReportStatus, (&report_nan_bs), 0,
                    {BLOODSUGAR_ALERT});

  Report_t report_inf_bp = {98.4f, 73.0f, 97.0f, 80.0f, EdgeCaseFloats::Inf(),
                            16.0f};
  CHECK_VITAL_FALSE(monitorVitalsReportStatus, (&report_inf_bp), 0,
                    {BLOODPRESSURE_ALERT});

  Report_t report_nan_rr = {98.4f, 73.0f,  97.0f,
                            80.0f, 120.0f, EdgeCaseFloats::NaN()};
  CHECK_VITAL_FALSE(monitorVitalsReportStatus, (&report_nan_rr), 0,
                    {RESPIRATORYRATE_ALERT});
}
