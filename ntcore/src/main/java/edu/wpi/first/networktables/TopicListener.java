// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.networktables;

import edu.wpi.first.util.WPIUtilJNI;
import java.util.HashMap;
import java.util.Map;
import java.util.concurrent.locks.Condition;
import java.util.concurrent.locks.ReentrantLock;
import java.util.function.Consumer;

/**
 * Topic change listener. This calls back to a callback function when a topic change matching the
 * specified mask occurs.
 */
public final class TopicListener implements AutoCloseable {
  /**
   * Create a listener for changes on a particular topic.
   *
   * @param topic Topic
   * @param eventMask Bitmask of TopicListenerFlags values
   * @param listener Listener function
   */
  public TopicListener(Topic topic, int eventMask, Consumer<TopicNotification> listener) {
    s_lock.lock();
    try {
      if (s_poller == 0) {
        s_inst = topic.getInstance();
        s_poller = NetworkTablesJNI.createTopicListenerPoller(s_inst.getHandle());
        startThread();
      }
      m_handle = NetworkTablesJNI.addPolledTopicListener(s_poller, topic.getHandle(), eventMask);
      s_listeners.put(m_handle, listener);
    } finally {
      s_lock.unlock();
    }
  }

  /**
   * Create a listener for changes to topics with names that start with any of the given prefixes.
   *
   * @param inst Instance
   * @param prefixes Topic name string prefixes
   * @param eventMask Bitmask of TopicListenerFlags values
   * @param listener Listener function
   */
  public TopicListener(
      NetworkTableInstance inst,
      String[] prefixes,
      int eventMask,
      Consumer<TopicNotification> listener) {
    s_lock.lock();
    try {
      if (s_poller == 0) {
        s_inst = inst;
        s_poller = NetworkTablesJNI.createTopicListenerPoller(inst.getHandle());
        startThread();
      }
      m_handle = NetworkTablesJNI.addPolledTopicListener(s_poller, prefixes, eventMask);
      s_listeners.put(m_handle, listener);
    } finally {
      s_lock.unlock();
    }
  }

  @Override
  public synchronized void close() {
    if (m_handle != 0) {
      s_lock.lock();
      try {
        s_listeners.remove(m_handle);
      } finally {
        s_lock.unlock();
      }
      NetworkTablesJNI.removeTopicListener(m_handle);
      m_handle = 0;
    }
  }

  /**
   * Determines if the native handle is valid.
   *
   * @return True if the native handle is valid, false otherwise.
   */
  public boolean isValid() {
    return m_handle != 0;
  }

  private int m_handle;

  private static final ReentrantLock s_lock = new ReentrantLock();
  private static final Map<Integer, Consumer<TopicNotification>> s_listeners = new HashMap<>();
  private static Thread s_thread;
  private static NetworkTableInstance s_inst;
  private static int s_poller;
  private static boolean s_waitQueue;
  private static final Condition s_waitQueueCond = s_lock.newCondition();

  private static void startThread() {
    s_thread =
        new Thread(
            () -> {
              boolean wasInterrupted = false;
              while (!Thread.interrupted()) {
                try {
                  WPIUtilJNI.waitForObject(s_poller);
                } catch (InterruptedException ex) {
                  s_lock.lock();
                  try {
                    if (s_waitQueue) {
                      s_waitQueue = false;
                      s_waitQueueCond.signalAll();
                      continue;
                    }
                  } finally {
                    s_lock.unlock();
                  }
                  Thread.currentThread().interrupt();
                  // don't try to destroy poller, as its handle is likely no longer valid
                  wasInterrupted = true;
                  break;
                }
                for (TopicNotification event :
                    NetworkTablesJNI.readTopicListenerQueue(s_inst, s_poller)) {
                  Consumer<TopicNotification> listener;
                  s_lock.lock();
                  try {
                    listener = s_listeners.get(event.listener);
                  } finally {
                    s_lock.unlock();
                  }
                  if (listener != null) {
                    try {
                      listener.accept(event);
                    } catch (Throwable throwable) {
                      System.err.println(
                          "Unhandled exception during listener callback: " + throwable.toString());
                      throwable.printStackTrace();
                    }
                  }
                }
              }
              s_lock.lock();
              try {
                if (!wasInterrupted) {
                  NetworkTablesJNI.destroyTopicListenerPoller(s_poller);
                }
                s_poller = 0;
              } finally {
                s_lock.unlock();
              }
            },
            "TopicListener");
    s_thread.setDaemon(true);
    s_thread.start();
  }
}
