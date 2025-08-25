#include "./test_monitor.h" // For numeric_limits if needed for edge cases
#include <string>

#define EXPECT_POSITION_ALERT(output, alertA, alertB, alertC)                  \
  output = GetCapturedOutput();                                                \
  EXPECT_TRUE(output.find(alertA) != std::string::npos);                       \
  EXPECT_TRUE(output.find(alertB) != std::string::npos);                       \
  EXPECT_TRUE(output.find(alertC) != std::string::npos);

// Test monitorVitalsStatus when all vitals are in range
TEST_F(MonitorTest, OkWhenAllVitalsInRange) {
  EXPECT_TRUE(monitorVitalsStatus(98.4f, 73.0f, 97.0f));
  EXPECT_TRUE(monitorVitalsStatus(98.1f, 70.0f, 98.0f));
  EXPECT_TRUE(monitorVitalsStatus(98.4f, 73.0f, 97.0f));
  EXPECT_EQ(GetCapturedOutput(), ""); // No alerts expected
}
// Test monitorVitalsStatus when any vital is out of range
TEST_F(MonitorTest, FalseWhenAnyVitalOutOfRange) {
  ResetOutput();
  EXPECT_FALSE(monitorVitalsStatus(104.0f, 73.0f, 97.0f));
  EXPECT_TRUE(GetCapturedOutput().find(TEMPERATURE_ALERT) != std::string::npos);
  ResetOutput();
  EXPECT_FALSE(monitorVitalsStatus(98.1f, 120.0f, 98.0f));
  EXPECT_TRUE(GetCapturedOutput().find(PULSE_ALERT) != std::string::npos);
  ResetOutput();
  EXPECT_FALSE(monitorVitalsStatus(98.4f, 73.0f, 80.0f));
  EXPECT_TRUE(GetCapturedOutput().find(SPO2_ALERT) != std::string::npos);
}
// Sweep tests for temperature ranges in monitorVitalsStatus (covers
// vitalTemperatureCheck indirectly)
TEST_F(MonitorTest, TemperatureRangeSweep) {
  // Below min
  ResetOutput();
  EXPECT_FALSE(monitorVitalsStatus(10.0f, 72.0f, 97.0f));
  EXPECT_TRUE(GetCapturedOutput().find(TEMPERATURE_ALERT) != std::string::npos);
  // At boundaries
  EXPECT_TRUE(monitorVitalsStatus(VITALS_TEMPERATURE_MIN_DEGF, 72.0f, 97.0f));
  EXPECT_TRUE(monitorVitalsStatus(VITALS_TEMPERATURE_MAX_DEGF, 72.0f, 98.0f));
  // Above max
  ResetOutput();
  EXPECT_FALSE(monitorVitalsStatus(110.0f, 72.0f, 97.0f));
  EXPECT_TRUE(GetCapturedOutput().find(TEMPERATURE_ALERT) != std::string::npos);
}
// Sweep tests for pulse rate ranges in monitorVitalsStatus (covers
// vitalPulseCheck indirectly)
TEST_F(MonitorTest, PulseRangeSweep) {
  // Below min
  ResetOutput();
  EXPECT_FALSE(monitorVitalsStatus(98.4f, 10.0f, 97.0f));
  EXPECT_TRUE(GetCapturedOutput().find(PULSE_ALERT) != std::string::npos);
  // At boundaries
  EXPECT_TRUE(monitorVitalsStatus(98.4f, VITALS_PULSE_MIN_COUNT, 97.0f));
  EXPECT_TRUE(monitorVitalsStatus(98.1f, VITALS_PULSE_MAX_COUNT, 98.0f));
  // Above max
  ResetOutput();
  EXPECT_FALSE(monitorVitalsStatus(98.4f, 110.0f, 97.0f));
  EXPECT_TRUE(GetCapturedOutput().find(PULSE_ALERT) != std::string::npos);
}
// Sweep tests for SPO2 ranges in monitorVitalsStatus (covers vitalOxygenCheck
// indirectly)
TEST_F(MonitorTest, SPO2RangeSweep) {
  // Below min
  ResetOutput();
  EXPECT_FALSE(monitorVitalsStatus(98.4f, 72.0f, 10.0f));
  EXPECT_TRUE(GetCapturedOutput().find(SPO2_ALERT) != std::string::npos);
  // At min and above (SPO2 can be >100? Assuming yes as per reference)
  EXPECT_TRUE(
      monitorVitalsStatus(98.4f, 72.0f,
                          VITALS_SPO2_MIN_PERCENT)); // Assuming typo fixed to
                                                     // VITALS_SPO2_MIN_PERCENT
  EXPECT_TRUE(monitorVitalsStatus(98.4f, 72.0f, 95.0f));
  EXPECT_TRUE(monitorVitalsStatus(98.4f, 72.0f, 100.0f));
}
// Direct tests for individual check functions
TEST_F(MonitorTest, VitalTemperatureCheck) {
  ResetOutput();
  EXPECT_EQ(vitalTemperatureCheck(98.0f), 1);
  EXPECT_EQ(GetCapturedOutput(), ""); // In range, no alert
  ResetOutput();
  EXPECT_EQ(vitalTemperatureCheck(103.0f), 0); // Above max
  EXPECT_TRUE(GetCapturedOutput().find(TEMPERATURE_ALERT) != std::string::npos);
  ResetOutput();
  EXPECT_EQ(vitalTemperatureCheck(94.0f),
            0); // Below min (assuming min=95.0f or similar)
  EXPECT_TRUE(GetCapturedOutput().find(TEMPERATURE_ALERT) != std::string::npos);
}
TEST_F(MonitorTest, VitalPulseCheck) {
  ResetOutput();
  EXPECT_EQ(vitalPulseCheck(72.0f), 1);
  EXPECT_EQ(GetCapturedOutput(), ""); // In range
  ResetOutput();
  EXPECT_EQ(vitalPulseCheck(50.0f), 0); // Below min
  EXPECT_TRUE(GetCapturedOutput().find(PULSE_ALERT) != std::string::npos);
  ResetOutput();
  EXPECT_EQ(vitalPulseCheck(110.0f), 0); // Above max
  EXPECT_TRUE(GetCapturedOutput().find(PULSE_ALERT) != std::string::npos);
}
TEST_F(MonitorTest, VitalOxygenCheck) {
  ResetOutput();
  EXPECT_EQ(vitalOxygenCheck(97.0f), 1);
  EXPECT_EQ(GetCapturedOutput(), ""); // In range
  ResetOutput();
  EXPECT_EQ(vitalOxygenCheck(80.0f), 0); // Below min
  EXPECT_TRUE(GetCapturedOutput().find(SPO2_ALERT) != std::string::npos);
}
// Test vitalsAlert directly (output and cycle behavior)
TEST_F(MonitorTest, VitalsAlertOutputAndCycles) {
  ResetOutput();
  EXPECT_EQ(vitalsAlert("Test Alert!\n"), 1);
  std::string output = GetCapturedOutput();
  EXPECT_TRUE(output.find("Test Alert!") != std::string::npos);
  // Check for asterisk patterns: since delay is no-op, outputs are sequential
  // Expect patterns like "* " and " *" repeated VITALS_ALERT_MAX_CYCLE times
  size_t star_count = 0;
  size_t space_star_count = 0;
  for (size_t pos = 0; (pos = output.find("\r* ", pos)) != std::string::npos;) {
    ++star_count;
    pos += 3; // Advance past found substring
  }
  for (size_t pos = 0; (pos = output.find("\r *", pos)) != std::string::npos;) {
    ++space_star_count;
    pos += 3;
  }
  EXPECT_EQ(star_count, VITALS_ALERT_MAX_CYCLE);
  EXPECT_EQ(space_star_count, VITALS_ALERT_MAX_CYCLE);
}
// Test delay function pointer mechanism
TEST_F(MonitorTest, VitalUpdateAlertDelay) {
  // Use static for call count to enable non-capturing lambda
  static int delay_call_count =
      0; // Reset per test? If needed, reset in SetUp/TearDown or use fixture
         // member with free function.
  // Reset count for this test
  delay_call_count = 0;
  // Non-capturing lambda (compatible with raw function pointer)
  auto custom_delay = [](long long seconds) {
    EXPECT_EQ(seconds, VITALS_ALERT_HOLD_SECONDS);
    ++delay_call_count;
  };
  // Update to custom (now assignable to delayAlertDisplay_ptr)
  vitalUpdateAlertDelay(custom_delay);
  // Call alert, which should use custom delay
  ResetOutput();
  vitalsAlert("Delay Test\n");
  // Expect 2 calls per cycle (two delays per loop)
  EXPECT_EQ(delay_call_count, 2 * VITALS_ALERT_MAX_CYCLE);
}
// Edge cases: Extreme values, NaN, etc. (assuming float inputs)
TEST_F(MonitorTest, EdgeCasesWithInvalidValues) {
  float nan = std::numeric_limits<float>::quiet_NaN();
  float inf = std::numeric_limits<float>::infinity();
  float neg_inf = -inf;
  // NaN in temperature
  ResetOutput();
  EXPECT_FALSE(monitorVitalsStatus(nan, 72.0f, 97.0f));
  EXPECT_TRUE(GetCapturedOutput().find(TEMPERATURE_ALERT) != std::string::npos);
  // Inf in pulseRate
  ResetOutput();
  EXPECT_FALSE(monitorVitalsStatus(98.4f, inf, 97.0f));
  EXPECT_TRUE(GetCapturedOutput().find(PULSE_ALERT) != std::string::npos);
  // Negative Inf in spo2
  ResetOutput();
  EXPECT_FALSE(monitorVitalsStatus(98.4f, 72.0f, neg_inf));
  EXPECT_TRUE(GetCapturedOutput().find(SPO2_ALERT) != std::string::npos);
  // Mixed: NaN in one, valid in others
  ResetOutput();
  EXPECT_FALSE(monitorVitalsStatus(98.4f, nan, 97.0f));
  EXPECT_TRUE(GetCapturedOutput().find(PULSE_ALERT) != std::string::npos);
  // All Inf
  ResetOutput();
  EXPECT_FALSE(monitorVitalsStatus(inf, inf, inf));
  std::string output;
  EXPECT_POSITION_ALERT(output, TEMPERATURE_ALERT, PULSE_ALERT, SPO2_ALERT);
}

