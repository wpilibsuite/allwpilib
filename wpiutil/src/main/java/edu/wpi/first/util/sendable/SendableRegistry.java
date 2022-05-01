// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.util.sendable;

import java.lang.ref.WeakReference;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.List;
import java.util.Map;
import java.util.WeakHashMap;
import java.util.function.Consumer;
import java.util.function.Supplier;

/**
 * The SendableRegistry class is the public interface for registering sensors and actuators for use
 * on dashboards and LiveWindow.
 */
public final class SendableRegistry {
  private static class Component {
    Component() {}

    Component(Sendable sendable) {
      m_sendable = new WeakReference<>(sendable);
    }

    WeakReference<Sendable> m_sendable;
    SendableBuilder m_builder;
    String m_name;
    String m_subsystem = "Ungrouped";
    WeakReference<Sendable> m_parent;
    boolean m_liveWindow;
    Object[] m_data;

    void setName(String moduleType, int channel) {
      m_name = moduleType + "[" + channel + "]";
    }

    void setName(String moduleType, int moduleNumber, int channel) {
      m_name = moduleType + "[" + moduleNumber + "," + channel + "]";
    }
  }

  private static Supplier<SendableBuilder> liveWindowFactory;
  private static final Map<Object, Component> components = new WeakHashMap<>();
  private static int nextDataHandle;

  private static Component getOrAdd(Sendable sendable) {
    Component comp = components.get(sendable);
    if (comp == null) {
      comp = new Component(sendable);
      components.put(sendable, comp);
    } else {
      if (comp.m_sendable == null) {
        comp.m_sendable = new WeakReference<>(sendable);
      }
    }
    return comp;
  }

  private SendableRegistry() {
    throw new UnsupportedOperationException("This is a utility class!");
  }

  /**
   * Sets the factory for LiveWindow builders.
   *
   * @param factory factory function
   */
  public static synchronized void setLiveWindowBuilderFactory(Supplier<SendableBuilder> factory) {
    liveWindowFactory = factory;
  }

  /**
   * Adds an object to the registry.
   *
   * @param sendable object to add
   * @param name component name
   */
  public static synchronized void add(Sendable sendable, String name) {
    Component comp = getOrAdd(sendable);
    comp.m_name = name;
  }

  /**
   * Adds an object to the registry.
   *
   * @param sendable object to add
   * @param moduleType A string that defines the module name in the label for the value
   * @param channel The channel number the device is plugged into
   */
  public static synchronized void add(Sendable sendable, String moduleType, int channel) {
    Component comp = getOrAdd(sendable);
    comp.setName(moduleType, channel);
  }

  /**
   * Adds an object to the registry.
   *
   * @param sendable object to add
   * @param moduleType A string that defines the module name in the label for the value
   * @param moduleNumber The number of the particular module type
   * @param channel The channel number the device is plugged into
   */
  public static synchronized void add(
      Sendable sendable, String moduleType, int moduleNumber, int channel) {
    Component comp = getOrAdd(sendable);
    comp.setName(moduleType, moduleNumber, channel);
  }

  /**
   * Adds an object to the registry.
   *
   * @param sendable object to add
   * @param subsystem subsystem name
   * @param name component name
   */
  public static synchronized void add(Sendable sendable, String subsystem, String name) {
    Component comp = getOrAdd(sendable);
    comp.m_name = name;
    comp.m_subsystem = subsystem;
  }

  /**
   * Adds an object to the registry and LiveWindow.
   *
   * @param sendable object to add
   * @param name component name
   */
  public static synchronized void addLW(Sendable sendable, String name) {
    Component comp = getOrAdd(sendable);
    if (liveWindowFactory != null) {
      comp.m_builder = liveWindowFactory.get();
    }
    comp.m_liveWindow = true;
    comp.m_name = name;
  }

  /**
   * Adds an object to the registry and LiveWindow.
   *
   * @param sendable object to add
   * @param moduleType A string that defines the module name in the label for the value
   * @param channel The channel number the device is plugged into
   */
  public static synchronized void addLW(Sendable sendable, String moduleType, int channel) {
    Component comp = getOrAdd(sendable);
    if (liveWindowFactory != null) {
      comp.m_builder = liveWindowFactory.get();
    }
    comp.m_liveWindow = true;
    comp.setName(moduleType, channel);
  }

  /**
   * Adds an object to the registry and LiveWindow.
   *
   * @param sendable object to add
   * @param moduleType A string that defines the module name in the label for the value
   * @param moduleNumber The number of the particular module type
   * @param channel The channel number the device is plugged into
   */
  public static synchronized void addLW(
      Sendable sendable, String moduleType, int moduleNumber, int channel) {
    Component comp = getOrAdd(sendable);
    if (liveWindowFactory != null) {
      comp.m_builder = liveWindowFactory.get();
    }
    comp.m_liveWindow = true;
    comp.setName(moduleType, moduleNumber, channel);
  }

