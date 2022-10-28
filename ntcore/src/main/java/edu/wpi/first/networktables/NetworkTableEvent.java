// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.networktables;

/**
 * NetworkTables event.
 *
 * <p>Events have flags. The flags are a bitmask and must be OR'ed together when listening to an
 * event to indicate the combination of events desired to be received.
 */
@SuppressWarnings("MemberName")
public final class NetworkTableEvent {
  /** No flags. */
  public static final int kNone = 0;

  /**
   * Initial listener addition. Set this flag to receive immediate notification of matches to the
   * flag criteria.
   */
  public static final int kImmediate = 0x01;

  /** Client connected (on server, any client connected). */
  public static final int kConnected = 0x02;

  /** Client disconnected (on server, any client disconnected). */
  public static final int kDisconnected = 0x04;

  /** Any connection event (connect or disconnect). */
  public static final int kConnection = kConnected | kDisconnected;

  /** New topic published. */
  public static final int kPublish = 0x08;

  /** Topic unpublished. */
  public static final int kUnpublish = 0x10;

  /** Topic properties changed. */
  public static final int kProperties = 0x20;

  /** Any topic event (publish, unpublish, or properties changed). */
  public static final int kTopic = kPublish | kUnpublish | kProperties;

  /** Topic value updated (via network). */
  public static final int kValueRemote = 0x40;

  /** Topic value updated (local). */
  public static final int kValueLocal = 0x80;

  /** Topic value updated (network or local). */
  public static final int kValueAll = kValueRemote | kValueLocal;

  /** Log message. */
  public static final int kLogMessage = 0x100;

  /**
   * Handle of listener that was triggered. The value returned when adding the listener can be used
   * to map this to a specific added listener.
   */
  public final int listener;

  /**
   * Event flags. For example, kPublish if the topic was not previously published. Also indicates
   * the data included with the event:
   *
   * <ul>
   *   <li>kConnected or kDisconnected: connInfo
   *   <li>kPublish, kUnpublish, or kProperties: topicInfo
   *   <li>kValueRemote, kValueLocal: valueData
   *   <li>kLogMessage: logMessage
   * </ul>
   */
  public final int flags;

  /** Connection information (for connection events). */
  public final ConnectionInfo connInfo;

  /** Topic information (for topic events). */
  public final TopicInfo topicInfo;

  /** Value data (for value events). */
  public final ValueEventData valueData;

  /** Log message (for log message events). */
  public final LogMessage logMessage;

  /**
   * Constructor. This should generally only be used internally to NetworkTables.
   *
   * @param inst Instance
   * @param listener Listener that was triggered
   * @param flags Event flags
   * @param connInfo Connection information
   * @param topicInfo Topic information
   * @param valueData Value data
   * @param logMessage Log message
   */
  public NetworkTableEvent(
      NetworkTableInstance inst,
      int listener,
      int flags,
      ConnectionInfo connInfo,
      TopicInfo topicInfo,
      ValueEventData valueData,
      LogMessage logMessage) {
    this.m_inst = inst;
    this.listener = listener;
    this.flags = flags;
    this.connInfo = connInfo;
    this.topicInfo = topicInfo;
    this.valueData = valueData;
    this.logMessage = logMessage;
  }

  /* Network table instance. */
  private final NetworkTableInstance m_inst;

  /**
   * Gets the instance associated with this event.
   *
   * @return Instance
   */
  public NetworkTableInstance getInstance() {
    return m_inst;
  }
}
