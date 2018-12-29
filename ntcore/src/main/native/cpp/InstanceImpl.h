/*----------------------------------------------------------------------------*/
/* Copyright (c) 2016-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#ifndef NTCORE_INSTANCEIMPL_H_
#define NTCORE_INSTANCEIMPL_H_

#include <atomic>
#include <memory>

#include <wpi/UidVector.h>
#include <wpi/mutex.h>

#include "ConnectionNotifier.h"
#include "Dispatcher.h"
#include "DsClient.h"
#include "EntryNotifier.h"
#include "Log.h"
#include "LoggerImpl.h"
#include "RpcServer.h"
#include "Storage.h"

namespace nt {

class InstanceImpl {
 public:
  explicit InstanceImpl(int inst);
  ~InstanceImpl();

  // Instance repository
  static InstanceImpl* GetDefault();
  static InstanceImpl* Get(int inst);
  static int GetDefaultIndex();
  static int Alloc();
  static void Destroy(int inst);

  LoggerImpl logger_impl;
  wpi::Logger logger;
  ConnectionNotifier connection_notifier;
  EntryNotifier entry_notifier;
  RpcServer rpc_server;
  Storage storage;
  Dispatcher dispatcher;
  DsClient ds_client;

 private:
  static int AllocImpl();

  static std::atomic<int> s_default;
  static std::atomic<InstanceImpl*> s_fast_instances[10];
  static wpi::UidVector<InstanceImpl*, 10> s_instances;
  static wpi::mutex s_mutex;
};

}  // namespace nt

#endif  // NTCORE_INSTANCEIMPL_H_
