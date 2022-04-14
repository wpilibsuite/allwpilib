// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "InstanceImpl.h"

using namespace nt;

std::atomic<int> InstanceImpl::s_default{-1};
std::atomic<InstanceImpl*> InstanceImpl::s_instances[kNumInstances];
wpi::mutex InstanceImpl::s_mutex;

using namespace std::placeholders;

InstanceImpl::InstanceImpl(int inst)
    : logger_impl(inst),
      logger(
          std::bind(&LoggerImpl::Log, &logger_impl, _1, _2, _3, _4)),  // NOLINT
      connection_notifier(inst),
      entry_notifier(inst, logger),
      rpc_server(inst, logger),
      storage(entry_notifier, rpc_server, logger),
      dispatcher(storage, connection_notifier, logger),
      ds_client(dispatcher, logger) {
  logger.set_min_level(logger_impl.GetMinLevel());
}

InstanceImpl::~InstanceImpl() {
  logger.SetLogger(nullptr);
}

InstanceImpl* InstanceImpl::GetDefault() {
  return Get(GetDefaultIndex());
}

InstanceImpl* InstanceImpl::Get(int inst) {
  if (inst < 0 || inst >= kNumInstances) {
    return nullptr;
  }
  return s_instances[inst];
}

int InstanceImpl::GetDefaultIndex() {
  int inst = s_default;
  if (inst >= 0) {
    return inst;
  }

  // slow path
  std::scoped_lock lock(s_mutex);

  // double-check
  inst = s_default;
  if (inst >= 0) {
    return inst;
  }

  // alloc and save
  inst = AllocImpl();
  s_default = inst;
  return inst;
}

int InstanceImpl::Alloc() {
  std::scoped_lock lock(s_mutex);
  return AllocImpl();
}

int InstanceImpl::AllocImpl() {
  int inst = 0;
  for (; inst < kNumInstances; ++inst) {
    if (!s_instances[inst]) {
      s_instances[inst] = new InstanceImpl(inst);
      return inst;
    }
  }
  return -1;
}

void InstanceImpl::Destroy(int inst) {
  std::scoped_lock lock(s_mutex);
  if (inst < 0 || inst >= kNumInstances) {
    return;
  }

  InstanceImpl* ptr = nullptr;
  s_instances[inst].exchange(ptr);
  delete ptr;
}
