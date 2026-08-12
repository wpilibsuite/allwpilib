// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.net;

/** Information about a device associated with a physical USB port. */
public class UsbDevice {
  /**
   * Constructs USB device information.
   *
   * @param port physical USB port index
   * @param syspath sysfs path of the device that generated the event
   * @param usbSyspath sysfs path of the physical USB device
   * @param subsystem kernel subsystem for the device that generated the event
   * @param deviceType kernel device type
   * @param deviceNode device node, such as /dev/ttyUSB0
   * @param vendorId USB vendor ID
   * @param productId USB product ID
   * @param manufacturer USB manufacturer name
   * @param product USB product name
   * @param serialNumber USB serial number
   */
  public UsbDevice(
      int port,
      String syspath,
      String usbSyspath,
      String subsystem,
      String deviceType,
      String deviceNode,
      String vendorId,
      String productId,
      String manufacturer,
      String product,
      String serialNumber) {
    m_port = port;
    m_syspath = syspath;
    m_usbSyspath = usbSyspath;
    m_subsystem = subsystem;
    m_deviceType = deviceType;
    m_deviceNode = deviceNode;
    m_vendorId = vendorId;
    m_productId = productId;
    m_manufacturer = manufacturer;
    m_product = product;
    m_serialNumber = serialNumber;
  }

  /** Returns the physical USB port index (0-3). */
  public int getPort() {
    return m_port;
  }

  /** Returns the sysfs path of the device that generated the event. */
  public String getSyspath() {
    return m_syspath;
  }

  /** Returns the sysfs path of the physical USB device. */
  public String getUsbSyspath() {
    return m_usbSyspath;
  }

  /** Returns the kernel subsystem for the device that generated the event. */
  public String getSubsystem() {
    return m_subsystem;
  }

  /** Returns the kernel device type, or an empty string if unavailable. */
  public String getDeviceType() {
    return m_deviceType;
  }

  /** Returns the device node, or an empty string if unavailable. */
  public String getDeviceNode() {
    return m_deviceNode;
  }

  /** Returns the USB vendor ID, or an empty string if unavailable. */
  public String getVendorId() {
    return m_vendorId;
  }

  /** Returns the USB product ID, or an empty string if unavailable. */
  public String getProductId() {
    return m_productId;
  }

  /** Returns the USB manufacturer name, or an empty string if unavailable. */
  public String getManufacturer() {
    return m_manufacturer;
  }

  /** Returns the USB product name, or an empty string if unavailable. */
  public String getProduct() {
    return m_product;
  }

  /** Returns the USB serial number, or an empty string if unavailable. */
  public String getSerialNumber() {
    return m_serialNumber;
  }

  private final int m_port;
  private final String m_syspath;
  private final String m_usbSyspath;
  private final String m_subsystem;
  private final String m_deviceType;
  private final String m_deviceNode;
  private final String m_vendorId;
  private final String m_productId;
  private final String m_manufacturer;
  private final String m_product;
  private final String m_serialNumber;
}
