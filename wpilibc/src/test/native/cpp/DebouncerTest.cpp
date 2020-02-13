#include "frc/Debouncer.h"
#include "gtest/gtest.h"
#include <units/units.h>

using namespace frc;

TEST(DebouncerTest, DebounceNoTriggerTest) {
  Debouncer debouncer(1000_ms);

  debouncer.Calculate(false);
  debouncer.Calculate(true);

  std::this_thread::sleep_for(std::chrono::milliseconds(100));

  EXPECT_FALSE(debouncer.Calculate(true));
}

TEST(DebouncerTest, DebounceTriggerTest) {
  Debouncer debouncer(20_ms);

  debouncer.Calculate(false);
  debouncer.Calculate(true);

  std::this_thread::sleep_for(std::chrono::milliseconds(100));

  EXPECT_TRUE(debouncer.Calculate(true));
}