  /**
   * Adds an object to the registry and LiveWindow.
   *
   * @param sendable object to add
   * @param subsystem subsystem name
   * @param name component name
   */
  public static synchronized void addLW(Sendable sendable, String subsystem, String name) {
    Component comp = getOrAdd(sendable);
    if (liveWindowFactory != null) {
      comp.m_builder = liveWindowFactory.get();
    }
    comp.m_liveWindow = true;
    comp.m_name = name;
    comp.m_subsystem = subsystem;
  }

  /**
   * Adds a child object to an object. Adds the child object to the registry if it's not already
   * present.
   *
   * @param parent parent object
   * @param child child object
   */
  public static synchronized void addChild(Sendable parent, Object child) {
    Component comp = components.get(child);
    if (comp == null) {
      comp = new Component();
      components.put(child, comp);
    }
    comp.m_parent = new WeakReference<>(parent);
  }

  /**
   * Removes an object from the registry.
   *
   * @param sendable object to remove
   * @return true if the object was removed; false if it was not present
   */
  public static synchronized boolean remove(Sendable sendable) {
    return components.remove(sendable) != null;
  }

  /**
   * Determines if an object is in the registry.
   *
   * @param sendable object to check
   * @return True if in registry, false if not.
   */
  public static synchronized boolean contains(Sendable sendable) {
    return components.containsKey(sendable);
  }

  /**
   * Gets the name of an object.
   *
   * @param sendable object
   * @return Name (empty if object is not in registry)
   */
  public static synchronized String getName(Sendable sendable) {
    Component comp = components.get(sendable);
    if (comp == null) {
      return "";
    }
    return comp.m_name;
  }

  /**
   * Sets the name of an object.
   *
   * @param sendable object
   * @param name name
   */
  public static synchronized void setName(Sendable sendable, String name) {
    Component comp = components.get(sendable);
    if (comp != null) {
      comp.m_name = name;
    }
  }

  /**
   * Sets the name of an object with a channel number.
   *
   * @param sendable object
   * @param moduleType A string that defines the module name in the label for the value
   * @param channel The channel number the device is plugged into
   */
  public static synchronized void setName(Sendable sendable, String moduleType, int channel) {
    Component comp = components.get(sendable);
    if (comp != null) {
      comp.setName(moduleType, channel);
    }
  }

  /**
   * Sets the name of an object with a module and channel number.
   *
   * @param sendable object
   * @param moduleType A string that defines the module name in the label for the value
   * @param moduleNumber The number of the particular module type
   * @param channel The channel number the device is plugged into
   */
  public static synchronized void setName(
      Sendable sendable, String moduleType, int moduleNumber, int channel) {
    Component comp = components.get(sendable);
    if (comp != null) {
      comp.setName(moduleType, moduleNumber, channel);
    }
  }

  /**
   * Sets both the subsystem name and device name of an object.
   *
   * @param sendable object
   * @param subsystem subsystem name
   * @param name device name
   */
  public static synchronized void setName(Sendable sendable, String subsystem, String name) {
    Component comp = components.get(sendable);
    if (comp != null) {
      comp.m_name = name;
      comp.m_subsystem = subsystem;
    }
  }

  /**
   * Gets the subsystem name of an object.
   *
   * @param sendable object
   * @return Subsystem name (empty if object is not in registry)
   */
  public static synchronized String getSubsystem(Sendable sendable) {
    Component comp = components.get(sendable);
    if (comp == null) {
      return "";
    }
    return comp.m_subsystem;
  }

  /**
   * Sets the subsystem name of an object.
   *
   * @param sendable object
   * @param subsystem subsystem name
   */
  public static synchronized void setSubsystem(Sendable sendable, String subsystem) {
    Component comp = components.get(sendable);
    if (comp != null) {
      comp.m_subsystem = subsystem;
    }
  }

  /**
   * Gets a unique handle for setting/getting data with setData() and getData().
   *
   * @return Handle
   */
  public static synchronized int getDataHandle() {
    return nextDataHandle++;
  }

  /**
   * Associates arbitrary data with an object in the registry.
   *
   * @param sendable object
   * @param handle data handle returned by getDataHandle()
   * @param data data to set
   * @return Previous data (may be null)
   */
  public static synchronized Object setData(Sendable sendable, int handle, Object data) {
    Component comp = components.get(sendable);
    if (comp == null) {
      return null;
    }
    Object rv = null;
    if (comp.m_data == null) {
      comp.m_data = new Object[handle + 1];
    } else if (handle < comp.m_data.length) {
      rv = comp.m_data[handle];
    } else {
      comp.m_data = Arrays.copyOf(comp.m_data, handle + 1);
    }
    comp.m_data[handle] = data;
    return rv;
  }

