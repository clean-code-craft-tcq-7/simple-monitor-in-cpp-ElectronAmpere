#include <gtest/gtest.h>
#include <limits>

#include "./monitor.h"

TEST(Monitor, OkWhenAnyVitalIsInRange) {
  ASSERT_TRUE(vitalsOk(98.4, 73, 97));
  ASSERT_TRUE(vitalsOk(98.1, 70, 98));
}


TEST(Sweep, PulseRangeSweep) {
  for (float move = 0; move <= VITALS_PULSE_MIN_COUNT; move++){
    EXPECT_FALSE(vitalsOk(98.4, move, 97));
  }
  for (float move = VITALS_PULSE_MIN_COUNT+1; move <= VITALS_PULSE_MAX_COUNT; move++){
    EXPECT_TRUE(vitalsOk(98.1, move, 98));
  }
}
