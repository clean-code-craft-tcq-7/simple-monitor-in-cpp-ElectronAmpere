#include <gtest/gtest.h>
#include <limits>

#include "./monitor.h"

/* Sweep Test */
TEST(vitalsOk, FloatSweepPulseCheck) {
  float temperature = 98.4f;
  float spo2 = 91.0f;
  
  EXPECT_EQ(vitalsOk(temperature,0.0f,spo2), true);
  EXPECT_EQ(vitalsOk(temperature,-1.0f,spo2), false);
  EXPECT_EQ(vitalsOk(temperature,3.14f,spo2), true);
  EXPECT_EQ(vitalsOk(temperature,std::numeric_limits<float>::infinity(),spo2), false);
  /* The variable sweep for quiet_NaN() becomes invalid */
  //EXPECT_EQ(vitalsOk(temperature,std::numeric_limits<float>::quiet_NaN(),spo2), false);
}
