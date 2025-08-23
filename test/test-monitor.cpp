#include "../src/monitor.h" // Assuming this is the include path for the header
#include <gtest/gtest.h>
#include <limits>  // For numeric_limits if needed for edge cases
#include <sstream> // For capturing stdout in tests
#include <string>

// Fixture class to set up common test environment
class MonitorTest : public ::testing::Test {
protected:
  void SetUp() override {
    // Set no-op delay for all tests to avoid real sleeps and make tests fast
    vitalUpdateAlertDelay([](long long /*seconds*/) {});
    // Redirect cout to a stringstream for output capture
    original_cout_buffer = std::cout.rdbuf(output_stream.rdbuf());
  }

  void TearDown() override {
    // Restore original cout buffer
    std::cout.rdbuf(original_cout_buffer);
    // Reset delay to default if needed (but not necessary for tests)
  }

  // Helper to get captured output
  std::string GetCapturedOutput() { return output_stream.str(); }

  // Helper to reset output stream
  void ResetOutput() {
    output_stream.str("");
    output_stream.clear();
  }

private:
  std::stringstream output_stream;
  std::streambuf *original_cout_buffer = nullptr;
};

// Test vitalsOk when all vitals are in range
TEST_F(MonitorTest, OkWhenAllVitalsInRange) {
  EXPECT_TRUE(vitalsOk(98.4f, 73.0f, 97.0f));
  EXPECT_TRUE(vitalsOk(98.1f, 70.0f, 98.0f));
  EXPECT_TRUE(vitalsOk(98.4f, 73.0f, 97.0f));
  EXPECT_EQ(GetCapturedOutput(), ""); // No alerts expected
}

// Test vitalsOk when any vital is out of range
TEST_F(MonitorTest, FalseWhenAnyVitalOutOfRange) {
  ResetOutput();
  EXPECT_FALSE(vitalsOk(104.0f, 73.0f, 97.0f));
  EXPECT_TRUE(GetCapturedOutput().find("Temperature is critical!") !=
              std::string::npos);

  ResetOutput();
  EXPECT_FALSE(vitalsOk(98.1f, 120.0f, 98.0f));
  EXPECT_TRUE(GetCapturedOutput().find("Pulse Rate is out of range!") !=
              std::string::npos);

  ResetOutput();
  EXPECT_FALSE(vitalsOk(98.4f, 73.0f, 80.0f));
  EXPECT_TRUE(GetCapturedOutput().find("Oxygen Saturation out of range!") !=
              std::string::npos);
}

// Sweep tests for temperature ranges in vitalsOk (covers vitalTemperatureCheck
// indirectly)
TEST_F(MonitorTest, TemperatureRangeSweep) {
  // Below min
  ResetOutput();
  EXPECT_FALSE(vitalsOk(10.0f, 72.0f, 97.0f));
  EXPECT_TRUE(GetCapturedOutput().find("Temperature is critical!") !=
              std::string::npos);

  // At boundaries
  EXPECT_TRUE(vitalsOk(VITALS_TEMPERATURE_MIN_DEGF, 72.0f, 97.0f));
  EXPECT_TRUE(vitalsOk(VITALS_TEMPERATURE_MAX_DEGF, 72.0f, 98.0f));

  // Above max
  ResetOutput();
  EXPECT_FALSE(vitalsOk(110.0f, 72.0f, 97.0f));
  EXPECT_TRUE(GetCapturedOutput().find("Temperature is critical!") !=
              std::string::npos);
}

// Sweep tests for pulse rate ranges in vitalsOk (covers vitalPulseCheck
// indirectly)
TEST_F(MonitorTest, PulseRangeSweep) {
  // Below min
  ResetOutput();
  EXPECT_FALSE(vitalsOk(98.4f, 10.0f, 97.0f));
  EXPECT_TRUE(GetCapturedOutput().find("Pulse Rate is out of range!") !=
              std::string::npos);

  // At boundaries
  EXPECT_TRUE(vitalsOk(98.4f, VITALS_PULSE_MIN_COUNT, 97.0f));
  EXPECT_TRUE(vitalsOk(98.1f, VITALS_PULSE_MAX_COUNT, 98.0f));

  // Above max
  ResetOutput();
  EXPECT_FALSE(vitalsOk(98.4f, 110.0f, 97.0f));
  EXPECT_TRUE(GetCapturedOutput().find("Pulse Rate is out of range!") !=
              std::string::npos);
}

