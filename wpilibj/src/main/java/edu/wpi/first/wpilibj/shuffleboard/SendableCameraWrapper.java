// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj.shuffleboard;

import edu.wpi.first.cscore.VideoSource;
import edu.wpi.first.networktables.NetworkTable;
import edu.wpi.first.networktables.NetworkTableInstance;
import edu.wpi.first.networktables.StringArrayPublisher;
import edu.wpi.first.networktables.StringArrayTopic;
import edu.wpi.first.util.sendable.Sendable;
import edu.wpi.first.util.sendable.SendableBuilder;
import edu.wpi.first.util.sendable.SendableRegistry;
import java.util.Map;
import java.util.Objects;
import java.util.WeakHashMap;

/** A wrapper to make video sources sendable and usable from Shuffleboard. */
public final class SendableCameraWrapper implements Sendable, AutoCloseable {
  private static final String kProtocol = "camera_server://";

  private static Map<String, SendableCameraWrapper> m_wrappers = new WeakHashMap<>();

  private static NetworkTable m_table;

  static {
    setNetworkTableInstance(NetworkTableInstance.getDefault());
  }

  private final String m_uri;
  private StringArrayPublisher m_streams;

  /**
   * Creates a new sendable wrapper. Private constructor to avoid direct instantiation with multiple
   * wrappers floating around for the same camera.
   *
   * @param source the source to wrap
   */
  private SendableCameraWrapper(VideoSource source) {
    this(source.getName());
  }

  private SendableCameraWrapper(String cameraName) {
    SendableRegistry.add(this, cameraName);
    m_uri = kProtocol + cameraName;
  }

  private SendableCameraWrapper(String cameraName, String[] cameraUrls) {
    this(cameraName);

    StringArrayTopic streams = new StringArrayTopic(m_table.getTopic(cameraName + "/streams"));
    if (streams.exists()) {
      throw new IllegalStateException(
          "A camera is already being streamed with the name '" + cameraName + "'");
    }

    m_streams = streams.publish();
    m_streams.set(cameraUrls);
  }

  /** Clears all cached wrapper objects. This should only be used in tests. */
  static void clearWrappers() {
    m_wrappers.clear();
  }

  @Override
  public void close() {
    SendableRegistry.remove(this);
    if (m_streams != null) {
      m_streams.close();
    }
  }

  /*
   * Sets NetworkTable instance used for camera publisher entries.
   *
   * @param inst NetworkTable instance
   */
  public static synchronized void setNetworkTableInstance(NetworkTableInstance inst) {
    m_table = inst.getTable("CameraPublisher");
  }

  /**
   * Gets a sendable wrapper object for the given video source, creating the wrapper if one does not
   * already exist for the source.
   *
   * @param source the video source to wrap
   * @return a sendable wrapper object for the video source, usable in Shuffleboard via {@link
   *     ShuffleboardTab#add(Sendable)} and {@link ShuffleboardLayout#add(Sendable)}
   */
  public static SendableCameraWrapper wrap(VideoSource source) {
    return m_wrappers.computeIfAbsent(source.getName(), name -> new SendableCameraWrapper(source));
  }

  /**
   * Creates a wrapper for an arbitrary camera stream. The stream URLs <i>must</i> be specified
   * using a host resolvable by a program running on a different host (such as a dashboard); prefer
   * using static IP addresses (if known) or DHCP identifiers such as {@code "raspberrypi.local"}.
   *
   * <p>If a wrapper already exists for the given camera, that wrapper is returned and the specified
   * URLs are ignored.
   *
   * @param cameraName the name of the camera. Cannot be null or empty
   * @param cameraUrls the URLs with which the camera stream may be accessed. At least one URL must
   *     be specified
   * @return a sendable wrapper object for the video source, usable in Shuffleboard via {@link
   *     ShuffleboardTab#add(Sendable)} and {@link ShuffleboardLayout#add(Sendable)}
   */
  public static SendableCameraWrapper wrap(String cameraName, String... cameraUrls) {
    if (m_wrappers.containsKey(cameraName)) {
      return m_wrappers.get(cameraName);
    }

    Objects.requireNonNull(cameraName, "cameraName");
    Objects.requireNonNull(cameraUrls, "cameraUrls");
    if (cameraName.isEmpty()) {
      throw new IllegalArgumentException("Camera name not specified");
    }
    if (cameraUrls.length == 0) {
      throw new IllegalArgumentException("No camera URLs specified");
    }
    for (int i = 0; i < cameraUrls.length; i++) {
      Objects.requireNonNull(cameraUrls[i], "Camera URL at index " + i + " was null");
    }

    SendableCameraWrapper wrapper = new SendableCameraWrapper(cameraName, cameraUrls);
    m_wrappers.put(cameraName, wrapper);
    return wrapper;
  }

  @Override
  public void initSendable(SendableBuilder builder) {
    builder.addStringProperty(".ShuffleboardURI", () -> m_uri, null);
  }
}
