/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2017-2018. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.networktables;

import java.util.HashMap;
import java.util.Map;
import java.util.concurrent.ConcurrentHashMap;
import java.util.concurrent.ConcurrentMap;
import java.util.concurrent.TimeUnit;
import java.util.concurrent.locks.Condition;
import java.util.concurrent.locks.ReentrantLock;
import java.util.function.Consumer;

/**
 * NetworkTables Instance.
 *
 * Instances are completely independent from each other.  Table operations on
 * one instance will not be visible to other instances unless the instances are
 * connected via the network.  The main limitation on instances is that you
 * cannot have two servers on the same network port.  The main utility of
 * instances is for unit testing, but they can also enable one program to
 * connect to two different NetworkTables networks.
 *
 * The global "default" instance (as returned by {@link #getDefault()}) is
 * always available, and is intended for the common case when there is only
 * a single NetworkTables instance being used in the program.
 *
 * Additional instances can be created with the {@link #create()} function.
 * A reference must be kept to the NetworkTableInstance returned by this
 * function to keep it from being garbage collected.
 */
public final class NetworkTableInstance {
  /**
   * Client/server mode flag values (as returned by {@link #getNetworkMode()}).
   * This is a bitmask.
   */
  public static final int kNetModeNone = 0x00;
  public static final int kNetModeServer = 0x01;
  public static final int kNetModeClient = 0x02;
  public static final int kNetModeStarting = 0x04;
  public static final int kNetModeFailure = 0x08;

  /**
   * The default port that network tables operates on.
   */
  public static final int kDefaultPort = 1735;

  /**
   * Construct from native handle.
   * @param handle Native handle
   */
  private NetworkTableInstance(int handle) {
    m_owned = false;
    m_handle = handle;
  }

  /**
   * Destroys the instance (if created by {@link #create()}).
   */
  public synchronized void free() {
    if (m_owned && m_handle != 0) {
      NetworkTablesJNI.destroyInstance(m_handle);
    }
  }

  /**
   * Determines if the native handle is valid.
   * @return True if the native handle is valid, false otherwise.
   */
  public boolean isValid() {
    return m_handle != 0;
  }

  /* The default instance. */
  private static NetworkTableInstance s_defaultInstance;

  /**
   * Get global default instance.
   * @return Global default instance
   */
  public synchronized static NetworkTableInstance getDefault() {
    if (s_defaultInstance == null) {
      s_defaultInstance = new NetworkTableInstance(NetworkTablesJNI.getDefaultInstance());
    }
    return s_defaultInstance;
  }

  /**
   * Create an instance.
   * Note: A reference to the returned instance must be retained to ensure the
   * instance is not garbage collected.
   * @return Newly created instance
   */
  public static NetworkTableInstance create() {
    NetworkTableInstance inst = new NetworkTableInstance(NetworkTablesJNI.createInstance());
    inst.m_owned = true;
    return inst;
  }

  /**
   * Gets the native handle for the entry.
   * @return Native handle
   */
  public int getHandle() {
    return m_handle;
  }

  /**
   * Gets the entry for a key.
   * @param name Key
   * @return Network table entry.
   */
  public NetworkTableEntry getEntry(String name) {
    return new NetworkTableEntry(this, NetworkTablesJNI.getEntry(m_handle, name));
  }

  /**
   * Get entries starting with the given prefix.
   * The results are optionally filtered by string prefix and entry type to
   * only return a subset of all entries.
   *
   * @param prefix entry name required prefix; only entries whose name
   * starts with this string are returned
   * @param types bitmask of types; 0 is treated as a "don't care"
   * @return Array of entries.
   */
  public NetworkTableEntry[] getEntries(String prefix, int types) {
    int[] handles = NetworkTablesJNI.getEntries(m_handle, prefix, types);
    NetworkTableEntry[] entries = new NetworkTableEntry[handles.length];
    for (int i = 0; i < handles.length; i++) {
      entries[i] = new NetworkTableEntry(this, handles[i]);
    }
    return entries;
  }

  /**
   * Get information about entries starting with the given prefix.
   * The results are optionally filtered by string prefix and entry type to
   * only return a subset of all entries.
   *
   * @param prefix entry name required prefix; only entries whose name
   * starts with this string are returned
   * @param types bitmask of types; 0 is treated as a "don't care"
   * @return Array of entry information.
   */
  public EntryInfo[] getEntryInfo(String prefix, int types) {
    return NetworkTablesJNI.getEntryInfo(this, m_handle, prefix, types);
  }

