// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <frc/Alert.h>

#include <algorithm>
#include <chrono>
#include <map>
#include <string>
#include <utility>

#include <fmt/format.h>
#include <gtest/gtest.h>
#include <networktables/NetworkTableInstance.h>
#include <networktables/StringArrayTopic.h>

using namespace frc;
using enum Alert::AlertType;
class AlertsTest : public ::testing::Test {
 public:
  ~AlertsTest() {
    EXPECT_EQ(GetSubscriberForType(kError).Get().size(), 0);
    EXPECT_EQ(GetSubscriberForType(kWarning).Get().size(), 0);
    EXPECT_EQ(GetSubscriberForType(kInfo).Get().size(), 0);
  }

  template <typename... Args>
  Alert MakeAlert(Args&&... args) {
    return Alert(GetGroupName(), std::forward<Args>(args)...);
  }

  std::string GetGroupName() {
    const ::testing::TestInfo* testInfo =
        ::testing::UnitTest::GetInstance()->current_test_info();
    return fmt::format("{}_{}", testInfo->test_suite_name(),
                       testInfo->test_case_name());
  }

  bool IsAlertActive(std::string_view text, Alert::AlertType type) {
    std::vector<std::string> activeAlerts = GetSubscriberForType(type).Get();
    return std::find(activeAlerts.begin(), activeAlerts.end(), text) !=
           activeAlerts.end();
  }

 private:
  std::string GetSubtableName(Alert::AlertType type) {
    switch (type) {
      case kError:
        return "errors";
      case kWarning:
        return "warnings";
      case kInfo:
        return "infos";
      default:
        return "unknown";
    }
  }

  std::map<Alert::AlertType, nt::StringArraySubscriber> m_subs;
  const nt::StringArraySubscriber& GetSubscriberForType(Alert::AlertType type) {
    if (m_subs.contains(type)) {
      return m_subs[type];
    } else {
      return m_subs
          .emplace(std::make_pair(
              type, nt::NetworkTableInstance::GetDefault()
                        .GetStringArrayTopic(
                            fmt::format("/SmartDashboard/{}/{}", GetGroupName(),
                                        GetSubtableName(type)))
                        .Subscribe({})))
          .first->second;
    }
  }
};

TEST_F(AlertsTest, Constructors) {
  // Make and destroy
  Alert{GetGroupName(), "One", kError};
  Alert a{GetGroupName(), "Two", kInfo};
  a.Set(true);
  a.Set(false);
  // Move assign
  a = Alert(GetGroupName(), "Three", kWarning);
  // Move construct
  Alert b{std::move(a)};
  {
    // Move assign with dynamic string
    std::string text = fmt::format(
        "{}", std::chrono::steady_clock::now().time_since_epoch().count());
    b = Alert(text, kError);
  }
}
