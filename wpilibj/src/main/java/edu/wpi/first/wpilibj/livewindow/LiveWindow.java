// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj.livewindow;

import edu.wpi.first.networktables.BooleanPublisher;
import edu.wpi.first.networktables.NetworkTable;
import edu.wpi.first.networktables.NetworkTableInstance;
import edu.wpi.first.networktables.StringPublisher;
import edu.wpi.first.networktables.StringTopic;
import edu.wpi.first.util.sendable.Sendable;
import edu.wpi.first.util.sendable.SendableBuilder;
import edu.wpi.first.wpilibj.smartdashboard.SendableBuilderImpl;
import java.lang.ref.WeakReference;
import java.util.Map;
import java.util.WeakHashMap;

/**
 * The LiveWindow class is the public interface for putting sensors and actuators on the LiveWindow.
 */
@SuppressWarnings("PMD.AvoidCatchingGenericException")
public final class LiveWindow {
  private static class Component implements AutoCloseable {
    Component() {}

    Component(Sendable sendable) {
      m_sendable = new WeakReference<>(sendable);
    }

    @Override
    public void close() throws Exception {
      m_builder.close();
      if (m_namePub != null) {
        m_namePub.close();
        m_namePub = null;
      }
      if (m_typePub != null) {
        m_typePub.close();
        m_typePub = null;
      }
    }

    WeakReference<Sendable> m_sendable;
    SendableBuilder m_builder;
    String m_name;
    String m_subsystem = "Ungrouped";
    WeakReference<Sendable> m_parent;
    boolean m_firstTime = true;
    boolean m_telemetryEnabled;
    StringPublisher m_namePub;
    StringPublisher m_typePub;

    void setName(String moduleType, int channel) {
      m_name = moduleType + "[" + channel + "]";
    }

    void setName(String moduleType, int moduleNumber, int channel) {
      m_name = moduleType + "[" + moduleNumber + "," + channel + "]";
    }
  }

  private static final NetworkTable liveWindowTable =
      NetworkTableInstance.getDefault().getTable("LiveWindow");
  private static final NetworkTable statusTable = liveWindowTable.getSubTable(".status");
  private static final BooleanPublisher enabledPub =
      statusTable.getBooleanTopic("LW Enabled").publish();
  private static final Map<Object, Component> components = new WeakHashMap<>();
  private static boolean startLiveWindow;
  private static boolean liveWindowEnabled;
  private static boolean telemetryEnabled;

  private static Runnable enabledListener;
  private static Runnable disabledListener;

  static {
    enabledPub.set(false);
  }

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

  private LiveWindow() {
    throw new UnsupportedOperationException("This is a utility class!");
  }

  /**
   * Adds an object to LiveWindow.
   *
   * @param sendable Object to add
   * @param name Component name
   */
  public static synchronized void add(Sendable sendable, String name) {
    Component comp = getOrAdd(sendable);
    if (comp.m_builder != null) {
      try {
        comp.m_builder.close();
      } catch (Exception e) {
        // ignore
      }
    }
    comp.m_builder = new SendableBuilderImpl();
    comp.m_name = name;
  }

  /**
   * Adds an object to LiveWindow.
   *
   * @param sendable Object to add
   * @param moduleType A string that defines the module name in the label for the value
   * @param channel The channel number the device is plugged into
   */
  public static synchronized void add(Sendable sendable, String moduleType, int channel) {
    Component comp = getOrAdd(sendable);
    if (comp.m_builder != null) {
      try {
        comp.m_builder.close();
      } catch (Exception e) {
        // ignore
      }
    }
    comp.m_builder = new SendableBuilderImpl();
    comp.setName(moduleType, channel);
  }

  /**
   * Adds an object to LiveWindow.
   *
   * @param sendable Object to add
   * @param moduleType A string that defines the module name in the label for the value
   * @param moduleNumber The number of the particular module type
   * @param channel The channel number the device is plugged into
   */
  public static synchronized void add(
      Sendable sendable, String moduleType, int moduleNumber, int channel) {
    Component comp = getOrAdd(sendable);
    if (comp.m_builder != null) {
      try {
        comp.m_builder.close();
      } catch (Exception e) {
        // ignore
      }
    }
    comp.m_builder = new SendableBuilderImpl();
    comp.setName(moduleType, moduleNumber, channel);
  }

  /**
   * Adds an object to LiveWindow.
   *
   * @param sendable Object to add
   * @param subsystem Subsystem name
   * @param name Component name
   */
  public static synchronized void add(Sendable sendable, String subsystem, String name) {
    Component comp = getOrAdd(sendable);
    if (comp.m_builder != null) {
      try {
        comp.m_builder.close();
      } catch (Exception e) {
        // ignore
      }
    }
    comp.m_builder = new SendableBuilderImpl();

    comp.m_name = name;
    comp.m_subsystem = subsystem;
  }

  /**
   * Adds a child object to an object. Adds the child object to LiveWindow if it's not already
   * present.
   *
   * @param parent Parent object
   * @param child Child object
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
   * Removes an object from LiveWindow.
   *
   * @param sendable Object to remove
   * @return True if the object was removed; false if it was not present
   */
  public static synchronized boolean remove(Sendable sendable) {
    Component comp = components.remove(sendable);
    if (comp != null) {
      try {
        comp.close();
      } catch (Exception e) {
        // ignore
      }
    }
    return comp != null;
  }