  /* Cache of created tables. */
  private final ConcurrentMap<String, NetworkTable> m_tables = new ConcurrentHashMap<>();

  /**
   * Gets the table with the specified key.
   *
   * @param key the key name
   * @return The network table
   */
  public NetworkTable getTable(String key) {
    // prepend leading / if not present
    String theKey;
    if (key.isEmpty() || key.equals("/")) {
      theKey = "";
    } else if (key.charAt(0) == NetworkTable.PATH_SEPARATOR) {
      theKey = key;
    } else {
      theKey = NetworkTable.PATH_SEPARATOR + key;
    }

    // cache created tables
    NetworkTable table = m_tables.get(theKey);
    if (table == null) {
      table = new NetworkTable(this, theKey);
      NetworkTable oldTable = m_tables.putIfAbsent(theKey, table);
      if (oldTable != null) {
        table = oldTable;
      }
    }
    return table;
  }

  /**
   * Deletes ALL keys in ALL subtables (except persistent values).
   * Use with caution!
   */
  public void deleteAllEntries() {
    NetworkTablesJNI.deleteAllEntries(m_handle);
  }

  /*
   * Callback Creation Functions
   */

  private static class EntryConsumer<T> {
    final NetworkTableEntry entry;
    final Consumer<T> consumer;

    EntryConsumer(NetworkTableEntry entry, Consumer<T> consumer) {
      this.entry = entry;
      this.consumer = consumer;
    }
  }

  private final ReentrantLock m_entryListenerLock = new ReentrantLock();
  private final Map<Integer, EntryConsumer<EntryNotification>> m_entryListeners = new HashMap<>();
  private Thread m_entryListenerThread;
  private int m_entryListenerPoller;
  private boolean m_entryListenerWaitQueue;
  private final Condition m_entryListenerWaitQueueCond = m_entryListenerLock.newCondition();

  private void startEntryListenerThread() {
    m_entryListenerThread = new Thread(() -> {
      boolean wasInterrupted = false;
      while (!Thread.interrupted()) {
        EntryNotification[] events;
        try {
          events = NetworkTablesJNI.pollEntryListener(this, m_entryListenerPoller);
        } catch (InterruptedException ex) {
          m_entryListenerLock.lock();
          try {
            if (m_entryListenerWaitQueue) {
              m_entryListenerWaitQueue = false;
              m_entryListenerWaitQueueCond.signalAll();
              continue;
            }
          } finally {
            m_entryListenerLock.unlock();
          }
          Thread.currentThread().interrupt();
          wasInterrupted = true; // don't try to destroy poller, as its handle is likely no longer valid
          break;
        }
        for (EntryNotification event : events) {
          EntryConsumer<EntryNotification> listener;
          m_entryListenerLock.lock();
          try {
            listener = m_entryListeners.get(event.listener);
          } finally {
            m_entryListenerLock.unlock();
          }
          if (listener != null) {
            event.entryObject = listener.entry;
            try {
              listener.consumer.accept(event);
            } catch (Throwable throwable) {
              System.err.println("Unhandled exception during entry listener callback: " + throwable.toString());
              throwable.printStackTrace();
            }
          }
        }
      }
      m_entryListenerLock.lock();
      try {
        if (!wasInterrupted) {
          NetworkTablesJNI.destroyEntryListenerPoller(m_entryListenerPoller);
        }
        m_entryListenerPoller = 0;
      } finally {
        m_entryListenerLock.unlock();
      }
    }, "NTEntryListener");
    m_entryListenerThread.setDaemon(true);
    m_entryListenerThread.start();
  }

  /**
   * Add a listener for all entries starting with a certain prefix.
   *
   * @param prefix            UTF-8 string prefix
   * @param listener          listener to add
   * @param flags             {@link EntryListenerFlags} bitmask
   * @return Listener handle
   */
  public int addEntryListener(String prefix, Consumer<EntryNotification> listener, int flags) {
    m_entryListenerLock.lock();
    try {
      if (m_entryListenerPoller == 0) {
        m_entryListenerPoller = NetworkTablesJNI.createEntryListenerPoller(m_handle);
        startEntryListenerThread();
      }
      int handle = NetworkTablesJNI.addPolledEntryListener(m_entryListenerPoller, prefix, flags);
      m_entryListeners.put(handle, new EntryConsumer<>(null, listener));
      return handle;
    } finally {
      m_entryListenerLock.unlock();
    }
  }

