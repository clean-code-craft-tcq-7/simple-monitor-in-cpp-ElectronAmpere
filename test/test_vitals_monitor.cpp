#include <gtest/gtest.h>
#include "../src/vitals_monitor.h"
#include "../src/monitor.h"
#include <string.h>
#include <stdlib.h>

// Test fixture for vitals monitor tests
class VitalsMonitorTest : public ::testing::Test {
protected:
  void SetUp() override {
    // Initialize common test data
    temperatureConfig = {
      .name = "temperature",
      .base_unit = "F",
      .tolerance_percent = 1.5,
      .upper_limit = 102.0,
      .lower_limit = 95.0,
    };
    pulseConfig = {
      .name = "pulse",
      .base_unit = "bpm",
      .tolerance_percent = 1.5,
      .upper_limit = 100.0,
      .lower_limit = 60.0,
    };
    spo2Config = {
      .name = "spo2",
      .base_unit = "%",
      .tolerance_percent = 1.5,
      .upper_limit = 100.0,
      .lower_limit = 90.0,
    };
  }

  vitalsConfig_t temperatureConfig;
  vitalsConfig_t pulseConfig;
  vitalsConfig_t spo2Config;
};

// Test calculateTolerance function
TEST_F(VitalsMonitorTest, CalculateTolerance_Temperature) {
  vitalsHandler_t handle = {
    .name = "temperature",
    .report_value = 98.6,
    .report_unit = "F",
  };
  calculateTolerance(&temperatureConfig, &handle);
  EXPECT_FLOAT_EQ(handle.tolerance_calculated, 102.0 * 0.015); // 1.53
  EXPECT_FLOAT_EQ(handle.upper_warning, 102.0 - 1.53); // 100.47
  EXPECT_FLOAT_EQ(handle.lower_warning, 95.0 + 1.53); // 96.53
  EXPECT_FLOAT_EQ(handle.upper_limit, 102.0);
  EXPECT_FLOAT_EQ(handle.lower_limit, 95.0);
}

TEST_F(VitalsMonitorTest, CalculateTolerance_NullInput) {
  vitalsHandler_t handle = {
    .name = "temperature",
    .report_value = 98.6,
    .report_unit = "F",
  };
  calculateTolerance(nullptr, &handle);
  EXPECT_FLOAT_EQ(handle.tolerance_calculated, 0.0);
  calculateTolerance(&temperatureConfig, nullptr);
  EXPECT_FLOAT_EQ(handle.tolerance_calculated, 0.0);
}

TEST_F(VitalsMonitorTest, CalculateTolerance_MismatchedNames) {
  vitalsHandler_t handle = {
    .name = "pulse",
    .report_value = 70.0,
    .report_unit = "bpm",
  };
  calculateTolerance(&temperatureConfig, &handle);
  EXPECT_FLOAT_EQ(handle.tolerance_calculated, 0.0);
}

// Test convertToBaseUnit function
TEST_F(VitalsMonitorTest, ConvertToBaseUnit_CelsiusToFahrenheit) {
  vitalsHandler_t handle = {
    .name = "temperature",
    .report_value = 35.0,
    .report_unit = "C",
  };
  convertToBaseUnit(&temperatureConfig, &handle);
  EXPECT_FLOAT_EQ(handle.base_value, (1.8 * 35.0) + 32); // 95.0°F
}

TEST_F(VitalsMonitorTest, ConvertToBaseUnit_FahrenheitToCelsius) {
  vitalsConfig_t config = {
    .name = "temperature",
    .base_unit = "C",
    .tolerance_percent = 1.5,
    .upper_limit = 38.89,
    .lower_limit = 35.0,
  };
  vitalsHandler_t handle = {
    .name = "temperature",
    .report_value = 95.0,
    .report_unit = "F",
  };
  convertToBaseUnit(&config, &handle);
  EXPECT_FLOAT_EQ(handle.base_value, (95.0 - 32) / 1.8); // ~35.0°C
}

