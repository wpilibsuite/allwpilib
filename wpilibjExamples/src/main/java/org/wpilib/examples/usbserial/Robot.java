// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.examples.usbserial;

import org.wpilib.framework.TimedRobot;
import org.wpilib.hardware.bus.SerialPort;
import org.wpilib.net.UsbDevice;
import org.wpilib.net.UsbDeviceDetector;

/** Opens and closes a serial port as a USB serial device is connected and disconnected. */
public class Robot extends TimedRobot {
  private final Object serialLock = new Object();
  private final UsbDeviceDetector detector;
  private SerialPort serial;
  private String serialPath = "";

  /** Creates the USB detector for physical USB port 0. */
  public Robot() {
    detector = new UsbDeviceDetector(0, this::deviceConnected, this::deviceDisconnected);
    if (!detector.start()) {
      System.err.println("USB device detection is unavailable");
    }
  }

  @Override
  public void robotPeriodic() {
    // Detector callbacks execute on a worker thread, so protect SerialPort use
    // with the same lock used when the callbacks open and close it.
    synchronized (serialLock) {
      if (serial == null) {
        return;
      }

      try {
        int available = serial.getBytesReceived();
        if (available > 0) {
          byte[] data = serial.read(Math.min(available, 256));
          System.out.write(data, 0, data.length);
          System.out.flush();
        }
      } catch (RuntimeException exception) {
        System.err.println("Serial read failed: " + exception.getMessage());
      }
    }
  }

  private void deviceConnected(UsbDevice device) {
    // The detector is generic. A USB serial adapter has a child device in the
    // tty subsystem; cameras, HID devices, and storage devices have different
    // subsystem values and can be handled by other consumers.
    String deviceNode = device.getDeviceNode();
    if (!device.getSubsystem().equals("tty") || deviceNode.isEmpty()) {
      return;
    }

    synchronized (serialLock) {
      if (serial != null) {
        return;
      }

      try {
        serial = new SerialPort(115200, deviceNode);
        serialPath = deviceNode;
        System.out.println("Opened serial device " + serialPath);
      } catch (RuntimeException exception) {
        System.err.println("Could not open " + deviceNode + ": " + exception.getMessage());
      }
    }
  }

  private void deviceDisconnected(UsbDevice device) {
    synchronized (serialLock) {
      if (serial == null || !device.getDeviceNode().equals(serialPath)) {
        return;
      }

      serial.close();
      serial = null;
      serialPath = "";
      System.out.println("Closed disconnected serial device");
    }
  }

  @Override
  public void close() {
    // Stop the callback thread before releasing state used by the callbacks.
    detector.close();
    synchronized (serialLock) {
      if (serial != null) {
        serial.close();
        serial = null;
      }
    }
    super.close();
  }
}