  /**
   * Add a listener for a particular entry.
   *
   * @param entry             the entry
   * @param listener          listener to add
   * @param flags             {@link EntryListenerFlags} bitmask
   * @return Listener handle
   */
  public int addEntryListener(NetworkTableEntry entry, Consumer<EntryNotification> listener, int flags) {
    if (!equals(entry.getInstance())) {
      throw new IllegalArgumentException("entry does not belong to this instance");
    }
    m_entryListenerLock.lock();
    try {
      if (m_entryListenerPoller == 0) {
        m_entryListenerPoller = NetworkTablesJNI.createEntryListenerPoller(m_handle);
        startEntryListenerThread();
      }
      int handle = NetworkTablesJNI.addPolledEntryListener(m_entryListenerPoller, entry.getHandle(), flags);
      m_entryListeners.put(handle, new EntryConsumer<>(entry, listener));
      return handle;
    } finally {
      m_entryListenerLock.unlock();
    }
  }

  /**
   * Remove an entry listener.
   * @param listener Listener handle to remove
   */
  public void removeEntryListener(int listener) {
    NetworkTablesJNI.removeEntryListener(listener);
  }

  /**
   * Wait for the entry listener queue to be empty.  This is primarily useful
   * for deterministic testing.  This blocks until either the entry listener
   * queue is empty (e.g. there are no more events that need to be passed along
   * to callbacks or poll queues) or the timeout expires.
   * @param timeout   timeout, in seconds.  Set to 0 for non-blocking behavior,
   *                  or a negative value to block indefinitely
   * @return False if timed out, otherwise true.
  */
  public boolean waitForEntryListenerQueue(double timeout) {
    if (!NetworkTablesJNI.waitForEntryListenerQueue(m_handle, timeout)) {
      return false;
    }
    m_entryListenerLock.lock();
    try {
      if (m_entryListenerPoller != 0) {
        m_entryListenerWaitQueue = true;
        NetworkTablesJNI.cancelPollEntryListener(m_entryListenerPoller);
        while (m_entryListenerWaitQueue) {
          try {
            if (timeout < 0) {
              m_entryListenerWaitQueueCond.await();
            } else {
              return m_entryListenerWaitQueueCond.await((long) (timeout * 1e9), TimeUnit.NANOSECONDS);
            }
          } catch (InterruptedException ex) {
            Thread.currentThread().interrupt();
            return true;
          }
        }
      }
    } finally {
      m_entryListenerLock.unlock();
    }
    return true;
  }

  private final ReentrantLock m_connectionListenerLock = new ReentrantLock();
  private final Map<Integer, Consumer<ConnectionNotification>> m_connectionListeners = new HashMap<>();
  private Thread m_connectionListenerThread;
  private int m_connectionListenerPoller;
  private boolean m_connectionListenerWaitQueue;
  private final Condition m_connectionListenerWaitQueueCond = m_connectionListenerLock.newCondition();

  private void startConnectionListenerThread() {
    m_connectionListenerThread = new Thread(() -> {
      boolean wasInterrupted = false;
      while (!Thread.interrupted()) {
        ConnectionNotification[] events;
        try {
          events = NetworkTablesJNI.pollConnectionListener(this, m_connectionListenerPoller);
        } catch (InterruptedException ex) {
          m_connectionListenerLock.lock();
          try {
            if (m_connectionListenerWaitQueue) {
              m_connectionListenerWaitQueue = false;
              m_connectionListenerWaitQueueCond.signalAll();
              continue;
            }
          } finally {
            m_connectionListenerLock.unlock();
          }
          Thread.currentThread().interrupt();
          wasInterrupted = true; // don't try to destroy poller, as its handle is likely no longer valid
          break;
        }
        for (ConnectionNotification event : events) {
          Consumer<ConnectionNotification> listener;
          m_connectionListenerLock.lock();
          try {
            listener = m_connectionListeners.get(event.listener);
          } finally {
            m_connectionListenerLock.unlock();
          }
          if (listener != null) {
            try {
              listener.accept(event);
            } catch (Throwable throwable) {
              System.err.println("Unhandled exception during connection listener callback: " + throwable.toString());
              throwable.printStackTrace();
            }
          }
        }
      }
      m_connectionListenerLock.lock();
      try {
        if (!wasInterrupted) {
          NetworkTablesJNI.destroyConnectionListenerPoller(m_connectionListenerPoller);
        }
        m_connectionListenerPoller = 0;
      } finally {
        m_connectionListenerLock.unlock();
      }
    }, "NTConnectionListener");
    m_connectionListenerThread.setDaemon(true);
    m_connectionListenerThread.start();
  }

