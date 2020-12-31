// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#ifndef NTCORE_NETWORKTABLES_RPCCALL_INL_
#define NTCORE_NETWORKTABLES_RPCCALL_INL_

#include <string>
#include <utility>

#include "networktables/RpcCall.h"
#include "ntcore_cpp.h"

namespace nt {

inline RpcCall::RpcCall(RpcCall&& other) noexcept : RpcCall() {
  swap(*this, other);
}

inline RpcCall::~RpcCall() {
  // automatically cancel result if user didn't request it
  if (m_call != 0) {
    CancelResult();
  }
}

inline bool RpcCall::GetResult(std::string* result) {
  if (GetRpcResult(m_entry, m_call, result)) {
    m_call = 0;
    return true;
  }
  return false;
}

inline bool RpcCall::GetResult(std::string* result, double timeout,
                               bool* timed_out) {
  if (GetRpcResult(m_entry, m_call, result, timeout, timed_out)) {
    m_call = 0;
    return true;
  }
  return false;
}

inline void RpcCall::CancelResult() {
  CancelRpcResult(m_entry, m_call);
  m_call = 0;
}

}  // namespace nt

#endif  // NTCORE_NETWORKTABLES_RPCCALL_INL_
