#include <gtest/gtest.h>
#include <frc/Alert.h>
#include <fmt/format.h>
#include <chrono>
TEST(AlertsTest, SmokeTest) {
  { frc::Alert("Thing A", frc::Alert::AlertType::kError); }
  frc::Alert a{"", frc::Alert::AlertType::kInfo};
  {
    std::string s = fmt::format("{}", std::chrono::steady_clock::now().time_since_epoch().count());
    a = frc::Alert(s, frc::Alert::AlertType::kError);
  }
}