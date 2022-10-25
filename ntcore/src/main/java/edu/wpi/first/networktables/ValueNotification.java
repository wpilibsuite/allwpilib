// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.networktables;

/** NetworkTables value notification. */
@SuppressWarnings("MemberName")
public final class ValueNotification extends NotificationBase {
  /** Topic handle. Topic.getHandle() can be used to map this to the corresponding Topic object. */
  public final int topic;

  /**
   * Subscriber/entry handle. Subscriber.getHandle() or entry.getHandle() can be used to map this to
   * the corresponding Subscriber or Entry object.
   */
  public final int subentry;

  /** The new value. */
  public final NetworkTableValue value;

  /** Update flags. */
  public final int flags;

  /**
   * Constructor. This should generally only be used internally to NetworkTables.
   *
   * @param inst Instance
   * @param listener Listener that was triggered
   * @param topic Topic handle
   * @param subentry Subscriber/entry handle
   * @param value The new value
   * @param flags Update flags
   */
  public ValueNotification(
      NetworkTableInstance inst,
      int listener,
      int topic,
      int subentry,
      NetworkTableValue value,
      int flags) {
    super(inst, listener);
    this.topic = topic;
    this.subentry = subentry;
    this.value = value;
    this.flags = flags;
  }

  /* Cached topic object. */
  Topic m_topicObject;

  /**
   * Get the topic as an object.
   *
   * @return Topic for this notification.
   */
  public Topic getTopic() {
    if (m_topicObject == null) {
      m_topicObject = new Topic(m_inst, topic);
    }
    return m_topicObject;
  }
}