TEST_F(VitalsMonitorTest, ConvertToBaseUnit_SameUnit) {
  vitalsHandler_t handle = {
    .name = "temperature",
    .report_value = 98.6,
    .report_unit = "F",
  };
  convertToBaseUnit(&temperatureConfig, &handle);
  EXPECT_FLOAT_EQ(handle.base_value, 98.6);
}

TEST_F(VitalsMonitorTest, ConvertToBaseUnit_NullInput) {
  vitalsHandler_t handle = {
    .name = "temperature",
    .report_value = 98.6,
    .report_unit = "F",
  };
  convertToBaseUnit(nullptr, &handle);
  EXPECT_FLOAT_EQ(handle.base_value, 98.6); // Fallback to report value
}

TEST_F(VitalsMonitorTest, ConvertToBaseUnit_UnknownUnit) {
  vitalsHandler_t handle = {
    .name = "temperature",
    .report_value = 98.6,
    .report_unit = "K",
  };
  convertToBaseUnit(&temperatureConfig, &handle);
  EXPECT_FLOAT_EQ(handle.base_value, 98.6); // Fallback to report value
}

// Test checkVitalBreach function
TEST_F(VitalsMonitorTest, CheckVitalBreach_Temperature) {
  vitalsHandler_t handle = {
    .name = "temperature",
    .report_value = 98.6,
    .report_unit = "F",
    .base_value = 98.6,
    .tolerance_calculated = 1.53,
    .upper_limit = 102.0,
    .upper_warning = 100.47,
    .lower_warning = 96.53,
    .lower_limit = 95.0,
  };
  EXPECT_EQ(checkVitalBreach(&handle), VITAL_NORMAL);

  handle.base_value = 103.0;
  EXPECT_EQ(checkVitalBreach(&handle), VITAL_HIGH_BREACHED);

  handle.base_value = 100.48;
  EXPECT_EQ(checkVitalBreach(&handle), VITAL_HIGH_WARNING);

  handle.base_value = 95.0;
  EXPECT_EQ(checkVitalBreach(&handle), VITAL_LOW_BREACHED);

  handle.base_value = 96.0;
  EXPECT_EQ(checkVitalBreach(&handle), VITAL_LOW_WARNING);
}

TEST_F(VitalsMonitorTest, CheckVitalBreach_Pulse) {
  vitalsHandler_t handle = {
    .name = "pulse",
    .report_value = 70.0,
    .report_unit = "bpm",
    .base_value = 70.0,
    .tolerance_calculated = 1.5,
    .upper_limit = 100.0,
    .upper_warning = 98.5,
    .lower_warning = 61.5,
    .lower_limit = 60.0,
  };
  EXPECT_EQ(checkVitalBreach(&handle), VITAL_NORMAL);

  handle.base_value = 101.0;
  EXPECT_EQ(checkVitalBreach(&handle), VITAL_HIGH_BREACHED);

  handle.base_value = 99.0;
  EXPECT_EQ(checkVitalBreach(&handle), VITAL_HIGH_WARNING);

  handle.base_value = 59.0;
  EXPECT_EQ(checkVitalBreach(&handle), VITAL_LOW_BREACHED);

  handle.base_value = 61.0;
  EXPECT_EQ(checkVitalBreach(&handle), VITAL_LOW_WARNING);
}

TEST_F(VitalsMonitorTest, CheckVitalBreach_NullInput) {
  EXPECT_EQ(checkVitalBreach(nullptr), VITAL_NORMAL);
}

// Test getBreachMessage function
TEST_F(VitalsMonitorTest, GetBreachMessage_Temperature) {
  vitalsHandler_t handle = {
    .name = "temperature",
    .breachType = VITAL_NORMAL,
  };
  EXPECT_STREQ(getBreachMessage(&handle), "Temperature is normal");

  handle.breachType = VITAL_HIGH_BREACHED;
  EXPECT_STREQ(getBreachMessage(&handle), "ALARM: Hyperthermia detected!");

  handle.breachType = VITAL_HIGH_WARNING;
  EXPECT_STREQ(getBreachMessage(&handle), "WARNING: Approaching hyperthermia");

  handle.breachType = VITAL_LOW_BREACHED;
  EXPECT_STREQ(getBreachMessage(&handle), "ALARM: Hypothermia detected!");

  handle.breachType = VITAL_LOW_WARNING;
  EXPECT_STREQ(getBreachMessage(&handle), "WARNING: Approaching hypothermia");
}