// Test multiple alerts in one monitorVitalsStatus call (e.g., two vitals out)
TEST_F(MonitorTest, MultipleAlertsInOneCheck) {
  ResetOutput();
  EXPECT_FALSE(monitorVitalsStatus(104.0f, 110.0f, 80.0f));
  std::string output;
  EXPECT_POSITION_ALERT(output, TEMPERATURE_ALERT, PULSE_ALERT, SPO2_ALERT);
}
// New sweep tests for blood sugar in vitalBloodSugarCheck
TEST_F(MonitorTest, BloodSugarRangeSweep) {
  // Below min
  ResetOutput();
  EXPECT_FALSE(vitalBloodSugarCheck(10.0f));
  EXPECT_TRUE(GetCapturedOutput().find(BLOODSUGAR_ALERT) != std::string::npos);
  ResetOutput();
  EXPECT_FALSE(vitalBloodSugarCheck(20.0f));
  EXPECT_TRUE(GetCapturedOutput().find(BLOODSUGAR_ALERT) != std::string::npos);
  ResetOutput();
  EXPECT_FALSE(vitalBloodSugarCheck(30.0f));
  EXPECT_TRUE(GetCapturedOutput().find(BLOODSUGAR_ALERT) != std::string::npos);
  ResetOutput();
  EXPECT_FALSE(vitalBloodSugarCheck(60.0f));
  EXPECT_TRUE(GetCapturedOutput().find(BLOODSUGAR_ALERT) != std::string::npos);
  // At boundaries
  EXPECT_TRUE(vitalBloodSugarCheck(VITALS_BLOODSUGAR_MIN));
  EXPECT_TRUE(vitalBloodSugarCheck(VITALS_BLOODSUGAR_MAX));
  // Above max
  ResetOutput();
  EXPECT_FALSE(vitalBloodSugarCheck(120.0f));
  EXPECT_TRUE(GetCapturedOutput().find(BLOODSUGAR_ALERT) != std::string::npos);
  ResetOutput();
  EXPECT_FALSE(vitalBloodSugarCheck(200.0f));
  EXPECT_TRUE(GetCapturedOutput().find(BLOODSUGAR_ALERT) != std::string::npos);
  ResetOutput();
  EXPECT_FALSE(vitalBloodSugarCheck(300.0f));
  EXPECT_TRUE(GetCapturedOutput().find(BLOODSUGAR_ALERT) != std::string::npos);
}
// New sweep tests for blood pressure in vitalBloodPressureCheck
TEST_F(MonitorTest, BloodPressureRangeSweep) {
  // Below min
  ResetOutput();
  EXPECT_FALSE(vitalBloodPressureCheck(10.0f));
  EXPECT_TRUE(GetCapturedOutput().find(BLOODPRESSURE_ALERT) !=
              std::string::npos);
  ResetOutput();
  EXPECT_FALSE(vitalBloodPressureCheck(20.0f));
  EXPECT_TRUE(GetCapturedOutput().find(BLOODPRESSURE_ALERT) !=
              std::string::npos);
  ResetOutput();
  EXPECT_FALSE(vitalBloodPressureCheck(30.0f));
  EXPECT_TRUE(GetCapturedOutput().find(BLOODPRESSURE_ALERT) !=
              std::string::npos);
  ResetOutput();
  EXPECT_FALSE(vitalBloodPressureCheck(80.0f));
  EXPECT_TRUE(GetCapturedOutput().find(BLOODPRESSURE_ALERT) !=
              std::string::npos);
  // At boundaries
  EXPECT_TRUE(vitalBloodPressureCheck(VITALS_BLOODPRESSURE_MIN));
  EXPECT_TRUE(vitalBloodPressureCheck(VITALS_BLOODPRESSURE_MAX));
  // Above max
  ResetOutput();
  EXPECT_FALSE(vitalBloodPressureCheck(160.0f));
  EXPECT_TRUE(GetCapturedOutput().find(BLOODPRESSURE_ALERT) !=
              std::string::npos);
  ResetOutput();
  EXPECT_FALSE(vitalBloodPressureCheck(200.0f));
  EXPECT_TRUE(GetCapturedOutput().find(BLOODPRESSURE_ALERT) !=
              std::string::npos);
  ResetOutput();
  EXPECT_FALSE(vitalBloodPressureCheck(300.0f));
  EXPECT_TRUE(GetCapturedOutput().find(BLOODPRESSURE_ALERT) !=
              std::string::npos);
}
// New sweep tests for respiratory rate in vitalRespiratoryRateCheck
// Assuming standard ranges: min 12.0f, max 20.0f (adjust if different)
TEST_F(MonitorTest, RespiratoryRateRangeSweep) {
  // Below min
  ResetOutput();
  EXPECT_FALSE(vitalRespiratoryRateCheck(0.0f));
  EXPECT_TRUE(GetCapturedOutput().find(RESPIRATORYRATE_ALERT) !=
              std::string::npos);
  ResetOutput();
  EXPECT_FALSE(vitalRespiratoryRateCheck(5.0f));
  EXPECT_TRUE(GetCapturedOutput().find(RESPIRATORYRATE_ALERT) !=
              std::string::npos);
  ResetOutput();
  EXPECT_FALSE(vitalRespiratoryRateCheck(10.0f));
  EXPECT_TRUE(GetCapturedOutput().find(RESPIRATORYRATE_ALERT) !=
              std::string::npos);
  // At boundaries
  EXPECT_TRUE(vitalRespiratoryRateCheck(VITALS_RESPIRATORYRATE_MIN));
  EXPECT_TRUE(vitalRespiratoryRateCheck(VITALS_RESPIRATORYRATE_MAX));
  // Above max
  ResetOutput();
  EXPECT_FALSE(vitalRespiratoryRateCheck(25.0f));
  EXPECT_TRUE(GetCapturedOutput().find(RESPIRATORYRATE_ALERT) !=
              std::string::npos);
  ResetOutput();
  EXPECT_FALSE(vitalRespiratoryRateCheck(30.0f));
  EXPECT_TRUE(GetCapturedOutput().find(RESPIRATORYRATE_ALERT) !=
              std::string::npos);
  ResetOutput();
  EXPECT_FALSE(vitalRespiratoryRateCheck(40.0f));
  EXPECT_TRUE(GetCapturedOutput().find(RESPIRATORYRATE_ALERT) !=
              std::string::npos);
}
// Direct tests for individual new check functions
TEST_F(MonitorTest, VitalBloodSugarCheck) {
  ResetOutput();
  EXPECT_EQ(vitalBloodSugarCheck(80.0f), 1);
  EXPECT_EQ(GetCapturedOutput(), ""); // In range, no alert
  ResetOutput();
  EXPECT_EQ(vitalBloodSugarCheck(120.0f), 0); // Above max
  EXPECT_TRUE(GetCapturedOutput().find(BLOODSUGAR_ALERT) != std::string::npos);
  ResetOutput();
  EXPECT_EQ(vitalBloodSugarCheck(60.0f), 0); // Below min
  EXPECT_TRUE(GetCapturedOutput().find(BLOODSUGAR_ALERT) != std::string::npos);
}
TEST_F(MonitorTest, VitalBloodPressureCheck) {
  ResetOutput();
  EXPECT_EQ(vitalBloodPressureCheck(120.0f), 1);
  EXPECT_EQ(GetCapturedOutput(), ""); // In range
  ResetOutput();
  EXPECT_EQ(vitalBloodPressureCheck(160.0f), 0); // Above max
  EXPECT_TRUE(GetCapturedOutput().find(BLOODPRESSURE_ALERT) !=
              std::string::npos);
  ResetOutput();
  EXPECT_EQ(vitalBloodPressureCheck(80.0f), 0); // Below min
  EXPECT_TRUE(GetCapturedOutput().find(BLOODPRESSURE_ALERT) !=
              std::string::npos);
}
TEST_F(MonitorTest, VitalRespiratoryRateCheck) {
  ResetOutput();
  EXPECT_EQ(vitalRespiratoryRateCheck(16.0f), 1);
  EXPECT_EQ(GetCapturedOutput(), ""); // In range
  ResetOutput();
  EXPECT_EQ(vitalRespiratoryRateCheck(25.0f), 0); // Above max
  EXPECT_TRUE(GetCapturedOutput().find(RESPIRATORYRATE_ALERT) !=
              std::string::npos);
  ResetOutput();
  EXPECT_EQ(vitalRespiratoryRateCheck(10.0f), 0); // Below min
  EXPECT_TRUE(GetCapturedOutput().find(RESPIRATORYRATE_ALERT) !=
              std::string::npos);
}
// Tests for monitorVitalsReportStatus
TEST_F(MonitorTest, VitalsReportNormalWhenAllInRange) {
  Report_t report = {98.4f, 73.0f, 97.0f, 80.0f, 120.0f, 16.0f}; // All in range
  EXPECT_TRUE(monitorVitalsReportStatus(&report));
  EXPECT_EQ(GetCapturedOutput(), ""); // No alerts
}
TEST_F(MonitorTest, VitalsReportNormalFalseWhenAnyOutOfRange) {
  Report_t report_temp_high = {104.0f, 73.0f, 97.0f, 80.0f, 120.0f, 16.0f};
  ResetOutput();
  EXPECT_FALSE(monitorVitalsReportStatus(&report_temp_high));
  EXPECT_TRUE(GetCapturedOutput().find(TEMPERATURE_ALERT) != std::string::npos);
  Report_t report_blood_sugar_low = {98.4f, 73.0f, 97.0f, 60.0f, 120.0f, 16.0f};
  ResetOutput();
  EXPECT_FALSE(monitorVitalsReportStatus(&report_blood_sugar_low));
  EXPECT_TRUE(GetCapturedOutput().find(BLOODSUGAR_ALERT) != std::string::npos);
  Report_t report_blood_pressure_high = {98.4f, 73.0f,  97.0f,
                                         80.0f, 160.0f, 16.0f};
  ResetOutput();
  EXPECT_FALSE(monitorVitalsReportStatus(&report_blood_pressure_high));
  EXPECT_TRUE(GetCapturedOutput().find(BLOODPRESSURE_ALERT) !=
              std::string::npos);
  Report_t report_respiratory_low = {98.4f, 73.0f, 97.0f, 80.0f, 120.0f, 10.0f};
  ResetOutput();
  EXPECT_FALSE(monitorVitalsReportStatus(&report_respiratory_low));
  EXPECT_TRUE(GetCapturedOutput().find(RESPIRATORYRATE_ALERT) !=
              std::string::npos);
}
TEST_F(MonitorTest, VitalsReportNormalMultipleAlerts) {
  Report_t report_multiple_out = {104.0f, 110.0f, 80.0f, 120.0f, 160.0f, 25.0f};
  ResetOutput();
  EXPECT_FALSE(monitorVitalsReportStatus(&report_multiple_out));
  std::string output;
  EXPECT_POSITION_ALERT(output, TEMPERATURE_ALERT, PULSE_ALERT, SPO2_ALERT);
  EXPECT_POSITION_ALERT(output, BLOODSUGAR_ALERT, BLOODPRESSURE_ALERT,
                        RESPIRATORYRATE_ALERT);
}
// Edge cases for new vitals in individual checks
TEST_F(MonitorTest, NewVitalsEdgeCasesWithInvalidValues) {
  float nan = std::numeric_limits<float>::quiet_NaN();
  float inf = std::numeric_limits<float>::infinity();
  // NaN in blood sugar
  ResetOutput();
  EXPECT_FALSE(vitalBloodSugarCheck(nan));
  EXPECT_TRUE(GetCapturedOutput().find(BLOODSUGAR_ALERT) != std::string::npos);
  // Inf in blood pressure
  ResetOutput();
  EXPECT_FALSE(vitalBloodPressureCheck(inf));
  EXPECT_TRUE(GetCapturedOutput().find(BLOODPRESSURE_ALERT) !=
              std::string::npos);
  // Negative Inf in respiratory rate
  ResetOutput();
  EXPECT_FALSE(vitalRespiratoryRateCheck(-inf));
  EXPECT_TRUE(GetCapturedOutput().find(RESPIRATORYRATE_ALERT) !=
              std::string::npos);
}
// Edge cases for monitorVitalsReportStatus with invalid values
TEST_F(MonitorTest, VitalsReportNormalWithInvalidValues) {
  float nan = std::numeric_limits<float>::quiet_NaN();
  float inf = std::numeric_limits<float>::infinity();
  Report_t report_nan_blood_sugar = {98.4f, 73.0f, 97.0f, nan, 120.0f, 16.0f};
  ResetOutput();
  EXPECT_FALSE(monitorVitalsReportStatus(&report_nan_blood_sugar));
  EXPECT_TRUE(GetCapturedOutput().find(BLOODSUGAR_ALERT) != std::string::npos);
  Report_t report_inf_blood_pressure = {98.4f, 73.0f, 97.0f, 80.0f, inf, 16.0f};
  ResetOutput();
  EXPECT_FALSE(monitorVitalsReportStatus(&report_inf_blood_pressure));
  EXPECT_TRUE(GetCapturedOutput().find(BLOODPRESSURE_ALERT) !=
              std::string::npos);
  Report_t report_nan_respiratory = {98.4f, 73.0f, 97.0f, 80.0f, 120.0f, nan};
  ResetOutput();
  EXPECT_FALSE(monitorVitalsReportStatus(&report_nan_respiratory));
  EXPECT_TRUE(GetCapturedOutput().find(RESPIRATORYRATE_ALERT) !=
              std::string::npos);
}
