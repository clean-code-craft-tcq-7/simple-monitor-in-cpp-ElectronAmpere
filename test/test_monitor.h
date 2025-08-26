#pragma once
#include "../src/alerts.h"
#include "../src/monitor.h" // Assuming this is the include path for the header
#include "../src/vitals.h"
#include <gtest/gtest.h>
#include <iostream>
#include <sstream> // For capturing stdout in tests

// Fixture class to set up common test environment
class MonitorTest : public ::testing::Test {
protected:
  // cppcheck-suppress unusedFunction
  void SetUp() override {
    // Set no-op delay for all tests to avoid real sleeps and make tests fast
    vitalUpdateAlertDelay([](long long /*seconds*/) {});
    // Redirect cout to a stringstream for output capture
    original_cout_buffer = std::cout.rdbuf(output_stream.rdbuf());
  }
  // cppcheck-suppress unusedFunction
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
