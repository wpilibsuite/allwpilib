// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.networktables;

/** NetworkTables Topic. */
public class Topic {
  /**
   * Constructor; use NetworkTableInstance.getTopic() instead.
   *
   * @param inst Instance
   * @param handle Native handle
   */
  Topic(NetworkTableInstance inst, int handle) {
    m_inst = inst;
    m_handle = handle;
  }

  /**
   * Determines if the native handle is valid.
   *
   * @return True if the native handle is valid, false otherwise.
   */
  public boolean isValid() {
    return m_handle != 0;
  }

  /**
   * Gets the native handle for the topic.
   *
   * @return Native handle
   */
  public int getHandle() {
    return m_handle;
  }

  /**
   * Gets the instance for the topic.
   *
   * @return Instance
   */
  public NetworkTableInstance getInstance() {
    return m_inst;
  }

  /**
   * Gets the name of the topic.
   *
   * @return the topic's name
   */
  public String getName() {
    return NetworkTablesJNI.getTopicName(m_handle);
  }

  /**
   * Gets the type of the topic.
   *
   * @return the topic's type
   */
  public NetworkTableType getType() {
    return NetworkTableType.getFromInt(NetworkTablesJNI.getType(m_handle));
  }

  /**
   * Gets the type string of the topic. This may have more information than the numeric type
   * (especially for raw values).
   *
   * @return the topic's type
   */
  public String getTypeString() {
    return NetworkTablesJNI.getTopicTypeString(m_handle);
  }

  /**
   * Gets combined information about the topic.
   *
   * @return Topic information
   */
  public TopicInfo getInfo() {
    return NetworkTablesJNI.getTopicInfo(m_inst, m_handle);
  }

  /**
   * Make value persistent through server restarts.
   *
   * @param persistent True for persistent, false for not persistent.
   */
  public void setPersistent(boolean persistent) {
    NetworkTablesJNI.setTopicPersistent(m_handle, persistent);
  }

  /**
   * Returns whether the value is persistent through server restarts.
   *
   * @return True if the value is persistent.
   */
  public boolean isPersistent() {
    return NetworkTablesJNI.getTopicPersistent(m_handle);
  }

  /**
   * Make the server retain the topic even when there are no publishers.
   *
   * @param retained True for retained, false for not retained.
   */
  public void setRetained(boolean retained) {
    NetworkTablesJNI.setTopicRetained(m_handle, retained);
  }

  /**
   * Returns whether the topic is retained by server when there are no publishers.
   *
   * @return True if the topic is retained.
   */
  public boolean isRetained() {
    return NetworkTablesJNI.getTopicRetained(m_handle);
  }

  /**
   * Allow storage of the topic's last value, allowing the value to be read (and not just accessed
   * through event queues and listeners).
   *
   * @param cached True for cached, false for not cached.
   */
  public void setCached(boolean cached) {
    NetworkTablesJNI.setTopicCached(m_handle, cached);
  }

  /**
   * Returns whether the topic's last value is stored.
   *
   * @return True if the topic is cached.
   */
  public boolean isCached() {
    return NetworkTablesJNI.getTopicCached(m_handle);
  }

  /**
   * Determines if the topic is currently being published.
   *
   * @return True if the topic exists, false otherwise.
   */
  public boolean exists() {
    return NetworkTablesJNI.getTopicExists(m_handle);
  }

  /**
   * Gets the current value of a property (as a JSON string).
   *
   * @param name property name
   * @return JSON string; "null" if the property does not exist.
   */
  public String getProperty(String name) {
    return NetworkTablesJNI.getTopicProperty(m_handle, name);
  }

  /**
   * Sets a property value.
   *
   * @param name property name
   * @param value property value (JSON string)
   * @throws IllegalArgumentException if properties is not parseable as JSON
   */
  public void setProperty(String name, String value) {
    NetworkTablesJNI.setTopicProperty(m_handle, name, value);
  }

  /**
   * Deletes a property. Has no effect if the property does not exist.
   *
   * @param name property name
   */
  public void deleteProperty(String name) {
    NetworkTablesJNI.deleteTopicProperty(m_handle, name);
  }

  /**
   * Gets all topic properties as a JSON object string. Each key in the object is the property name,
   * and the corresponding value is the property value.
   *
   * @return JSON string
   */
  public String getProperties() {
    return NetworkTablesJNI.getTopicProperties(m_handle);
  }

  /**
   * Updates multiple topic properties. Each key in the passed-in object is the name of the property
   * to add/update, and the corresponding value is the property value to set for that property. Null
   * values result in deletion of the corresponding property.
   *
   * @param properties JSON object string with keys to add/update/delete
   * @throws IllegalArgumentException if properties is not a JSON object
   */
  public void setProperties(String properties) {
    NetworkTablesJNI.setTopicProperties(m_handle, properties);
  }

