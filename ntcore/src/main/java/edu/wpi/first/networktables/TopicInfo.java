// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.networktables;

/** NetworkTables topic information. */
@SuppressWarnings("MemberName")
public final class TopicInfo {
  /** Topic handle. */
  public final int topic;

  /** Topic name. */
  public final String name;

  /** Topic type. */
  public final NetworkTableType type;

  /** Topic type string. */
  public final String typeStr;

  /**
   * Constructor. This should generally only be used internally to NetworkTables.
   *
   * @param inst Instance
   * @param handle Topic handle
   * @param name Name
   * @param type Type (integer version of {@link NetworkTableType})
   * @param typeStr Type string
   */
  public TopicInfo(NetworkTableInstance inst, int handle, String name, int type, String typeStr) {
    this.m_inst = inst;
    this.topic = handle;
    this.name = name;
    this.type = NetworkTableType.getFromInt(type);
    this.typeStr = typeStr;
  }

  /* Network table instance. */
  private final NetworkTableInstance m_inst;

  /* Cached topic object. */
  private Topic m_topicObject;

  /**
   * Get the instance.
   *
   * @return Instance
   */
  public NetworkTableInstance getInstance() {
    return m_inst;
  }

  /**
   * Get the topic as an object.
   *
   * @return Topic
   */
  public Topic getTopic() {
    if (m_topicObject == null) {
      m_topicObject = new Topic(m_inst, topic);
    }
    return m_topicObject;
  }
}