TEST_F(VitalsMonitorTest, GetBreachMessage_Pulse) {
  vitalsHandler_t handle = {
    .name = "pulse",
    .breachType = VITAL_NORMAL,
  };
  EXPECT_STREQ(getBreachMessage(&handle), "Pulse rate is normal");

  handle.breachType = VITAL_HIGH_BREACHED;
  EXPECT_STREQ(getBreachMessage(&handle), "ALARM: High pulse rate detected!");

  handle.breachType = VITAL_HIGH_WARNING;
  EXPECT_STREQ(getBreachMessage(&handle), "WARNING: Approaching high pulse rate");

  handle.breachType = VITAL_LOW_BREACHED;
  EXPECT_STREQ(getBreachMessage(&handle), "ALARM: Low pulse rate detected!");

  handle.breachType = VITAL_LOW_WARNING;
  EXPECT_STREQ(getBreachMessage(&handle), "WARNING: Approaching low pulse rate");
}

TEST_F(VitalsMonitorTest, GetBreachMessage_SPO2) {
  vitalsHandler_t handle = {
    .name = "spo2",
    .breachType = VITAL_NORMAL,
  };
  EXPECT_STREQ(getBreachMessage(&handle), "SPO2 is normal");

  handle.breachType = VITAL_HIGH_BREACHED;
  EXPECT_STREQ(getBreachMessage(&handle), "ALARM: High SPO2 detected!");

  handle.breachType = VITAL_HIGH_WARNING;
  EXPECT_STREQ(getBreachMessage(&handle), "WARNING: Approaching high SPO2");

  handle.breachType = VITAL_LOW_BREACHED;
  EXPECT_STREQ(getBreachMessage(&handle), "ALARM: Low SPO2 detected!");

  handle.breachType = VITAL_LOW_WARNING;
  EXPECT_STREQ(getBreachMessage(&handle), "WARNING: Approaching low SPO2");
}

TEST_F(VitalsMonitorTest, GetBreachMessage_UnknownVital) {
  vitalsHandler_t handle = {
    .name = "unknown",
    .breachType = VITAL_NORMAL,
  };
  EXPECT_STREQ(getBreachMessage(&handle), "Unknown vital parameter");
}

TEST_F(VitalsMonitorTest, GetBreachMessage_NullInput) {
  EXPECT_STREQ(getBreachMessage(nullptr), "Invalid vital data");
}

// Test getVitalsConfigInfo function
TEST_F(VitalsMonitorTest, GetVitalsConfigInfo_Temperature) {
  char *info = getVitalsConfigInfo(&temperatureConfig);
  ASSERT_NE(info, nullptr);
  std::string expected = "Name: temperature\nBase Unit: F\nTolerance Percentage: 1.50%\nUpper Limit: 102.00\nLower Limit: 95.00";
  EXPECT_STREQ(info, expected.c_str());
  free(info);
}

TEST_F(VitalsMonitorTest, GetVitalsConfigInfo_NullInput) {
  char *info = getVitalsConfigInfo(nullptr);
  ASSERT_NE(info, nullptr);
  EXPECT_STREQ(info, "Invalid vital configuration");
  free(info);
}