  /**
   * Create a new subscriber to the topic.
   *
   * <p>The subscriber is only active as long as the returned object is not closed.
   *
   * @param options subscribe options
   * @return subscriber
   */
  public GenericSubscriber genericSubscribe(PubSubOption... options) {
    return genericSubscribe("", options);
  }

  /**
   * Create a new subscriber to the topic.
   *
   * <p>The subscriber is only active as long as the returned object is not closed.
   *
   * <p>Subscribers that do not match the published data type do not return any values. To determine
   * if the data type matches, use the appropriate Topic functions.
   *
   * @param typeString type string
   * @param options subscribe options
   * @return subscriber
   */
  public GenericSubscriber genericSubscribe(String typeString, PubSubOption... options) {
    return new GenericEntryImpl(
        this,
        NetworkTablesJNI.subscribe(
            m_handle, NetworkTableType.getFromString(typeString).getValue(), typeString, options));
  }

  /**
   * Create a new publisher to the topic.
   *
   * <p>The publisher is only active as long as the returned object is not closed.
   *
   * <p>It is not possible to publish two different data types to the same topic. Conflicts between
   * publishers are typically resolved by the server on a first-come, first-served basis. Any
   * published values that do not match the topic's data type are dropped (ignored). To determine if
   * the data type matches, use the appropriate Topic functions.
   *
   * @param typeString type string
   * @param options publish options
   * @return publisher
   */
  public GenericPublisher genericPublish(String typeString, PubSubOption... options) {
    return new GenericEntryImpl(
        this,
        NetworkTablesJNI.publish(
            m_handle, NetworkTableType.getFromString(typeString).getValue(), typeString, options));
  }

  /**
   * Create a new publisher to the topic, with type string and initial properties.
   *
   * <p>The publisher is only active as long as the returned object is not closed.
   *
   * <p>It is not possible to publish two different data types to the same topic. Conflicts between
   * publishers are typically resolved by the server on a first-come, first-served basis. Any
   * published values that do not match the topic's data type are dropped (ignored). To determine if
   * the data type matches, use the appropriate Topic functions.
   *
   * @param typeString type string
   * @param properties JSON properties
   * @param options publish options
   * @return publisher
   * @throws IllegalArgumentException if properties is not a JSON object
   */
  public GenericPublisher genericPublishEx(
      String typeString, String properties, PubSubOption... options) {
    return new GenericEntryImpl(
        this,
        NetworkTablesJNI.publishEx(
            m_handle,
            NetworkTableType.getFromString(typeString).getValue(),
            typeString,
            properties,
            options));
  }

  /**
   * Create a new generic entry for the topic.
   *
   * <p>Entries act as a combination of a subscriber and a weak publisher. The subscriber is active
   * as long as the entry is not closed. The publisher is created when the entry is first written
   * to, and remains active until either unpublish() is called or the entry is closed.
   *
   * <p>It is not possible to publish two different data types to the same topic. Conflicts between
   * publishers are typically resolved by the server on a first-come, first-served basis. Any
   * published values that do not match the topic's data type are dropped (ignored). To determine if
   * the data type matches, use the appropriate Topic functions.
   *
   * @param options publish and/or subscribe options
   * @return entry
   */
  public GenericEntry getGenericEntry(PubSubOption... options) {
    return getGenericEntry("", options);
  }

  /**
   * Create a new generic entry for the topic.
   *
   * <p>Entries act as a combination of a subscriber and a weak publisher. The subscriber is active
   * as long as the entry is not closed. The publisher is created when the entry is first written
   * to, and remains active until either unpublish() is called or the entry is closed.
   *
   * <p>It is not possible to publish two different data types to the same topic. Conflicts between
   * publishers are typically resolved by the server on a first-come, first-served basis. Any
   * published values that do not match the topic's data type are dropped (ignored). To determine if
   * the data type matches, use the appropriate Topic functions.
   *
   * @param typeString type string
   * @param options publish and/or subscribe options
   * @return entry
   */
  public GenericEntry getGenericEntry(String typeString, PubSubOption... options) {
    return new GenericEntryImpl(
        this,
        NetworkTablesJNI.getEntry(
            m_handle, NetworkTableType.getFromString(typeString).getValue(), typeString, options));
  }

  @Override
  public boolean equals(Object other) {
    return other instanceof Topic topic && m_handle == topic.m_handle;
  }

  @Override
  public int hashCode() {
    return m_handle;
  }

  /** NetworkTables instance. */
  protected NetworkTableInstance m_inst;

  /** NetworkTables handle. */
  protected int m_handle;
}