  /**
   * Determines if an object is in LiveWindow.
   *
   * @param sendable Object to check
   * @return True if in LiveWindow, false if not.
   */
  public static synchronized boolean contains(Sendable sendable) {
    return components.containsKey(sendable);
  }

  /**
   * Gets the name of an object.
   *
   * @param sendable Object
   * @return Name (empty if object is not in LiveWindow)
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
   * @param sendable Object
   * @param name Name
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
   * @param sendable Object
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
   * @param sendable Object
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
   * @param sendable Object
   * @param subsystem Subsystem name
   * @param name Device name
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
   * @param sendable Object
   * @return Subsystem name (empty if object is not in LiveWindow)
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
   * @param sendable Object
   * @param subsystem Subsystem name
   */
  public static synchronized void setSubsystem(Sendable sendable, String subsystem) {
    Component comp = components.get(sendable);
    if (comp != null) {
      comp.m_subsystem = subsystem;
    }
  }

  public static synchronized void setEnabledListener(Runnable runnable) {
    enabledListener = runnable;
  }

  public static synchronized void setDisabledListener(Runnable runnable) {
    disabledListener = runnable;
  }

  public static synchronized boolean isEnabled() {
    return liveWindowEnabled;
  }

  /**
   * Set the enabled state of LiveWindow.
   *
   * <p>If it's being enabled, turn off the scheduler and remove all the commands from the queue and
   * enable all the components registered for LiveWindow. If it's being disabled, stop all the
   * registered components and re-enable the scheduler.
   *
   * <p>TODO: add code to disable PID loops when enabling LiveWindow. The commands should re-enable
   * the PID loops themselves when they get rescheduled. This prevents arms from starting to move
   * around, etc. after a period of adjusting them in LiveWindow mode.
   *
   * @param enabled True to enable LiveWindow.
   */
  public static synchronized void setEnabled(boolean enabled) {
    if (liveWindowEnabled != enabled) {
      startLiveWindow = enabled;
      liveWindowEnabled = enabled;
      updateValues(); // Force table generation now to make sure everything is defined
      if (enabled) {
        System.out.println("Starting live window mode.");
        if (enabledListener != null) {
          enabledListener.run();
        }
      } else {
        System.out.println("stopping live window mode.");
        for (Component comp : components.values()) {
          ((SendableBuilderImpl) comp.m_builder).stopLiveWindowMode();
        }
        if (disabledListener != null) {
          disabledListener.run();
        }
      }
      enabledPub.set(enabled);
    }
  }

  /**
   * Enable telemetry for a single component.
   *
   * @param sendable Component
   */
  public static synchronized void enableTelemetry(Sendable sendable) {
    // Re-enable global setting in case disableAllTelemetry() was called.
    telemetryEnabled = true;
    getOrAdd(sendable).m_telemetryEnabled = true;
  }

  /**
   * Disable telemetry for a single component.
   *
   * @param sendable Component
   */
  public static synchronized void disableTelemetry(Sendable sendable) {
    getOrAdd(sendable).m_telemetryEnabled = false;
  }

  /** Disable ALL telemetry. */
  public static synchronized void disableAllTelemetry() {
    telemetryEnabled = false;
    for (Component comp : components.values()) {
      comp.m_telemetryEnabled = false;
    }
  }

  /** Enable ALL telemetry. */
  public static synchronized void enableAllTelemetry() {
    telemetryEnabled = true;
    for (Component comp : components.values()) {
      comp.m_telemetryEnabled = true;
    }
  }

  /**
   * Tell all the sensors to update (send) their values.
   *
   * <p>Actuators are handled through callbacks on their value changing from the SmartDashboard
   * widgets.
   */
  public static synchronized void updateValues() {
    // Only do this if either LiveWindow mode or telemetry is enabled.
    if (!liveWindowEnabled && !telemetryEnabled) {
      return;
    }

    for (Component comp : components.values()) {
      if (comp.m_sendable == null || comp.m_parent != null) {
        continue;
      }
      if (!liveWindowEnabled && !comp.m_telemetryEnabled) {
        return;
      }

      if (comp.m_firstTime) {
        // By holding off creating the NetworkTable entries, it allows the
        // components to be redefined. This allows default sensor and actuator
        // values to be created that are replaced with the custom names from
        // users calling setName.
        if (comp.m_name.isEmpty()) {
          return;
        }
        NetworkTable ssTable = liveWindowTable.getSubTable(comp.m_subsystem);
        NetworkTable table;
        // Treat name==subsystem as top level of subsystem
        if (comp.m_name.equals(comp.m_subsystem)) {
          table = ssTable;
        } else {
          table = ssTable.getSubTable(comp.m_name);
        }
        comp.m_namePub = new StringTopic(table.getTopic(".name")).publish();
        comp.m_namePub.set(comp.m_name);
        ((SendableBuilderImpl) comp.m_builder).setTable(table);
        comp.m_sendable.get().initSendable(comp.m_builder);
        comp.m_typePub = new StringTopic(ssTable.getTopic(".type")).publish();
        comp.m_typePub.set("LW Subsystem");

        comp.m_firstTime = false;
      }

      if (startLiveWindow) {
        ((SendableBuilderImpl) comp.m_builder).startLiveWindowMode();
      }
      comp.m_builder.update();
    }

    startLiveWindow = false;
  }
}
