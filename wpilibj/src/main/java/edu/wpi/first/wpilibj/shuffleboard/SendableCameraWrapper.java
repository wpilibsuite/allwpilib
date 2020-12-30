// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj.shuffleboard;

import edu.wpi.cscore.VideoSource;
import edu.wpi.first.wpilibj.Sendable;
import edu.wpi.first.wpilibj.smartdashboard.SendableBuilder;
import edu.wpi.first.wpilibj.smartdashboard.SendableRegistry;
import java.util.Map;
import java.util.WeakHashMap;

/** A wrapper to make video sources sendable and usable from Shuffleboard. */
public final class SendableCameraWrapper implements Sendable, AutoCloseable {
  private static final String kProtocol = "camera_server://";

  private static Map<VideoSource, SendableCameraWrapper> m_wrappers = new WeakHashMap<>();

  private final String m_uri;

  /**
   * Creates a new sendable wrapper. Private constructor to avoid direct instantiation with multiple
   * wrappers floating around for the same camera.
   *
   * @param source the source to wrap
   */
  private SendableCameraWrapper(VideoSource source) {
    String name = source.getName();
    SendableRegistry.add(this, name);
    m_uri = kProtocol + name;
  }

  @Override
  public void close() {
    SendableRegistry.remove(this);
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
    return m_wrappers.computeIfAbsent(source, SendableCameraWrapper::new);
  }

  @Override
  public void initSendable(SendableBuilder builder) {
    builder.addStringProperty(".ShuffleboardURI", () -> m_uri, null);
  }
}