// Sweep tests for SPO2 ranges in vitalsOk (covers vitalOxygenCheck indirectly)
TEST_F(MonitorTest, SPO2RangeSweep) {
  // Below min
  ResetOutput();
  EXPECT_FALSE(vitalsOk(98.4f, 72.0f, 10.0f));
  EXPECT_TRUE(GetCapturedOutput().find("Oxygen Saturation out of range!") !=
              std::string::npos);

  // At min and above (SPO2 can be >100? Assuming yes as per reference)
  EXPECT_TRUE(vitalsOk(98.4f, 72.0f,
                       VITALS_SPO2_MIN_PERCENT)); // Assuming typo fixed to
                                                  // VITALS_SPO2_MIN_PERCENT
  EXPECT_TRUE(vitalsOk(98.4f, 72.0f, 95.0f));
  EXPECT_TRUE(vitalsOk(98.4f, 72.0f, 100.0f));
}

// Direct tests for individual check functions
TEST_F(MonitorTest, VitalTemperatureCheck) {
  ResetOutput();
  EXPECT_EQ(vitalTemperatureCheck(98.0f), 1);
  EXPECT_EQ(GetCapturedOutput(), ""); // In range, no alert

  ResetOutput();
  EXPECT_EQ(vitalTemperatureCheck(103.0f), 0); // Above max
  EXPECT_TRUE(GetCapturedOutput().find("Temperature is critical!") !=
              std::string::npos);

  ResetOutput();
  EXPECT_EQ(vitalTemperatureCheck(94.0f),
            0); // Below min (assuming min=95.0f or similar)
  EXPECT_TRUE(GetCapturedOutput().find("Temperature is critical!") !=
              std::string::npos);
}

TEST_F(MonitorTest, VitalPulseCheck) {
  ResetOutput();
  EXPECT_EQ(vitalPulseCheck(72.0f), 1);
  EXPECT_EQ(GetCapturedOutput(), ""); // In range

  ResetOutput();
  EXPECT_EQ(vitalPulseCheck(50.0f), 0); // Below min
  EXPECT_TRUE(GetCapturedOutput().find("Pulse Rate is out of range!") !=
              std::string::npos);

  ResetOutput();
  EXPECT_EQ(vitalPulseCheck(110.0f), 0); // Above max
  EXPECT_TRUE(GetCapturedOutput().find("Pulse Rate is out of range!") !=
              std::string::npos);
}

TEST_F(MonitorTest, VitalOxygenCheck) {
  ResetOutput();
  EXPECT_EQ(vitalOxygenCheck(97.0f), 1);
  EXPECT_EQ(GetCapturedOutput(), ""); // In range

  ResetOutput();
  EXPECT_EQ(vitalOxygenCheck(80.0f), 0); // Below min
  EXPECT_TRUE(GetCapturedOutput().find("Oxygen Saturation out of range!") !=
              std::string::npos);
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
  EXPECT_FALSE(vitalsOk(nan, 72.0f, 97.0f));
  EXPECT_TRUE(GetCapturedOutput().find("Temperature is critical!") !=
              std::string::npos);

  // Inf in pulseRate
  ResetOutput();
  EXPECT_FALSE(vitalsOk(98.4f, inf, 97.0f));
  EXPECT_TRUE(GetCapturedOutput().find("Pulse Rate is out of range!") !=
              std::string::npos);

  // Negative Inf in spo2
  ResetOutput();
  EXPECT_FALSE(vitalsOk(98.4f, 72.0f, neg_inf));
  EXPECT_TRUE(GetCapturedOutput().find("Oxygen Saturation out of range!") !=
              std::string::npos);

  // Mixed: NaN in one, valid in others
  ResetOutput();
  EXPECT_FALSE(vitalsOk(98.4f, nan, 97.0f));
  EXPECT_TRUE(GetCapturedOutput().find("Pulse Rate is out of range!") !=
              std::string::npos);

  // All Inf
  ResetOutput();
  EXPECT_FALSE(vitalsOk(inf, inf, inf));
  std::string output = GetCapturedOutput();
  EXPECT_TRUE(output.find("Temperature is critical!") != std::string::npos);
  EXPECT_TRUE(output.find("Pulse Rate is out of range!") != std::string::npos);
  EXPECT_TRUE(output.find("Oxygen Saturation out of range!") !=
              std::string::npos);
}

// Test multiple alerts in one vitalsOk call (e.g., two vitals out)
TEST_F(MonitorTest, MultipleAlertsInOneCheck) {
  ResetOutput();
  EXPECT_FALSE(vitalsOk(104.0f, 110.0f, 80.0f));
  std::string output = GetCapturedOutput();
  EXPECT_TRUE(output.find("Temperature is critical!") != std::string::npos);
  EXPECT_TRUE(output.find("Pulse Rate is out of range!") != std::string::npos);
  EXPECT_TRUE(output.find("Oxygen Saturation out of range!") !=
              std::string::npos);
}
