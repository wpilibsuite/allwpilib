/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2016. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.cscore;

/// A source that represents a USB camera.
public class USBCamera extends VideoSource {
  private static final String kPropWbAuto = "white_balance_temperature_auto";
  private static final String kPropWbValue = "white_balance_temperature";
  private static final String kPropExAuto = "exposure_auto";
  private static final String kPropExValue = "exposure_absolute";
  private static final String kPropBrValue = "brightness";

  public class WhiteBalance {
    public static final int kFixedIndoor = 3000;
    public static final int kFixedOutdoor1 = 4000;
    public static final int kFixedOutdoor2 = 5000;
    public static final int kFixedFluorescent1 = 5100;
    public static final int kFixedFlourescent2 = 5200;
  }

  // Cached to avoid duplicate string lookups
  private VideoProperty m_wbAuto;
  private VideoProperty m_wbValue;
  private VideoProperty m_exAuto;
  private VideoProperty m_exValue;
  private VideoProperty m_brValue;

  /// Create a source for a USB camera based on device number.
  /// @param name Source name (arbitrary unique identifier)
  /// @param dev Device number (e.g. 0 for /dev/video0)
  public USBCamera(String name, int dev) {
    super(CameraServerJNI.createUSBCameraDev(name, dev));
  }

  /// Create a source for a USB camera based on device path.
  /// @param name Source name (arbitrary unique identifier)
  /// @param path Path to device (e.g. "/dev/video0" on Linux)
  public USBCamera(String name, String path) {
    super(CameraServerJNI.createUSBCameraPath(name, path));
  }

  /// Enumerate USB cameras on the local system.
  /// @return Vector of USB camera information (one for each camera)
  public static USBCameraInfo[] enumerateUSBCameras() {
    return CameraServerJNI.enumerateUSBCameras();
  }

  /// Get the path to the device.
  public String getPath() {
    return CameraServerJNI.getUSBCameraPath(m_handle);
  }

  /// Set the brightness, as a percentage (0-100).
  public synchronized void setBrightness(int brightness) {
    if (brightness > 100) {
      brightness = 100;
    } else if (brightness < 0) {
      brightness = 0;
    }
    if (m_brValue == null) m_brValue = getProperty(kPropBrValue);
    m_brValue.set(brightness);
  }

  /// Get the brightness, as a percentage (0-100).
  public synchronized int getBrightness() {
    if (m_brValue == null) m_brValue = getProperty(kPropBrValue);
    return m_brValue.get();
  }

  /// Set the white balance to auto.
  public synchronized void setWhiteBalanceAuto() {
    if (m_wbAuto == null) m_wbAuto = getProperty(kPropWbAuto);
    m_wbAuto.set(1);  // auto
  }

  /// Set the white balance to hold current.
  public synchronized void setWhiteBalanceHoldCurrent() {
    if (m_wbAuto == null) m_wbAuto = getProperty(kPropWbAuto);
    m_wbAuto.set(0);  // manual
  }

  /// Set the white balance to manual, with specified color temperature.
  public synchronized void setWhiteBalanceManual(int value) {
    if (m_wbAuto == null) m_wbAuto = getProperty(kPropWbAuto);
    m_wbAuto.set(0);  // manual
    if (m_wbValue == null) m_wbValue = getProperty(kPropWbValue);
    m_wbValue.set(value);
  }

  /// Set the exposure to auto aperature.
  public synchronized void setExposureAuto() {
    if (m_exAuto == null) m_exAuto = getProperty(kPropExAuto);
    m_exAuto.set(0);  // auto; yes, this is opposite of white balance.
  }

  /// Set the exposure to hold current.
  public synchronized void setExposureHoldCurrent() {
    if (m_exAuto == null) m_exAuto = getProperty(kPropExAuto);
    m_exAuto.set(1);  // manual
  }

  /// Set the exposure to manual, as a percentage (0-100).
  public synchronized void setExposureManual(int value) {
    if (m_exAuto == null) m_exAuto = getProperty(kPropExAuto);
    m_exAuto.set(1);  // manual
    if (value > 100) {
      value = 100;
    } else if (value < 0) {
      value = 0;
    }
    if (m_exValue == null) m_exValue = getProperty(kPropExValue);
    m_exValue.set(value);
  }
}
