package edu.wpi.cameraserver;

/// A source that represents a video camera.
public class CvSource extends VideoSource {
  /// Create an OpenCV source with a single channel.
  /// @param name Source name (arbitrary unique identifier)
  public CvSource(String name) {
    super(CameraServerJNI.createCvSource(name, 1));
  }

  /// Create an OpenCV source.
  /// @param name Source name (arbitrary unique identifier)
  /// @param numChannels Number of channels
  public CvSource(String name, int numChannels) {
    super(CameraServerJNI.createCvSource(name, numChannels));
  }

  /// Put an OpenCV image onto the specified channel.
  /// @param channel Channel number (range 0 to nChannels-1)
  /// @param image OpenCV image
  //public void putImage(int channel, Mat image);

  /// Signal sinks connected to this source that all new channel images have
  /// been put to the stream and are ready to be read.
  public void notifyFrame() {
    CameraServerJNI.notifySourceFrame(m_handle);
  }

  /// Put an OpenCV image onto channel 0 and notify sinks.
  /// This is identical in behavior to calling PutImage(0, image) followed by
  /// NotifyFrame().
  /// @param image OpenCV image
  //public void putFrame(Mat image);

  /// Signal sinks that an error has occurred.  This should be called instead
  /// of NotifyFrame when an error occurs.
  public void notifyError(String msg) {
    CameraServerJNI.notifySourceError(m_handle, msg);
  }

  /// Set source connection status.  Defaults to true.
  /// @param connected True for connected, false for disconnected
  public void setConnected(boolean connected) {
    CameraServerJNI.setSourceConnected(m_handle, connected);
  }

  /// Create a property.
  /// @param name Property name
  /// @param type Property type
  /// @return Property
  public VideoProperty createProperty(String name, VideoProperty.Type type) {
    return new VideoProperty(
        CameraServerJNI.createSourceProperty(m_handle, name, type.getValue()));
  }

  /// Create a property with a change callback.
  /// @param name Property name
  /// @param type Property type
  /// @param onChange Callback to call when the property value changes
  /// @return Property
  //public VideoProperty createProperty(
  //    String name, VideoProperty.Type type,
  //    std::function<void(String name, VideoProperty property)>
  //        onChange);

  /// Remove a property.
  /// @param name Property name
  public void removeProperty(VideoProperty property) {
    CameraServerJNI.removeSourceProperty(m_handle, property.m_handle);
  }

  /// Remove a property.
  /// @param name Property name
  public void removeProperty(String name) {
    CameraServerJNI.removeSourcePropertyByName(m_handle, name);
  }
}
