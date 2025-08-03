#include <gtest/gtest.h>
#include "./monitor.h"
#include <sstream>
#include <thread>
#include <chrono>

// Constants for expected output
const std::string ALERT_ASTERISKS = "* * * "; // For VITALS_ALERT_MAX_CYCLE = 3

// Test fixture
class VitalsTest : public ::testing::Test {
protected:
    void SetUp() override {
        original_cout_buffer = std::cout.rdbuf();
        std::cout.rdbuf(output_stream.rdbuf());
    }

    void TearDown() override {
        std::cout.rdbuf(original_cout_buffer);
    }

    // Helper to generate expected alert output
    std::string getExpectedAlertOutput(const std::string& message) const {
        return message + ALERT_ASTERISKS;
    }

    // Helper to check alert output and return value
    void checkAlertFunction(int (*func)(float), float value, const std::string& expected_message, int expected_return) {
        output_stream.str(""); // Clear output stream
        EXPECT_EQ(func(value), expected_return);
        if (expected_return == 0) {
            EXPECT_EQ(output_stream.str(), getExpectedAlertOutput(expected_message));
        } else {
            EXPECT_TRUE(output_stream.str().empty());
        }
    }

    std::stringstream output_stream;
    std::streambuf* original_cout_buffer = nullptr;
};

// Parameterized test struct for vital checks
struct VitalCheckTestParams {
    float value;
    int expected_return;
    std::string alert_message;
};

// Test vitalsAlert
TEST_F(VitalsTest, VitalsAlert_PrintsCorrectMessage) {
    std::string alert_message = "Test Alert!\n";
    EXPECT_EQ(vitalsAlert(alert_message), 1);
    EXPECT_EQ(output_stream.str(), getExpectedAlertOutput(alert_message));
}

// Parameterized tests for vitalTemperatureCheck
class VitalTemperatureCheckTest : public VitalsTest, public ::testing::WithParamInterface<VitalCheckTestParams> {};

TEST_P(VitalTemperatureCheckTest, TemperatureCheck) {
    const auto& param = GetParam();
    checkAlertFunction(vitalTemperatureCheck, param.value, param.alert_message, param.expected_return);
}

INSTANTIATE_TEST_SUITE_P(
    TemperatureTests,
    VitalTemperatureCheckTest,
    ::testing::Values(
        VitalCheckTestParams{98.6f, 1, ""}, // Normal
        VitalCheckTestParams{101.0f, 0, "Temperature is critical!\n"}, // Too high
        VitalCheckTestParams{94.0f, 0, "Temperature is critical!\n"}   // Too low
    ));

// Parameterized tests for vitalPulseCheck
class VitalPulseCheckTest : public VitalsTest, public ::testing::WithParamInterface<VitalCheckTestParams> {};

TEST_P(VitalPulseCheckTest, PulseCheck) {
    const auto& param = GetParam();
    checkAlertFunction(vitalPulseCheck, param.value, param.alert_message, param.expected_return);
}

INSTANTIATE_TEST_SUITE_P(
    PulseTests,
    VitalPulseCheckTest,
    ::testing::Values(
        VitalCheckTestParams{80.0f, 1, ""}, // Normal
        VitalCheckTestParams{101.0f, 0, "Pulse Rate is out of range!\n"}, // Too high
        VitalCheckTestParams{59.0f, 0, "Pulse Rate is out of range!\n"}   // Too low
    ));

// Parameterized tests for vitalOxygenCheck
class VitalOxygenCheckTest : public VitalsTest, public ::testing::WithParamInterface<VitalCheckTestParams> {};

TEST_P(VitalOxygenCheckTest, OxygenCheck) {
    const auto& param = GetParam();
    checkAlertFunction(vitalOxygenCheck, param.value, param.alert_message, param.expected_return);
}

INSTANTIATE_TEST_SUITE_P(
    OxygenTests,
    VitalOxygenCheckTest,
    ::testing::Values(
        VitalCheckTestParams{95.0f, 1, ""}, // Normal
        VitalCheckTestParams{89.0f, 0, "Oxygen Saturation out of range!\n"} // Too low
    ));

// Parameterized test struct for vitalsOk
struct VitalsOkTestParams {
    float temperature;
    float pulse;
    float spo2;
    int expected_return;
    std::vector<std::string> alert_messages; // Expected alert messages in order
};

// Parameterized tests for vitalsOk
class VitalsOkTest : public VitalsTest, public ::testing::WithParamInterface<VitalsOkTestParams> {};

TEST_P(VitalsOkTest, VitalsOkCheck) {
    const auto& param = GetParam();
    output_stream.str(""); // Clear output stream
    EXPECT_EQ(vitalsOk(param.temperature, param.pulse, param.spo2), param.expected_return);
    std::string expected_output;
    for (const auto& msg : param.alert_messages) {
        expected_output += getExpectedAlertOutput(msg);
    }
    EXPECT_EQ(output_stream.str(), expected_output);
}

INSTANTIATE_TEST_SUITE_P(
    VitalsOkTests,
    VitalsOkTest,
    ::testing::Values(
        VitalsOkTestParams{98.6f, 80.0f, 95.0f, 1, {}}, // All normal
        VitalsOkTestParams{101.0f, 80.0f, 95.0f, 0, {"Temperature is critical!\n"}}, // Temp too high
        VitalsOkTestParams{98.6f, 101.0f, 95.0f, 0, {"Pulse Rate is out of range!\n"}}, // Pulse too high
        VitalsOkTestParams{98.6f, 80.0f, 89.0f, 0, {"Oxygen Saturation out of range!\n"}}, // SpO2 too low
        VitalsOkTestParams{101.0f, 101.0f, 89.0f, 0, {
            "Temperature is critical!\n",
            "Pulse Rate is out of range!\n",
            "Oxygen Saturation out of range!\n"
        }} // All out of range
    ));

// Main function for running tests
int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
