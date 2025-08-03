#include <gtest/gtest.h>
#include <limits>
#include "./monitor.h"

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

TEST(Sweep, TemperatureRangeSweep) {
  EXPECT_FALSE(vitalsOk(10, 72, 97));
  EXPECT_FALSE(vitalsOk(20, 72, 97));
  EXPECT_FALSE(vitalsOk(30, 72, 97));
  EXPECT_FALSE(vitalsOk(60,  72, 97));
  EXPECT_TRUE(vitalsOk(VITALS_TEMPERATURE_MIN_DEGF, 72, 97));
  EXPECT_TRUE(vitalsOk(VITALS_TEMPERATURE_MAX_DEGF, 72, 98));
  EXPECT_FALSE(vitalsOk(110, 72, 97));
  EXPECT_FALSE(vitalsOk(200, 72, 97));
  EXPECT_FALSE(vitalsOk(300, 72, 97));
}

TEST(Sweep, SPO2Sweep) {
  EXPECT_FALSE(vitalsOk(98.4, 72, 10));
  EXPECT_FALSE(vitalsOk(98.4, 72, 20));
  EXPECT_FALSE(vitalsOk(98.4, 72, 30));
  EXPECT_FALSE(vitalsOk(98.4,  72, 40));
  EXPECT_TRUE(vitalsOk(98.4, 72, VTIALS_SPO2_MIN_PERCENT));
  EXPECT_TRUE(vitalsOk(98.4, 72, 95));
  EXPECT_TRUE(vitalsOk(98.4, 72, 100));
  EXPECT_TRUE(vitalsOk(98.4, 72, 200));
  EXPECT_TRUE(vitalsOk(98.4, 72, 300));
}

TEST(Monitor, OkWhenAllVitalIsInRange) {
  EXPECT_TRUE(vitalsOk(98.4, 73, 97));
  EXPECT_TRUE(vitalsOk(98.1, 70, 98));
  EXPECT_TRUE(vitalsOk(98.4, 73, 97));
}

TEST(Monitor, NoWhenAnyVitalIsNotInRange) {
  EXPECT_FALSE(vitalsOk(104.0, 73, 97));
  EXPECT_FALSE(vitalsOk(98.1, 120, 98));
  EXPECT_FALSE(vitalsOk(98.4, 73, 80));
}
