/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2015. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#ifndef NT_ISTORAGE_H_
#define NT_ISTORAGE_H_

#include <functional>
#include <memory>
#include <vector>

#include "llvm/ArrayRef.h"
#include "llvm/StringRef.h"

#include "Message.h"
#include "ntcore_cpp.h"

namespace nt {

class IDispatcher;
class NetworkConnection;

class IStorage {
 public:
  IStorage() = default;
  IStorage(const IStorage&) = delete;
  IStorage& operator=(const IStorage&) = delete;
  virtual ~IStorage() = default;

  // Accessors required by Dispatcher.  An interface is used for
  // generation of outgoing messages to break a dependency loop between
  // Storage and Dispatcher.
  virtual void SetDispatcher(IDispatcher* dispatcher, bool server) = 0;
  virtual void ClearDispatcher() = 0;

  // Required for wire protocol 2.0 to get the entry type of an entry when
  // receiving entry updates (because the length/type is not provided in the
  // message itself).  Not used in wire protocol 3.0.
  virtual NT_Type GetMessageEntryType(unsigned int id) const = 0;

  virtual void ProcessIncoming(std::shared_ptr<Message> msg,
                               NetworkConnection* conn,
                               std::weak_ptr<NetworkConnection> conn_weak) = 0;
  virtual void GetInitialAssignments(
      NetworkConnection& conn, std::vector<std::shared_ptr<Message>>* msgs) = 0;
  virtual void ApplyInitialAssignments(
      NetworkConnection& conn, llvm::ArrayRef<std::shared_ptr<Message>> msgs,
      bool new_server, std::vector<std::shared_ptr<Message>>* out_msgs) = 0;

  // Filename-based save/load functions.  Used both by periodic saves and
  // accessible directly via the user API.
  virtual const char* SavePersistent(StringRef filename,
                                     bool periodic) const = 0;
  virtual const char* LoadPersistent(
      StringRef filename,
      std::function<void(std::size_t line, const char* msg)> warn) = 0;
};

}  // namespace nt

#endif  // NT_ISTORAGE_H_
