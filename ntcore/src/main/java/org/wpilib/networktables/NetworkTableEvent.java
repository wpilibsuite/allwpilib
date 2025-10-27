// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.networktables;

/**
 * NetworkTables event.
 *
 * <p>There are different kinds of events. When creating a listener, a combination of event kinds
 * can be listened to by building an EnumSet of NetworkTableEvent.Kind.
 */
@SuppressWarnings("MemberName")
public final class NetworkTableEvent {
  /** NetworkTable event kind. */
  public enum Kind {
    /**
     * Initial listener addition. Set this to receive immediate notification of matches to other
     * criteria.
     */
    kImmediate(0x0001),

    /** Client connected (on server, any client connected). */
    kConnected(0x0002),

    /** Client disconnected (on server, any client disconnected). */
    kDisconnected(0x0004),

    /** Any connection event (connect or disconnect). */
    kConnection(0x0004 | 0x0002),

    /** New topic published. */
    kPublish(0x0008),

    /** Topic unpublished. */
    kUnpublish(0x0010),

    /** Topic properties changed. */
    kProperties(0x0020),

    /** Any topic event (publish, unpublish, or properties changed). */
    kTopic(0x0020 | 0x0010 | 0x0008),

    /** Topic value updated (via network). */
    kValueRemote(0x0040),

    /** Topic value updated (local). */
    kValueLocal(0x0080),

    /** Topic value updated (network or local). */
    kValueAll(0x0080 | 0x0040),

    /** Log message. */
    kLogMessage(0x0100),

    /** Time synchronized with server. */
    kTimeSync(0x0200);

    private final int value;

    Kind(int value) {
      this.value = value;
    }

    /**
     * Returns the NetworkTable event kind value.
     *
     * @return The NetworkTable event kind value.
     */
    public int getValue() {
      return value;
    }
  }

  /**
   * Handle of listener that was triggered. The value returned when adding the listener can be used
   * to map this to a specific added listener.
   */
  public final int listener;

  /**
   * Determine if event is of a particular kind. For example, kPublish if the topic was not
   * previously published. Also indicates the data included with the event:
   *
   * <ul>
   *   <li>kConnected or kDisconnected: connInfo
   *   <li>kPublish, kUnpublish, or kProperties: topicInfo
   *   <li>kValueRemote, kValueLocal: valueData
   *   <li>kLogMessage: logMessage
   * </ul>
   *
   * @param kind Kind
   * @return True if event matches kind
   */
  public boolean is(Kind kind) {
    return (m_flags & kind.getValue()) != 0;
  }

  private final int m_flags;

  /** Connection information (for connection events). */
  public final ConnectionInfo connInfo;

  /** Topic information (for topic events). */
  public final TopicInfo topicInfo;

  /** Value data (for value events). */
  public final ValueEventData valueData;

  /** Log message (for log message events). */
  public final LogMessage logMessage;

  /** Log message (for log message events). */
  public final TimeSyncEventData timeSyncData;

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
   * @param timeSyncData Time sync data
   */
  public NetworkTableEvent(
      NetworkTableInstance inst,
      int listener,
      int flags,
      ConnectionInfo connInfo,
      TopicInfo topicInfo,
      ValueEventData valueData,
      LogMessage logMessage,
      TimeSyncEventData timeSyncData) {
    this.m_inst = inst;
    this.listener = listener;
    this.m_flags = flags;
    this.connInfo = connInfo;
    this.topicInfo = topicInfo;
    this.valueData = valueData;
    this.logMessage = logMessage;
    this.timeSyncData = timeSyncData;
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
