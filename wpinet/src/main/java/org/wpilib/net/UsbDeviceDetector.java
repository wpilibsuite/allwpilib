// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.net;

import java.lang.ref.Cleaner.Cleanable;
import java.util.Objects;
import java.util.function.Consumer;
import org.wpilib.util.cleanup.WPICleaner;

/** Detects devices associated with one of the four physical SystemCore USB ports. */
public class UsbDeviceDetector implements AutoCloseable {
  private final int m_handle;
  private final Cleanable m_cleanable;

  private static Runnable cleanupAction(int handle) {
    return () -> WPINetJNI.freeUsbDeviceDetector(handle);
  }

  /**
   * Constructs a USB device detector.
   *
   * <p>Callbacks execute on an internal worker thread. They must not call {@link #start()}, {@link
   * #stop()}, or {@link #close()} on this detector.
   *
   * @param port physical USB port index (0-3)
   * @param connected called when a matching device is connected; devices already present are
   *     reported when {@link #start()} is called
   * @param disconnected called when a previously reported device is removed
   * @throws IllegalArgumentException if port is outside the range 0-3
   * @throws NullPointerException if either callback is null
   */
  @SuppressWarnings("this-escape")
  public UsbDeviceDetector(
      int port, Consumer<UsbDevice> connected, Consumer<UsbDevice> disconnected) {
    if (port < 0 || port > 3) {
      throw new IllegalArgumentException("USB port must be in the range 0-3");
    }
    m_handle =
        WPINetJNI.createUsbDeviceDetector(
            port,
            Objects.requireNonNull(connected, "connected"),
            Objects.requireNonNull(disconnected, "disconnected"));
    m_cleanable = WPICleaner.register(this, cleanupAction(m_handle));
  }

  /** Starts device detection. */
  public boolean start() {
    return WPINetJNI.startUsbDeviceDetector(m_handle);
  }

  /** Stops device detection. */
  public void stop() {
    WPINetJNI.stopUsbDeviceDetector(m_handle);
  }

  /** Returns whether device detection is running. */
  public boolean isRunning() {
    return WPINetJNI.isUsbDeviceDetectorRunning(m_handle);
  }

  /** Returns whether a SystemCore implementation is available. */
  public boolean hasImplementation() {
    return WPINetJNI.getUsbDeviceDetectorHasImplementation(m_handle);
  }

  @Override
  public void close() {
    m_cleanable.clean();
  }
}