  /**
   * Add a connection listener.
   *
   * @param listener Listener to add
   * @param immediateNotify Notify listener of all existing connections
   * @return Listener handle
   */
  public int addConnectionListener(Consumer<ConnectionNotification> listener, boolean immediateNotify) {
    m_connectionListenerLock.lock();
    try {
      if (m_connectionListenerPoller == 0) {
        m_connectionListenerPoller = NetworkTablesJNI.createConnectionListenerPoller(m_handle);
        startConnectionListenerThread();
      }
      int handle = NetworkTablesJNI.addPolledConnectionListener(m_connectionListenerPoller, immediateNotify);
      m_connectionListeners.put(handle, listener);
      return handle;
    } finally {
      m_connectionListenerLock.unlock();
    }
  }

  /**
   * Remove a connection listener.
   * @param listener Listener handle to remove
   */
  public void removeConnectionListener(int listener) {
    m_connectionListenerLock.lock();
    try {
      m_connectionListeners.remove(listener);
    } finally {
      m_connectionListenerLock.unlock();
    }
    NetworkTablesJNI.removeConnectionListener(listener);
  }

  /**
   * Wait for the connection listener queue to be empty.  This is primarily useful
   * for deterministic testing.  This blocks until either the connection listener
   * queue is empty (e.g. there are no more events that need to be passed along
   * to callbacks or poll queues) or the timeout expires.
   * @param timeout   timeout, in seconds.  Set to 0 for non-blocking behavior,
   *                  or a negative value to block indefinitely
   * @return False if timed out, otherwise true.
   */
  public boolean waitForConnectionListenerQueue(double timeout) {
    if (!NetworkTablesJNI.waitForConnectionListenerQueue(m_handle, timeout)) {
      return false;
    }
    m_connectionListenerLock.lock();
    try {
      if (m_connectionListenerPoller != 0) {
        m_connectionListenerWaitQueue = true;
        NetworkTablesJNI.cancelPollConnectionListener(m_connectionListenerPoller);
        while (m_connectionListenerWaitQueue) {
          try {
            if (timeout < 0) {
              m_connectionListenerWaitQueueCond.await();
            } else {
              return m_connectionListenerWaitQueueCond.await((long) (timeout * 1e9), TimeUnit.NANOSECONDS);
            }
          } catch (InterruptedException ex) {
            Thread.currentThread().interrupt();
            return true;
          }
        }
      }
    } finally {
      m_connectionListenerLock.unlock();
    }
    return true;
  }

  /*
   * Remote Procedure Call Functions
   */

  private final ReentrantLock m_rpcCallLock = new ReentrantLock();
  private final Map<Integer, EntryConsumer<RpcAnswer>> m_rpcCalls = new HashMap<>();
  private Thread m_rpcCallThread;
  private int m_rpcCallPoller;
  private boolean m_rpcCallWaitQueue;
  private final Condition m_rpcCallWaitQueueCond = m_rpcCallLock.newCondition();

