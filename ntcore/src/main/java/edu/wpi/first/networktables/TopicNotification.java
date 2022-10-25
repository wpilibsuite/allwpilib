// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.networktables;

/** NetworkTables topic notification. */
@SuppressWarnings("MemberName")
public final class TopicNotification extends NotificationBase {
  /** Topic information. */
  public final TopicInfo info;

  /**
   * Update flags. For example, {@link TopicListenerFlags#kPublish} if the topic was not previously
   * published.
   */
  public final int flags;

  /**
   * Constructor. This should generally only be used internally to NetworkTables.
   *
   * @param listener Listener that was triggered
   * @param info Topic information
   * @param flags Update flags
   */
  public TopicNotification(int listener, TopicInfo info, int flags) {
    super(info.getInstance(), listener);
    this.info = info;
    this.flags = flags;
  }
}
