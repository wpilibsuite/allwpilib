// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#ifndef NTCORE_NETWORKTABLES_RPCCALL_H_
#define NTCORE_NETWORKTABLES_RPCCALL_H_

#include <string>
#include <utility>

#include "ntcore_c.h"

namespace nt {

class NetworkTableEntry;

/**
 * NetworkTables Remote Procedure Call
 * @ingroup ntcore_cpp_api
 */
class RpcCall final {
 public:
  /**
   * Construct invalid instance.
   */
  RpcCall() = default;

  /**
   * Construct from native handles.
   *
   * @param entry Entry handle
   * @param call  Call handle
   */
  RpcCall(NT_Entry entry, NT_RpcCall call) : m_entry(entry), m_call(call) {}

  RpcCall(RpcCall&& other) noexcept;
  RpcCall(const RpcCall&) = delete;
  RpcCall& operator=(const RpcCall&) = delete;

  /**
   * Destructor.  Cancels the result if no other action taken.
   */
  ~RpcCall();

  /**
   * Determines if the native handle is valid.
   *
   * @return True if the native handle is valid, false otherwise.
   */
  explicit operator bool() const { return m_call != 0; }

  /**
   * Get the RPC entry.
   *
   * @return NetworkTableEntry for the RPC.
   */
  NetworkTableEntry GetEntry() const;

  /**
   * Get the call native handle.
   *
   * @return Native handle.
   */
  NT_RpcCall GetCall() const { return m_call; }

  /**
   * Get the result (return value).  This function blocks until
   * the result is received.
   *
   * @param result      received result (output)
   * @return False on error, true otherwise.
   */
  bool GetResult(std::string* result);

  /**
   * Get the result (return value).  This function blocks until
   * the result is received or it times out.
   *
   * @param result      received result (output)
   * @param timeout     timeout, in seconds
   * @param timed_out   true if the timeout period elapsed (output)
   * @return False on error or timeout, true otherwise.
   */
  bool GetResult(std::string* result, double timeout, bool* timed_out);

  /**
   * Ignore the result.  This function is non-blocking.
   */
  void CancelResult();

  friend void swap(RpcCall& first, RpcCall& second) {
    using std::swap;
    swap(first.m_entry, second.m_entry);
    swap(first.m_call, second.m_call);
  }

 private:
  NT_Entry m_entry{0};
  NT_RpcCall m_call{0};
};

}  // namespace nt

#include "networktables/RpcCall.inl"

#endif  // NTCORE_NETWORKTABLES_RPCCALL_H_
