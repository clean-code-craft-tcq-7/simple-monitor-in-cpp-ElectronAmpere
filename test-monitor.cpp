#include <gtest/gtest.h>
#include <limits>

#include "./monitor.h"

TEST(Monitor, OkWhenAnyVitalIsInRange) {
  ASSERT_TRUE(vitalsOk(98.4, 73, 97));
  ASSERT_TRUE(vitalsOk(98.1, 70, 98));
}

TEST(Sweep, PulseRangeSweep) {
  EXPECT_FALSE(vitalsOk(98.4, 10, 97));
  EXPECT_FALSE(vitalsOk(98.4, 20, 97));
  EXPECT_FALSE(vitalsOk(98.4, 30, 97));
  EXPECT_TRUE(vitalsOk(98.4, 60, 97));
  EXPECT_TRUE(vitalsOk(98.4, VITALS_PULSE_MIN_COUNT, 97));
  EXPECT_TRUE(vitalsOk(98.1, VITALS_PULSE_MAX_COUNT, 98));
  EXPECT_FALSE(vitalsOk(98.4, 110, 97));
  EXPECT_FALSE(vitalsOk(98.4, 200, 97));
  EXPECT_FALSE(vitalsOk(98.4, 300, 97));
}