// Test getVitalsHandlerInfo function
TEST_F(VitalsMonitorTest, GetVitalsHandlerInfo_TemperatureNormal) {
  vitalsHandler_t handle = {
    .name = "temperature",
    .report_value = 98.6,
    .report_unit = "F",
    .base_value = 98.6,
    .tolerance_calculated = 1.53,
    .upper_limit = 102.0,
    .upper_warning = 100.47,
    .lower_warning = 96.53,
    .lower_limit = 95.0,
    .breachType = VITAL_NORMAL,
  };
  char *info = getVitalsHandlerInfo(&handle);
  ASSERT_NE(info, nullptr);
  std::string expected = "Name: temperature\nReport Value: 98.60 F\nBase Value: 98.60\nTolerance Calculated: 1.53\n"
                        "Upper Limit: 102.00\nUpper Warning: 100.47\nLower Warning: 96.53\nLower Limit: 95.00\n"
                        "Breach Type: 0\nStatus: Temperature is normal";
  EXPECT_STREQ(info, expected.c_str());
  free(info);
}

TEST_F(VitalsMonitorTest, GetVitalsHandlerInfo_TemperatureHighBreach) {
  vitalsHandler_t handle = {
    .name = "temperature",
    .report_value = 103.0,
    .report_unit = "F",
    .base_value = 103.0,
    .tolerance_calculated = 1.53,
    .upper_limit = 102.0,
    .upper_warning = 100.47,
    .lower_warning = 96.53,
    .lower_limit = 95.0,
    .breachType = VITAL_HIGH_BREACHED,
  };
  char *info = getVitalsHandlerInfo(&handle);
  ASSERT_NE(info, nullptr);
  std::string expected = "Name: temperature\nReport Value: 103.00 F\nBase Value: 103.00\nTolerance Calculated: 1.53\n"
                        "Upper Limit: 102.00\nUpper Warning: 100.47\nLower Warning: 96.53\nLower Limit: 95.00\n"
                        "Breach Type: -2\nStatus: ALARM: Hyperthermia detected!";
  EXPECT_STREQ(info, expected.c_str());
  free(info);
}

TEST_F(VitalsMonitorTest, GetVitalsHandlerInfo_NullInput) {
  char *info = getVitalsHandlerInfo(nullptr);
  ASSERT_NE(info, nullptr);
  EXPECT_STREQ(info, "Invalid vital handler");
  free(info);
}

// Test processVital function
TEST_F(VitalsMonitorTest, ProcessVital_TemperatureLowWarning) {
  vitalsHandler_t handle = {
    .name = "temperature",
    .report_value = 35.3,  // Changed from 35.0
    .report_unit = "C",
  };
  char *info = processVital(&temperatureConfig, &handle);
  ASSERT_NE(info, nullptr);
  std::string expected = "Vital: temperature\nReported: 35.30 C\nBase Value: 95.54 F\nStatus: WARNING: Approaching hypothermia";
  EXPECT_STREQ(info, expected.c_str());
  free(info);
}


TEST_F(VitalsMonitorTest, ProcessVital_PulseHighWarning) {
  vitalsHandler_t handle = {
    .name = "pulse",
    .report_value = 99.0,
    .report_unit = "bpm",
  };
  char *info = processVital(&pulseConfig, &handle);
  ASSERT_NE(info, nullptr);
  std::string expected = "Vital: pulse\nReported: 99.00 bpm\nBase Value: 99.00 bpm\nStatus: WARNING: Approaching high pulse rate";
  EXPECT_STREQ(info, expected.c_str());
  free(info);
}

TEST_F(VitalsMonitorTest, ProcessVital_SPO2LowBreach) {
  vitalsHandler_t handle = {
    .name = "spo2",
    .report_value = 88.0,
    .report_unit = "%",
  };
  char *info = processVital(&spo2Config, &handle);
  ASSERT_NE(info, nullptr);
  std::string expected = "Vital: spo2\nReported: 88.00 %\nBase Value: 88.00 %\nStatus: ALARM: Low SPO2 detected!";
  EXPECT_STREQ(info, expected.c_str());
  free(info);
}

TEST_F(VitalsMonitorTest, ProcessVital_NullInput) {
  char *info = processVital(nullptr, nullptr);
  ASSERT_NE(info, nullptr);
  EXPECT_STREQ(info, "Error: Invalid vital configuration or handler");
  free(info);
}