  /**
   * Gets arbitrary data associated with an object in the registry.
   *
   * @param sendable object
   * @param handle data handle returned by getDataHandle()
   * @return data (may be null if none associated)
   */
  public static synchronized Object getData(Sendable sendable, int handle) {
    Component comp = components.get(sendable);
    if (comp == null || comp.m_data == null || handle >= comp.m_data.length) {
      return null;
    }
    return comp.m_data[handle];
  }

  /**
   * Enables LiveWindow for an object.
   *
   * @param sendable object
   */
  public static synchronized void enableLiveWindow(Sendable sendable) {
    Component comp = components.get(sendable);
    if (comp != null) {
      comp.m_liveWindow = true;
    }
  }

  /**
   * Disables LiveWindow for an object.
   *
   * @param sendable object
   */
  public static synchronized void disableLiveWindow(Sendable sendable) {
    Component comp = components.get(sendable);
    if (comp != null) {
      comp.m_liveWindow = false;
    }
  }

  /**
   * Publishes an object in the registry to a builder.
   *
   * @param sendable object
   * @param builder sendable builder
   */
  public static synchronized void publish(Sendable sendable, SendableBuilder builder) {
    Component comp = getOrAdd(sendable);
    if (comp.m_builder != null) {
      comp.m_builder.clearProperties();
    }
    comp.m_builder = builder; // clear any current builder
    sendable.initSendable(comp.m_builder);
    comp.m_builder.update();
  }

  /**
   * Updates network table information from an object.
   *
   * @param sendable object
   */
  public static synchronized void update(Sendable sendable) {
    Component comp = components.get(sendable);
    if (comp != null && comp.m_builder != null) {
      comp.m_builder.update();
    }
  }

  /** Data passed to foreachLiveWindow() callback function. */
  public static class CallbackData {
    /** Sendable object. */
    @SuppressWarnings("MemberName")
    public Sendable sendable;

    /** Name. */
    @SuppressWarnings("MemberName")
    public String name;

    /** Subsystem. */
    @SuppressWarnings("MemberName")
    public String subsystem;

    /** Parent sendable object. */
    @SuppressWarnings("MemberName")
    public Sendable parent;

    /** Data stored in object with setData(). Update this to change the data. */
    @SuppressWarnings("MemberName")
    public Object data;

    /** Sendable builder for the sendable. */
    @SuppressWarnings("MemberName")
    public SendableBuilder builder;
  }

  // As foreachLiveWindow is single threaded, cache the components it
  // iterates over to avoid risk of ConcurrentModificationException
  private static List<Component> foreachComponents = new ArrayList<>();

  /**
   * Iterates over LiveWindow-enabled objects in the registry. It is *not* safe to call other
   * SendableRegistry functions from the callback.
   *
   * @param dataHandle data handle to get data object passed to callback
   * @param callback function to call for each object
   */
  @SuppressWarnings({"PMD.AvoidCatchingThrowable", "PMD.AvoidReassigningCatchVariables"})
  public static synchronized void foreachLiveWindow(
      int dataHandle, Consumer<CallbackData> callback) {
    CallbackData cbdata = new CallbackData();
    foreachComponents.clear();
    foreachComponents.addAll(components.values());
    for (Component comp : foreachComponents) {
      if (comp.m_builder == null || comp.m_sendable == null) {
        continue;
      }
      cbdata.sendable = comp.m_sendable.get();
      if (cbdata.sendable != null && comp.m_liveWindow) {
        cbdata.name = comp.m_name;
        cbdata.subsystem = comp.m_subsystem;
        if (comp.m_parent != null) {
          cbdata.parent = comp.m_parent.get();
        } else {
          cbdata.parent = null;
        }
        if (comp.m_data != null && dataHandle < comp.m_data.length) {
          cbdata.data = comp.m_data[dataHandle];
        } else {
          cbdata.data = null;
        }
        cbdata.builder = comp.m_builder;
        try {
          callback.accept(cbdata);
        } catch (Throwable throwable) {
          Throwable cause = throwable.getCause();
          if (cause != null) {
            throwable = cause;
          }
          System.err.println("Unhandled exception calling LiveWindow for " + comp.m_name + ": ");
          throwable.printStackTrace();
          comp.m_liveWindow = false;
        }
        if (cbdata.data != null) {
          if (comp.m_data == null) {
            comp.m_data = new Object[dataHandle + 1];
          } else if (dataHandle >= comp.m_data.length) {
            comp.m_data = Arrays.copyOf(comp.m_data, dataHandle + 1);
          }
          comp.m_data[dataHandle] = cbdata.data;
        }
      }
    }
    foreachComponents.clear();
  }
}