  private void startRpcCallThread() {
    m_rpcCallThread = new Thread(() -> {
      boolean wasInterrupted = false;
      while (!Thread.interrupted()) {
        RpcAnswer[] events;
        try {
          events = NetworkTablesJNI.pollRpc(this, m_rpcCallPoller);
        } catch (InterruptedException ex) {
          m_rpcCallLock.lock();
          try {
            if (m_rpcCallWaitQueue) {
              m_rpcCallWaitQueue = false;
              m_rpcCallWaitQueueCond.signalAll();
              continue;
            }
          } finally {
            m_rpcCallLock.unlock();
          }
          Thread.currentThread().interrupt();
          wasInterrupted = true; // don't try to destroy poller, as its handle is likely no longer valid
          break;
        }
        for (RpcAnswer event : events) {
          EntryConsumer<RpcAnswer> listener;
          m_rpcCallLock.lock();
          try {
            listener = m_rpcCalls.get(event.entry);
          } finally {
            m_rpcCallLock.unlock();
          }
          if (listener != null) {
            event.entryObject = listener.entry;
            try {
              listener.consumer.accept(event);
            } catch (Throwable throwable) {
              System.err.println("Unhandled exception during RPC callback: " + throwable.toString());
              throwable.printStackTrace();
            }
          }
        }
      }
      m_rpcCallLock.lock();
      try {
        if (!wasInterrupted) {
          NetworkTablesJNI.destroyRpcCallPoller(m_rpcCallPoller);
        }
        m_rpcCallPoller = 0;
      } finally {
        m_rpcCallLock.unlock();
      }
    }, "NTRpcCall");
    m_rpcCallThread.setDaemon(true);
    m_rpcCallThread.start();
  }

  private static final byte[] rev0def = new byte[] {0};

  /**
   * Create a callback-based RPC entry point.  Only valid to use on the server.
   * The callback function will be called when the RPC is called.
   * This function creates RPC version 0 definitions (raw data in and out).
   * @param entry     the entry
   * @param callback  callback function
   */
  public void createRpc(NetworkTableEntry entry, Consumer<RpcAnswer> callback) {
    m_rpcCallLock.lock();
    try {
      if (m_rpcCallPoller == 0) {
        m_rpcCallPoller = NetworkTablesJNI.createRpcCallPoller(m_handle);
        startRpcCallThread();
      }
      NetworkTablesJNI.createPolledRpc(entry.getHandle(), rev0def, m_rpcCallPoller);
      m_rpcCalls.put(entry.getHandle(), new EntryConsumer<>(entry, callback));
    } finally {
      m_rpcCallLock.unlock();
    }
  }

  /**
   * Wait for the incoming RPC call queue to be empty.  This is primarily useful
   * for deterministic testing.  This blocks until either the RPC call
   * queue is empty (e.g. there are no more events that need to be passed along
   * to callbacks or poll queues) or the timeout expires.
   * @param timeout   timeout, in seconds.  Set to 0 for non-blocking behavior,
   *                  or a negative value to block indefinitely
   * @return False if timed out, otherwise true.
   */
  public boolean waitForRpcCallQueue(double timeout) {
    if (!NetworkTablesJNI.waitForRpcCallQueue(m_handle, timeout)) {
      return false;
    }
    m_rpcCallLock.lock();
    try {
      if (m_rpcCallPoller != 0) {
        m_rpcCallWaitQueue = true;
        NetworkTablesJNI.cancelPollRpc(m_rpcCallPoller);
        while (m_rpcCallWaitQueue) {
          try {
            if (timeout < 0) {
              m_rpcCallWaitQueueCond.await();
            } else {
              return m_rpcCallWaitQueueCond.await((long) (timeout * 1e9), TimeUnit.NANOSECONDS);
            }
          } catch (InterruptedException ex) {
            Thread.currentThread().interrupt();
            return true;
          }
        }
      }
    } finally {
      m_rpcCallLock.unlock();
    }
    return true;
  }

  /*
   * Client/Server Functions
   */

  /**
   * Set the network identity of this node.
   * This is the name used during the initial connection handshake, and is
   * visible through ConnectionInfo on the remote node.
   * @param name      identity to advertise
   */
  public void setNetworkIdentity(String name) {
    NetworkTablesJNI.setNetworkIdentity(m_handle, name);
  }

  /**
   * Get the current network mode.
   * @return Bitmask of NetworkMode.
   */
  public int getNetworkMode() {
    return NetworkTablesJNI.getNetworkMode(m_handle);
  }

  /**
   * Starts a server using the networktables.ini as the persistent file,
   * using the default listening address and port.
   */
  public void startServer() {
    startServer("networktables.ini");
  }

  /**
   * Starts a server using the specified persistent filename, using the default
   * listening address and port.
   *
   * @param persistFilename  the name of the persist file to use
   */
  public void startServer(String persistFilename) {
    startServer(persistFilename, "");
  }

