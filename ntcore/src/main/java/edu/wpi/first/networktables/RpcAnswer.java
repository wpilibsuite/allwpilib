/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.networktables;

/**
 * NetworkTables Remote Procedure Call (Server Side).
 */
public final class RpcAnswer implements AutoCloseable {
  /** Entry handle. */
  @SuppressWarnings("MemberName")
  public final int entry;

  /** Call handle. */
  @SuppressWarnings("MemberName")
  public int call;

  /** Entry name. */
  @SuppressWarnings("MemberName")
  public final String name;

  /** Call raw parameters. */
  @SuppressWarnings("MemberName")
  public final String params;

  /** Connection that called the RPC. */
  @SuppressWarnings("MemberName")
  public final ConnectionInfo conn;

  /** Constructor.
   * This should generally only be used internally to NetworkTables.
   * @param inst Instance
   * @param entry Entry handle
   * @param call Call handle
   * @param name Entry name
   * @param params Call raw parameters
   * @param conn Connection info
   */
  public RpcAnswer(NetworkTableInstance inst, int entry, int call, String name, String params,
                   ConnectionInfo conn) {
    this.m_inst = inst;
    this.entry = entry;
    this.call = call;
    this.name = name;
    this.params = params;
    this.conn = conn;
  }

  static final byte[] emptyResponse = new byte[] {};

  @Deprecated
  public void free() {
    close();
  }

  /**
   * Posts an empty response if one was not previously sent.
   */
  @Override
  public synchronized void close() {
    if (call != 0) {
      postResponse(emptyResponse);
    }
  }

  /**
   * Determines if the native handle is valid.
   * @return True if the native handle is valid, false otherwise.
   */
  public boolean isValid() {
    return call != 0;
  }

  /**
   * Post RPC response (return value) for a polled RPC.
   * @param result  result raw data that will be provided to remote caller
   */
  public void postResponse(byte[] result) {
    NetworkTablesJNI.postRpcResponse(entry, call, result);
    call = 0;
  }

  /* Network table instance. */
  private final NetworkTableInstance m_inst;

  /* Cached entry object. */
  NetworkTableEntry m_entryObject;

  /**
   * Get the entry as an object.
   * @return NetworkTableEntry for the RPC.
   */
  NetworkTableEntry getEntry() {
    if (m_entryObject == null) {
      m_entryObject = new NetworkTableEntry(m_inst, entry);
    }
    return m_entryObject;
  }
}
