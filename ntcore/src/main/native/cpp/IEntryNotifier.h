// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#ifndef NTCORE_IENTRYNOTIFIER_H_
#define NTCORE_IENTRYNOTIFIER_H_

#include <climits>
#include <memory>

#include "ntcore_cpp.h"

namespace nt {

class IEntryNotifier {
 public:
  IEntryNotifier() = default;
  IEntryNotifier(const IEntryNotifier&) = delete;
  IEntryNotifier& operator=(const IEntryNotifier&) = delete;
  virtual ~IEntryNotifier() = default;
  virtual bool local_notifiers() const = 0;

  virtual unsigned int Add(
      std::function<void(const EntryNotification& event)> callback,
      wpi::StringRef prefix, unsigned int flags) = 0;
  virtual unsigned int Add(
      std::function<void(const EntryNotification& event)> callback,
      unsigned int local_id, unsigned int flags) = 0;
  virtual unsigned int AddPolled(unsigned int poller_uid, wpi::StringRef prefix,
                                 unsigned int flags) = 0;
  virtual unsigned int AddPolled(unsigned int poller_uid, unsigned int local_id,
                                 unsigned int flags) = 0;

  virtual void NotifyEntry(unsigned int local_id, wpi::StringRef name,
                           std::shared_ptr<Value> value, unsigned int flags,
                           unsigned int only_listener = UINT_MAX) = 0;
};

}  // namespace nt

#endif  // NTCORE_IENTRYNOTIFIER_H_
