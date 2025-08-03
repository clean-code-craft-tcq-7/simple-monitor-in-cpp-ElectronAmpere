#include <gtest/gtest.h>
#include "./monitor.h"
#include <sstream>
#include <thread>
#include <chrono>

// Redirect cout to capture output
class VitalsTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Save original cout buffer
        original_cout_buffer = std::cout.rdbuf();
        // Redirect cout to our stringstream
        std::cout.rdbuf(output_stream.rdbuf());
    }

    void TearDown() override {
        // Restore original cout buffer
        std::cout.rdbuf(original_cout_buffer);
    }

    std::stringstream output_stream;
    std::streambuf* original_cout_buffer = nullptr;
};

// Test vitalsAlert
TEST_F(VitalsTest, VitalsAlert_PrintsCorrectMessage) {
    std::string alert_message = "Test Alert!\n";
    vitalsAlert(alert_message);
    std::string output = output_stream.str();
    // Expected output: alert message followed by "* * * " (for 3 cycles)
    std::string expected = alert_message + "* * * ";
    EXPECT_EQ(output, expected);
    EXPECT_EQ(vitalsAlert(alert_message), 1); // Always returns 1
}

// Test vitalTemperatureCheck
TEST_F(VitalsTest, VitalTemperatureCheck_NormalRange) {
    EXPECT_EQ(vitalTemperatureCheck(98.6f), 1); // Normal temperature
    EXPECT_TRUE(output_stream.str().empty());   // No alert
}

TEST_F(VitalsTest, VitalTemperatureCheck_TooHigh) {
    EXPECT_EQ(vitalTemperatureCheck(101.0f), 0); // Above max
    std::string output = output_stream.str();
    std::string expected = "Temperature is critical!\n* * * ";
    EXPECT_EQ(output, expected);
}

TEST_F(VitalsTest, VitalTemperatureCheck_TooLow) {
    EXPECT_EQ(vitalTemperatureCheck(94.0f), 0); // Below min
    std::string output = output_stream.str();
    std::string expected = "Temperature is critical!\n* * * ";
    EXPECT_EQ(output, expected);
}

// Test vitalPulseCheck
TEST_F(VitalsTest, VitalPulseCheck_NormalRange) {
    EXPECT_EQ(vitalPulseCheck(80.0f), 1); // Normal pulse
    EXPECT_TRUE(output_stream.str().empty()); // No alert
}

TEST_F(VitalsTest, VitalPulseCheck_TooHigh) {
    EXPECT_EQ(vitalPulseCheck(101.0f), 0); // Above max
    std::string output = output_stream.str();
    std::string expected = "Pulse Rate is out of range!\n* * * ";
    EXPECT_EQ(output, expected);
}

TEST_F(VitalsTest, VitalPulseCheck_TooLow) {
    EXPECT_EQ(vitalPulseCheck(59.0f), 0); // Below min
    std::string output = output_stream.str();
    std::string expected = "Pulse Rate is out of range!\n* * * ";
    EXPECT_EQ(output, expected);
}

// Test vitalOxygenCheck
TEST_F(VitalsTest, VitalOxygenCheck_NormalRange) {
    EXPECT_EQ(vitalOxygenCheck(95.0f), 1); // Normal SpO2
    EXPECT_TRUE(output_stream.str().empty()); // No alert
}

TEST_F(VitalsTest, VitalOxygenCheck_TooLow) {
    EXPECT_EQ(vitalOxygenCheck(89.0f), 0); // Below min
    std::string output = output_stream.str();
    std::string expected = "Oxygen Saturation out of range!\n* * * ";
    EXPECT_EQ(output, expected);
}

// Test vitalsOk
TEST_F(VitalsTest, VitalsOk_AllNormal) {
    EXPECT_EQ(vitalsOk(98.6f, 80.0f, 95.0f), 1); // All in range
    EXPECT_TRUE(output_stream.str().empty());      // No alerts
}

TEST_F(VitalsTest, VitalsOk_TemperatureOutOfRange) {
    EXPECT_EQ(vitalsOk(101.0f, 80.0f, 95.0f), 0); // Temperature too high
    std::string output = output_stream.str();
    std::string expected = "Temperature is critical!\n* * * ";
    EXPECT_EQ(output, expected);
}

TEST_F(VitalsTest, VitalsOk_PulseOutOfRange) {
    EXPECT_EQ(vitalsOk(98.6f, 101.0f, 95.0f), 0); // Pulse too high
    std::string output = output_stream.str();
    std::string expected = "Pulse Rate is out of range!\n* * * ";
    EXPECT_EQ(output, expected);
}

TEST_F(VitalsTest, VitalsOk_OxygenOutOfRange) {
    EXPECT_EQ(vitalsOk(98.6f, 80.0f, 89.0f), 0); // SpO2 too low
    std::string output = output_stream.str();
    std::string expected = "Oxygen Saturation out of range!\n* * * ";
    EXPECT_EQ(output, expected);
}

TEST_F(VitalsTest, VitalsOk_MultipleOutOfRange) {
    EXPECT_EQ(vitalsOk(101.0f, 101.0f, 89.0f), 0); // All out of range
    std::string output = output_stream.str();
    std::string expected = "Temperature is critical!\n* * * "
                          "Pulse Rate is out of range!\n* * * "
                          "Oxygen Saturation out of range!\n* * * ";
    EXPECT_EQ(output, expected);
}
