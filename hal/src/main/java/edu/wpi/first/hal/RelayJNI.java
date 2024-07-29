// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.hal;

/**
 * Relay Output HAL JNI Functions.
 *
 * @see "hal/Relay.h"
 */
public class RelayJNI extends DIOJNI {
  /**
   * Initializes a relay.
   *
   * <p>Note this call will only initialize either the forward or reverse port of the relay. If you
   * need both, you will need to initialize 2 relays.
   *
   * @param halPortHandle the port handle to initialize
   * @param forward true for the forward port, false for the reverse port
   * @return the created relay handle
   * @see "HAL_InitializeRelayPort"
   */
  public static native int initializeRelayPort(int halPortHandle, boolean forward);

  /**
   * Frees a relay port.
   *
   * @param relayPortHandle the relay handle
   * @see "HAL_FreeRelayPort"
   */
  public static native void freeRelayPort(int relayPortHandle);

  /**
   * Checks if a relay channel is valid.
   *
   * @param channel the channel to check
   * @return true if the channel is valid, otherwise false
   * @see "HAL_CheckRelayChannel"
   */
  public static native boolean checkRelayChannel(int channel);

  /**
   * Sets the state of a relay output.
   *
   * @param relayPortHandle the relay handle
   * @param on true for on, false for off
   * @see "HAL_SetRelay"
   */
  public static native void setRelay(int relayPortHandle, boolean on);

  /**
   * Gets the current state of the relay channel.
   *
   * @param relayPortHandle the relay handle
   * @return true for on, false for off
   * @see "HAL_GetRelay"
   */
  public static native boolean getRelay(int relayPortHandle);

  /** Utility class. */
  private RelayJNI() {}
}
