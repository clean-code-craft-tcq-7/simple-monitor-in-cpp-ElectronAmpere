#include <gtest/gtest.h>
#include <string>
#include <utility>
#include <cmath>
#include <vector>
#include <limits>

#include "./monitor.h"

/**
 * TEST(TestSuiteName, TestName) {
 * ... statements ...
 * }
 */
/* Sweep Test */
TEST(vitalsOk, FloatSweepPulseCheck) {
    float temperature = 98.4f, float spo2 = 91.0f;
    EXPECT_EQ(vitalsOk(temperature, 0.0f,spo2), true) << "Failed for float value: 0.0";
    EXPECT_EQ(vitalsOk(temperature, -1.0f,spo2), false) << "Failed for float value: -1.0";
    EXPECT_EQ(vitalsOk(temperature, 3.14f,spo2), true) << "Failed for float value: 3.14";
    EXPECT_EQ(vitalsOk(temperature, std::numeric_limits<float>::infinity(),spo2), false) << "Failed for float infinity";
    EXPECT_EQ(vitalsOk(temperature, std::numeric_limits<float>::quiet_NaN(),spo2), false) << "Failed for float NaN";
}
/**
TEST(vitalsOk, PulseChecker_T0) {
  EXPECT_FALSE(vitalsOk(98.4, 0, 70));
  EXPECT_TRUE(vitalsOk(98.1, 70, 98));
}

TEST(Monitor, OkWhenAnyVitalIsInRange) {
  ASSERT_TRUE(vitalsOk(98.4, 73, 97));
  ASSERT_TRUE(vitalsOk(98.1, 70, 98));
}
**/
