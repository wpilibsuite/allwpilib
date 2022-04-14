// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#ifndef NTCORE_MOCKENTRYNOTIFIER_H_
#define NTCORE_MOCKENTRYNOTIFIER_H_

#include <memory>

#include "IEntryNotifier.h"
#include "gmock/gmock.h"

namespace nt {

class MockEntryNotifier : public IEntryNotifier {
 public:
  MOCK_CONST_METHOD0(local_notifiers, bool());
  MOCK_METHOD3(
      Add,
      unsigned int(std::function<void(const EntryNotification& event)> callback,
                   std::string_view prefix, unsigned int flags));
  MOCK_METHOD3(
      Add,
      unsigned int(std::function<void(const EntryNotification& event)> callback,
                   unsigned int local_id, unsigned int flags));
  MOCK_METHOD3(AddPolled,
               unsigned int(unsigned int poller_uid, std::string_view prefix,
                            unsigned int flags));
  MOCK_METHOD3(AddPolled,
               unsigned int(unsigned int poller_uid, unsigned int local_id,
                            unsigned int flags));
  MOCK_METHOD5(NotifyEntry,
               void(unsigned int local_id, std::string_view name,
                    std::shared_ptr<Value> value, unsigned int flags,
                    unsigned int only_listener));
};

}  // namespace nt

#endif  // NTCORE_MOCKENTRYNOTIFIER_H_
