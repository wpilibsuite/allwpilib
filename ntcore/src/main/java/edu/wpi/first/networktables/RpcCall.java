// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.networktables;

/**
 * NetworkTables Remote Procedure Call.
 */
public final class RpcCall implements AutoCloseable {
  /** Constructor.
   * This should generally only be used internally to NetworkTables.
   *
   * @param entry Entry
   * @param call Call handle
   */
  public RpcCall(NetworkTableEntry entry, int call) {
    m_entry = entry;
    m_call = call;
  }

  /**
   * Cancels the result if no other action taken.
   */
  @Override
  public synchronized void close() {
    if (m_call != 0) {
      cancelResult();
    }
  }

  /**
   * Determines if the native handle is valid.
   *
   * @return True if the native handle is valid, false otherwise.
   */
  public boolean isValid() {
    return m_call != 0;
  }

  /**
   * Get the RPC entry.
   *
   * @return NetworkTableEntry for the RPC.
   */
  public NetworkTableEntry getEntry() {
    return m_entry;
  }

  /**
   * Get the call native handle.
   *
   * @return Native handle.
   */
  public int getCall() {
    return m_call;
  }

  /**
   * Get the result (return value).  This function blocks until
   * the result is received.
   *
   * @return Received result (output)
   */
  public byte[] getResult() {
    byte[] result = NetworkTablesJNI.getRpcResult(m_entry.getHandle(), m_call);
    if (result.length != 0) {
      m_call = 0;
    }
    return result;
  }

  /**
   * Get the result (return value).  This function blocks until
   * the result is received or it times out.
   *
   * @param timeout     timeout, in seconds
   * @return Received result (output)
   */
  public byte[] getResult(double timeout) {
    byte[] result = NetworkTablesJNI.getRpcResult(m_entry.getHandle(), m_call, timeout);
    if (result.length != 0) {
      m_call = 0;
    }
    return result;
  }

  /**
   * Ignore the result.  This function is non-blocking.
   */
  public void cancelResult() {
    NetworkTablesJNI.cancelRpcResult(m_entry.getHandle(), m_call);
  }

  private final NetworkTableEntry m_entry;
  private int m_call;
}
