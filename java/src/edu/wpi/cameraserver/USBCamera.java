package edu.wpi.cameraserver;

/// A source that represents a USB camera.
public class USBCamera extends VideoSource {
  /// Create a source for a USB camera based on device number.
  /// @param name Source name (arbitrary unique identifier)
  /// @param dev Device number (e.g. 0 for /dev/video0)
  public USBCamera(String name, int dev) {
    super(CameraServerJNI.createUSBSourceDev(name, dev));
  }

  /// Create a source for a USB camera based on device path.
  /// @param name Source name (arbitrary unique identifier)
  /// @param path Path to device (e.g. "/dev/video0" on Linux)
  public USBCamera(String name, String path) {
    super(CameraServerJNI.createUSBSourcePath(name, path));
  }

  /// Enumerate USB cameras on the local system.
  /// @return Vector of USB camera information (one for each camera)
  public static USBCameraInfo[] enumerateUSBCameras() {
    return CameraServerJNI.enumerateUSBCameras();
  }
}