  /**
   * Starts a server using the specified filename and listening address,
   * using the default port.
   *
   * @param persistFilename  the name of the persist file to use
   * @param listenAddress    the address to listen on, or empty to listen on any
   *                         address
   */
  public void startServer(String persistFilename, String listenAddress) {
    startServer(persistFilename, listenAddress, kDefaultPort);
  }

  /**
   * Starts a server using the specified filename, listening address, and port.
   *
   * @param persistFilename  the name of the persist file to use
   * @param listenAddress    the address to listen on, or empty to listen on any
   *                         address
   * @param port             port to communicate over
   */
  public void startServer(String persistFilename, String listenAddress, int port) {
    NetworkTablesJNI.startServer(m_handle, persistFilename, listenAddress, port);
  }

  /**
   * Stops the server if it is running.
   */
  public void stopServer() {
    NetworkTablesJNI.stopServer(m_handle);
  }

  /**
   * Starts a client.  Use SetServer to set the server name and port.
   */
  public void startClient() {
    NetworkTablesJNI.startClient(m_handle);
  }

  /**
   * Starts a client using the specified server and the default port
   *
   * @param serverName  server name
   */
  public void startClient(String serverName) {
    startClient(serverName, kDefaultPort);
  }

  /**
   * Starts a client using the specified server and port
   *
   * @param serverName  server name
   * @param port        port to communicate over
   */
  public void startClient(String serverName, int port) {
    NetworkTablesJNI.startClient(m_handle, serverName, port);
  }

  /**
   * Starts a client using the specified servers and default port.  The
   * client will attempt to connect to each server in round robin fashion.
   *
   * @param serverNames   array of server names
   */
  public void startClient(String[] serverNames) {
    startClient(serverNames, kDefaultPort);
  }

  /**
   * Starts a client using the specified servers and port number.  The
   * client will attempt to connect to each server in round robin fashion.
   *
   * @param serverNames   array of server names
   * @param port          port to communicate over
   */
  public void startClient(String[] serverNames, int port) {
    int[] ports = new int[serverNames.length];
    for (int i = 0; i < serverNames.length; i++) {
      ports[i] = port;
    }
    startClient(serverNames, ports);
  }

  /**
   * Starts a client using the specified (server, port) combinations.  The
   * client will attempt to connect to each server in round robin fashion.
   *
   * @param serverNames   array of server names
   * @param ports         array of port numbers
   */
  public void startClient(String[] serverNames, int[] ports) {
    NetworkTablesJNI.startClient(m_handle, serverNames, ports);
  }

  /**
   * Starts a client using commonly known robot addresses for the specified
   * team using the default port number.
   *
   * @param team        team number
   */
  public void startClientTeam(int team) {
    startClientTeam(team, kDefaultPort);
  }

  /**
   * Starts a client using commonly known robot addresses for the specified
   * team.
   *
   * @param team        team number
   * @param port        port to communicate over
   */
  public void startClientTeam(int team, int port) {
    NetworkTablesJNI.startClientTeam(m_handle, team, port);
  }

  /**
   * Stops the client if it is running.
   */
  public void stopClient() {
    NetworkTablesJNI.stopClient(m_handle);
  }

  /**
   * Sets server address and port for client (without restarting client).
   * Changes the port to the default port.
   *
   * @param serverName  server name
   */
  public void setServer(String serverName) {
    setServer(serverName, kDefaultPort);
  }

  /**
   * Sets server address and port for client (without restarting client).
   *
   * @param serverName  server name
   * @param port        port to communicate over
   */
  public void setServer(String serverName, int port) {
    NetworkTablesJNI.setServer(m_handle, serverName, port);
  }

  /**
   * Sets server addresses and port for client (without restarting client).
   * Changes the port to the default port.  The client will attempt to connect
   * to each server in round robin fashion.
   *
   * @param serverNames   array of server names
   */
  public void setServer(String[] serverNames) {
    setServer(serverNames, kDefaultPort);
  }

  /**
   * Sets server addresses and port for client (without restarting client).
   * The client will attempt to connect to each server in round robin fashion.
   *
   * @param serverNames   array of server names
   * @param port          port to communicate over
   */
  public void setServer(String[] serverNames, int port) {
    int[] ports = new int[serverNames.length];
    for (int i = 0; i < serverNames.length; i++) {
      ports[i] = port;
    }
    setServer(serverNames, ports);
  }

