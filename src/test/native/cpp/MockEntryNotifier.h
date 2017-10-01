/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2017. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#ifndef NT_TEST_MOCKENTRYNOTIFIER_H_
#define NT_TEST_MOCKENTRYNOTIFIER_H_

#include "gmock/gmock.h"

#include "IEntryNotifier.h"

namespace nt {

class MockEntryNotifier : public IEntryNotifier {
 public:
  MOCK_CONST_METHOD0(local_notifiers, bool());
  MOCK_METHOD3(
      Add,
      unsigned int(std::function<void(const EntryNotification& event)> callback,
                   llvm::StringRef prefix, unsigned int flags));
  MOCK_METHOD3(
      Add,
      unsigned int(std::function<void(const EntryNotification& event)> callback,
                   unsigned int local_id, unsigned int flags));
  MOCK_METHOD3(AddPolled,
               unsigned int(unsigned int poller_uid, llvm::StringRef prefix,
                            unsigned int flags));
  MOCK_METHOD3(AddPolled,
               unsigned int(unsigned int poller_uid, unsigned int local_id,
                            unsigned int flags));
  MOCK_METHOD5(NotifyEntry,
               void(unsigned int local_id, StringRef name,
                    std::shared_ptr<Value> value, unsigned int flags,
                    unsigned int only_listener));
};

}  // namespace nt

#endif  // NT_TEST_MOCKENTRYNOTIFIER_H_
