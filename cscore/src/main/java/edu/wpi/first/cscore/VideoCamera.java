// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.cscore;

/** A source that represents a video camera. */
public class VideoCamera extends VideoSource {
  /** White balance. */
  public static class WhiteBalance {
    /** Fixed indoor white balance. */
    public static final int kFixedIndoor = 3000;

    /** Fixed outdoor white balance 1. */
    public static final int kFixedOutdoor1 = 4000;

    /** Fixed outdoor white balance 2. */
    public static final int kFixedOutdoor2 = 5000;

    /** Fixed fluorescent white balance 1. */
    public static final int kFixedFluorescent1 = 5100;

    /** Fixed fluorescent white balance 2. */
    public static final int kFixedFlourescent2 = 5200;

    /** Default constructor. */
    public WhiteBalance() {}
  }

  /**
   * Constructs a VideoCamera.
   *
   * @param handle The video camera handle.
   */
  protected VideoCamera(int handle) {
    super(handle);
  }

  /**
   * Set the brightness, as a percentage (0-100).
   *
   * @param brightness Brightness as a percentage (0-100).
   */
  public synchronized void setBrightness(int brightness) {
    CameraServerJNI.setCameraBrightness(m_handle, brightness);
  }

  /**
   * Get the brightness, as a percentage (0-100).
   *
   * @return The brightness as a percentage (0-100).
   */
  public synchronized int getBrightness() {
    return CameraServerJNI.getCameraBrightness(m_handle);
  }

  /** Set the white balance to auto. */
  public synchronized void setWhiteBalanceAuto() {
    CameraServerJNI.setCameraWhiteBalanceAuto(m_handle);
  }

  /** Set the white balance to hold current. */
  public synchronized void setWhiteBalanceHoldCurrent() {
    CameraServerJNI.setCameraWhiteBalanceHoldCurrent(m_handle);
  }

  /**
   * Set the white balance to manual, with specified color temperature.
   *
   * @param value The specified color temperature.
   */
  public synchronized void setWhiteBalanceManual(int value) {
    CameraServerJNI.setCameraWhiteBalanceManual(m_handle, value);
  }

  /** Set the exposure to auto aperture. */
  public synchronized void setExposureAuto() {
    CameraServerJNI.setCameraExposureAuto(m_handle);
  }

  /** Set the exposure to hold current. */
  public synchronized void setExposureHoldCurrent() {
    CameraServerJNI.setCameraExposureHoldCurrent(m_handle);
  }

  /**
   * Set the exposure to manual, as a percentage (0-100).
   *
   * @param value The exposure as a percentage (0-100).
   */
  public synchronized void setExposureManual(int value) {
    CameraServerJNI.setCameraExposureManual(m_handle, value);
  }
}