  /**
   * Sets server addresses and ports for client (without restarting client).
   * The client will attempt to connect to each server in round robin fashion.
   *
   * @param serverNames   array of server names
   * @param ports         array of port numbers
   */
  public void setServer(String[] serverNames, int[] ports) {
    NetworkTablesJNI.setServer(m_handle, serverNames, ports);
  }

  /**
   * Sets server addresses and port for client (without restarting client).
   * Changes the port to the default port.  The client will attempt to connect
   * to each server in round robin fashion.
   *
   * @param team        team number
   */
  public void setServerTeam(int team) {
    setServerTeam(team, kDefaultPort);
  }

  /**
   * Sets server addresses and port for client (without restarting client).
   * Connects using commonly known robot addresses for the specified team.
   *
   * @param team        team number
   * @param port        port to communicate over
   */
  public void setServerTeam(int team, int port) {
    NetworkTablesJNI.setServerTeam(m_handle, team, port);
  }

  /**
   * Starts requesting server address from Driver Station.
   * This connects to the Driver Station running on localhost to obtain the
   * server IP address, and connects with the default port.
   */
  public void startDSClient() {
    startDSClient(kDefaultPort);
  }

  /**
   * Starts requesting server address from Driver Station.
   * This connects to the Driver Station running on localhost to obtain the
   * server IP address.
   *
   * @param port server port to use in combination with IP from DS
   */
  public void startDSClient(int port) {
    NetworkTablesJNI.startDSClient(m_handle, port);
  }

  /**
   * Stops requesting server address from Driver Station.
   */
  public void stopDSClient() {
    NetworkTablesJNI.stopDSClient(m_handle);
  }

  /**
   * Set the periodic update rate.
   * Sets how frequently updates are sent to other nodes over the network.
   *
   * @param interval update interval in seconds (range 0.01 to 1.0)
   */
  public void setUpdateRate(double interval) {
    NetworkTablesJNI.setUpdateRate(m_handle, interval);
  }

  /**
   * Flushes all updated values immediately to the network.
   * Note: This is rate-limited to protect the network from flooding.
   * This is primarily useful for synchronizing network updates with
   * user code.
   */
  public void flush() {
    NetworkTablesJNI.flush(m_handle);
  }

  /**
   * Gets information on the currently established network connections.
   * If operating as a client, this will return either zero or one values.
   * @return array of connection information
   */
  public ConnectionInfo[] getConnections() {
    return NetworkTablesJNI.getConnections(m_handle);
  }

  /**
   * Return whether or not the instance is connected to another node.
   * @return True if connected.
   */
  public boolean isConnected() {
    return NetworkTablesJNI.isConnected(m_handle);
  }

  /**
   * Saves persistent keys to a file.  The server does this automatically.
   *
   * @param filename file name
   * @throws PersistentException if error saving file
   */
  public void savePersistent(String filename) throws PersistentException {
    NetworkTablesJNI.savePersistent(m_handle, filename);
  }

  /**
   * Loads persistent keys from a file.  The server does this automatically.
   *
   * @param filename file name
   * @return List of warnings (errors result in an exception instead)
   * @throws PersistentException if error reading file
   */
  public String[] loadPersistent(String filename) throws PersistentException {
    return NetworkTablesJNI.loadPersistent(m_handle, filename);
  }

  /**
   * Save table values to a file.  The file format used is identical to
   * that used for SavePersistent.
   * @param filename  filename
   * @param prefix    save only keys starting with this prefix
   * @throws PersistentException if error saving file
   */
  public void saveEntries(String filename, String prefix) throws PersistentException {
    NetworkTablesJNI.saveEntries(m_handle, filename, prefix);
  }

  /**
   * Load table values from a file.  The file format used is identical to
   * that used for SavePersistent / LoadPersistent.
   * @param filename  filename
   * @param prefix    load only keys starting with this prefix
   * @return List of warnings (errors result in an exception instead)
   * @throws PersistentException if error saving file
   */
  public String[] loadEntries(String filename, String prefix) throws PersistentException {
    return NetworkTablesJNI.loadEntries(m_handle, filename, prefix);
  }

  private final ReentrantLock m_loggerLock = new ReentrantLock();
  private final Map<Integer, Consumer<LogMessage>> m_loggers = new HashMap<>();
  private Thread m_loggerThread;
  private int m_loggerPoller;
  private boolean m_loggerWaitQueue;
  private final Condition m_loggerWaitQueueCond = m_loggerLock.newCondition();

