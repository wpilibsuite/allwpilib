// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#ifndef NTCORE_ICONNECTIONNOTIFIER_H_
#define NTCORE_ICONNECTIONNOTIFIER_H_

#include <climits>

#include "ntcore_cpp.h"

namespace nt {

class IConnectionNotifier {
 public:
  IConnectionNotifier() = default;
  IConnectionNotifier(const IConnectionNotifier&) = delete;
  IConnectionNotifier& operator=(const IConnectionNotifier&) = delete;
  virtual ~IConnectionNotifier() = default;
  virtual unsigned int Add(
      std::function<void(const ConnectionNotification& event)> callback) = 0;
  virtual unsigned int AddPolled(unsigned int poller_uid) = 0;
  virtual void NotifyConnection(bool connected, const ConnectionInfo& conn_info,
                                unsigned int only_listener = UINT_MAX) = 0;
};

}  // namespace nt

#endif  // NTCORE_ICONNECTIONNOTIFIER_H_
