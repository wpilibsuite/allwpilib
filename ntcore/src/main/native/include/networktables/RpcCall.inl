/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2017. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#ifndef NT_RPCCALL_INL_
#define NT_RPCCALL_INL_

#include "ntcore_cpp.h"

namespace nt {

inline RpcCall::RpcCall(RpcCall&& other) : RpcCall() {
  swap(*this, other);
}

inline RpcCall::~RpcCall() {
  // automatically cancel result if user didn't request it
  if (m_call != 0) CancelResult();
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

#endif  // NT_RPCCALL_INL_
