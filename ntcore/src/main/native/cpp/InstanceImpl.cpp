/*----------------------------------------------------------------------------*/
/* Copyright (c) 2015-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "InstanceImpl.h"

using namespace nt;

std::atomic<int> InstanceImpl::s_default{-1};
std::atomic<InstanceImpl*> InstanceImpl::s_fast_instances[10];
wpi::UidVector<InstanceImpl*, 10> InstanceImpl::s_instances;
wpi::mutex InstanceImpl::s_mutex;

using namespace std::placeholders;

InstanceImpl::InstanceImpl(int inst)
    : logger_impl(inst),
      logger(std::bind(&LoggerImpl::Log, &logger_impl, _1, _2, _3, _4)),
      connection_notifier(inst),
      entry_notifier(inst, logger),
      rpc_server(inst, logger),
      storage(entry_notifier, rpc_server, logger),
      dispatcher(storage, connection_notifier, logger),
      ds_client(dispatcher, logger) {
  logger.set_min_level(logger_impl.GetMinLevel());
}

InstanceImpl::~InstanceImpl() { logger.SetLogger(nullptr); }

InstanceImpl* InstanceImpl::GetDefault() { return Get(GetDefaultIndex()); }

InstanceImpl* InstanceImpl::Get(int inst) {
  if (inst < 0) return nullptr;

  // fast path, just an atomic read
  if (static_cast<unsigned int>(inst) <
      (sizeof(s_fast_instances) / sizeof(s_fast_instances[0]))) {
    InstanceImpl* ptr = s_fast_instances[inst];
    if (ptr) return ptr;
  }

  // slow path
  std::lock_guard<wpi::mutex> lock(s_mutex);

  // static fast-path block
  if (static_cast<unsigned int>(inst) <
      (sizeof(s_fast_instances) / sizeof(s_fast_instances[0]))) {
    // double-check
    return s_fast_instances[inst];
  }

  // vector
  if (static_cast<unsigned int>(inst) < s_instances.size()) {
    return s_instances[inst];
  }

  // doesn't exist
  return nullptr;
}

int InstanceImpl::GetDefaultIndex() {
  int inst = s_default;
  if (inst >= 0) return inst;

  // slow path
  std::lock_guard<wpi::mutex> lock(s_mutex);

  // double-check
  inst = s_default;
  if (inst >= 0) return inst;

  // alloc and save
  inst = AllocImpl();
  s_default = inst;
  return inst;
}

int InstanceImpl::Alloc() {
  std::lock_guard<wpi::mutex> lock(s_mutex);
  return AllocImpl();
}

int InstanceImpl::AllocImpl() {
  unsigned int inst = s_instances.emplace_back(nullptr);
  InstanceImpl* ptr = new InstanceImpl(inst);
  s_instances[inst] = ptr;

  if (inst < (sizeof(s_fast_instances) / sizeof(s_fast_instances[0]))) {
    s_fast_instances[inst] = ptr;
  }

  return static_cast<int>(inst);
}

void InstanceImpl::Destroy(int inst) {
  std::lock_guard<wpi::mutex> lock(s_mutex);
  if (inst < 0 || static_cast<unsigned int>(inst) >= s_instances.size()) return;

  if (static_cast<unsigned int>(inst) <
      (sizeof(s_fast_instances) / sizeof(s_fast_instances[0]))) {
    s_fast_instances[inst] = nullptr;
  }

  delete s_instances[inst];
  s_instances.erase(inst);
}
