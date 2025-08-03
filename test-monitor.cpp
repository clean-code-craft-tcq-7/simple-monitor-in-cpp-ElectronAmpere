#include <gtest/gtest.h>
#include "./monitor.h"
#include <stdio.h>
#include <string.h>
#include <unistd.h>

// Constants for expected output
#define ALERT_ASTERISKS "* * * "
#define OUTPUT_FILE "test_output.txt"

// Test fixture for stdout redirection
class VitalsTest : public ::testing::Test {
  protected:
    void SetUp() override {
        redirect_stdout_to_file(OUTPUT_FILE);
    }

    void TearDown() override {
        restore_stdout();
        remove(OUTPUT_FILE); // Clean up file
    }

    void redirect_stdout_to_file(const char* filename) {
        freopen(filename, "w", stdout);
    }

    void restore_stdout() {
        freopen("/dev/tty", "w", stdout);
    }

    char* read_file_content() {
        static char buffer[1024];
        buffer[0] = '\0';
        FILE* file = fopen(OUTPUT_FILE, "r");
        if (file) {
            fread(buffer, 1, sizeof(buffer) - 1, file);
            buffer[sizeof(buffer) - 1] = '\0';
            fclose(file);
        }
        return buffer;
    }

    void get_expected_alert_output(const char* message, char* output, size_t size) {
        snprintf(output, size, "%s%s", message, ALERT_ASTERISKS);
    }

    void check_vital_function(int (*func)(float), float value, 
                              int expected_return, const char* alert_message) {
        char expected[256] = "";
        if (alert_message[0] != '\0') {
            get_expected_alert_output(alert_message, expected, sizeof(expected));
        }
        EXPECT_EQ(func(value), expected_return);
        EXPECT_STREQ(read_file_content(), expected);
    }
};

// Test vitalsAlert
TEST_F(VitalsTest, VitalsAlert_PrintsCorrectMessage) {
    const char* alert_message = "Test Alert!\n";
    char expected[256];
    get_expected_alert_output(alert_message, expected, sizeof(expected));
    EXPECT_EQ(vitalsAlert(alert_message), 1);
    EXPECT_STREQ(read_file_content(), expected);
}

// Test vitalTemperatureCheck
TEST_F(VitalsTest, VitalTemperatureCheck_NormalRange) {
    check_vital_function(vitalTemperatureCheck, 98.6f, 1, "");
}

TEST_F(VitalsTest, VitalTemperatureCheck_TooHigh) {
    check_vital_function(vitalTemperatureCheck, 101.0f, 0, "Temperature is critical!\n");
}

TEST_F(VitalsTest, VitalTemperatureCheck_TooLow) {
    check_vital_function(vitalTemperatureCheck, 94.0f, 0, "Temperature is critical!\n");
}

// Test vitalPulseCheck
TEST_F(VitalsTest, VitalPulseCheck_NormalRange) {
    check_vital_function(vitalPulseCheck, 80.0f, 1, "");
}

TEST_F(VitalsTest, VitalPulseCheck_TooHigh) {
    check_vital_function(vitalPulseCheck, 101.0f, 0, "Pulse Rate is out of range!\n");
}

TEST_F(VitalsTest, VitalPulseCheck_TooLow) {
    check_vital_function(vitalPulseCheck, 59.0f, 0, "Pulse Rate is out of range!\n");
}

// Test vitalOxygenCheck
TEST_F(VitalsTest, VitalOxygenCheck_NormalRange) {
    check_vital_function(vitalOxygenCheck, 95.0f, 1, "");
}

TEST_F(VitalsTest, VitalOxygenCheck_TooLow) {
    check_vital_function(vitalOxygenCheck, 89.0f, 0, "Oxygen Saturation out of range!\n");
}

// Test vitalsOk
TEST_F(VitalsTest, VitalsOk_AllNormal) {
    EXPECT_EQ(vitalsOk(98.6f, 80.0f, 95.0f), 1);
    EXPECT_STREQ(read_file_content(), "");
}

TEST_F(VitalsTest, VitalsOk_TemperatureOutOfRange) {
    char expected[256];
    get_expected_alert_output("Temperature is critical!\n", expected, sizeof(expected));
    EXPECT_EQ(vitalsOk(101.0f, 80.0f, 95.0f), 0);
    EXPECT_STREQ(read_file_content(), expected);
}

TEST_F(VitalsTest, VitalsOk_PulseOutOfRange) {
    char expected[256];
    get_expected_alert_output("Pulse Rate is out of range!\n", expected, sizeof(expected));
    EXPECT_EQ(vitalsOk(98.6f, 101.0f, 95.0f), 0);
    EXPECT_STREQ(read_file_content(), expected);
}

TEST_F(VitalsTest, VitalsOk_OxygenOutOfRange) {
    char expected[256];
    get_expected_alert_output("Oxygen Saturation out of range!\n", expected, sizeof(expected));
    EXPECT_EQ(vitalsOk(98.6f, 80.0f, 89.0f), 0);
    EXPECT_STREQ(read_file_content(), expected);
}

TEST_F(VitalsTest, VitalsOk_MultipleOutOfRange) {
    char expected[1024];
    snprintf(expected, sizeof(expected), "Temperature is critical!\n%s"
                                       "Pulse Rate is out of range!\n%s"
                                       "Oxygen Saturation out of range!\n%s",
             ALERT_ASTERISKS, ALERT_ASTERISKS, ALERT_ASTERISKS);
    EXPECT_EQ(vitalsOk(101.0f, 101.0f, 89.0f), 0);
    EXPECT_STREQ(read_file_content(), expected);
}