  private void startLogThread() {
    m_loggerThread = new Thread(() -> {
      boolean wasInterrupted = false;
      while (!Thread.interrupted()) {
        LogMessage[] events;
        try {
          events = NetworkTablesJNI.pollLogger(this, m_loggerPoller);
        } catch (InterruptedException ex) {
          Thread.currentThread().interrupt();
          wasInterrupted = true; // don't try to destroy poller, as its handle is likely no longer valid
          break;
        }
        for (LogMessage event : events) {
          Consumer<LogMessage> logger;
          m_loggerLock.lock();
          try {
            logger = m_loggers.get(event.logger);
          } finally {
            m_loggerLock.unlock();
          }
          if (logger != null) {
            try {
              logger.accept(event);
            } catch (Throwable throwable) {
              System.err.println("Unhandled exception during logger callback: " + throwable.toString());
              throwable.printStackTrace();
            }
          }
        }
      }
      m_loggerLock.lock();
      try {
        if (!wasInterrupted) {
          NetworkTablesJNI.destroyLoggerPoller(m_loggerPoller);
        }
        m_rpcCallPoller = 0;
      } finally {
        m_loggerLock.unlock();
      }
    }, "NTLogger");
    m_loggerThread.setDaemon(true);
    m_loggerThread.start();
  }

  /**
   * Add logger callback function.  By default, log messages are sent to stderr;
   * this function sends log messages with the specified levels to the provided
   * callback function instead.  The callback function will only be called for
   * log messages with level greater than or equal to minLevel and less than or
   * equal to maxLevel; messages outside this range will be silently ignored.
   *
   * @param func        log callback function
   * @param minLevel    minimum log level
   * @param maxLevel    maximum log level
   * @return Logger handle
   */
  public int addLogger(Consumer<LogMessage> func, int minLevel, int maxLevel) {
    m_loggerLock.lock();
    try {
      if (m_loggerPoller == 0) {
        m_loggerPoller = NetworkTablesJNI.createLoggerPoller(m_handle);
        startLogThread();
      }
      int handle = NetworkTablesJNI.addPolledLogger(m_loggerPoller, minLevel, maxLevel);
      m_loggers.put(handle, func);
      return handle;
    } finally {
      m_loggerLock.unlock();
    }
  }

  /**
   * Remove a logger.
   * @param logger Logger handle to remove
   */
  public void removeLogger(int logger) {
    m_loggerLock.lock();
    try {
      m_loggers.remove(logger);
    } finally {
      m_loggerLock.unlock();
    }
    NetworkTablesJNI.removeLogger(logger);
  }

  /**
   * Wait for the incoming log event queue to be empty.  This is primarily useful
   * for deterministic testing.  This blocks until either the log event
   * queue is empty (e.g. there are no more events that need to be passed along
   * to callbacks or poll queues) or the timeout expires.
   * @param timeout   timeout, in seconds.  Set to 0 for non-blocking behavior,
   *                  or a negative value to block indefinitely
   * @return False if timed out, otherwise true.
   */
  public boolean waitForLoggerQueue(double timeout) {
    if (!NetworkTablesJNI.waitForLoggerQueue(m_handle, timeout)) {
      return false;
    }
    m_loggerLock.lock();
    try {
      if (m_loggerPoller != 0) {
        m_loggerWaitQueue = true;
        NetworkTablesJNI.cancelPollLogger(m_loggerPoller);
        while (m_loggerWaitQueue) {
          try {
            if (timeout < 0) {
              m_loggerWaitQueueCond.await();
            } else {
              return m_loggerWaitQueueCond.await((long) (timeout * 1e9), TimeUnit.NANOSECONDS);
            }
          } catch (InterruptedException ex) {
            Thread.currentThread().interrupt();
            return true;
          }
        }
      }
    } finally {
      m_loggerLock.unlock();
    }
    return true;
  }

  @Override
  public boolean equals(Object o) {
    if (o == this) {
      return true;
    }
    if (!(o instanceof NetworkTableInstance)) {
      return false;
    }
    NetworkTableInstance other = (NetworkTableInstance) o;
    return m_handle == other.m_handle;
  }

  @Override
  public int hashCode() {
    return m_handle;
  }

  private boolean m_owned;
  private int m_handle;
}
