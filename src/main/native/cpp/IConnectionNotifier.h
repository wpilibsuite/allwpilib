/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2017. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#ifndef NT_ICONNECTIONNOTIFIER_H_
#define NT_ICONNECTIONNOTIFIER_H_

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

#endif  // NT_ICONNECTIONNOTIFIER_H_
