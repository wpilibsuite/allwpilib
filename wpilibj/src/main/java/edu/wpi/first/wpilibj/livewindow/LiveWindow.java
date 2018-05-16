/*----------------------------------------------------------------------------*/
/* Copyright (c) 2008-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.livewindow;

import java.util.HashMap;
import java.util.Map;

import edu.wpi.first.networktables.NetworkTable;
import edu.wpi.first.networktables.NetworkTableEntry;
import edu.wpi.first.networktables.NetworkTableInstance;
import edu.wpi.first.wpilibj.command.Scheduler;
import edu.wpi.first.wpilibj.smartdashboard.SendableBuilderImpl;
import edu.wpi.first.wpilibj.Sendable;


/**
 * The LiveWindow class is the public interface for putting sensors and actuators on the
 * LiveWindow.
 */
public class LiveWindow {
  private static class Component {
    Component(Sendable sendable, Sendable parent) {
      m_sendable = sendable;
      m_parent = parent;
    }

    final Sendable m_sendable;
    Sendable m_parent;
    final SendableBuilderImpl m_builder = new SendableBuilderImpl();
    boolean m_firstTime = true;
    boolean m_telemetryEnabled = true;
  }

  private static final Map<Object, Component> components = new HashMap<>();
  private static final NetworkTable liveWindowTable =
      NetworkTableInstance.getDefault().getTable("LiveWindow");
  private static final NetworkTable statusTable = liveWindowTable.getSubTable(".status");
  private static final NetworkTableEntry enabledEntry = statusTable.getEntry("LW Enabled");
  private static boolean startLiveWindow = false;
  private static boolean liveWindowEnabled = false;
  private static boolean telemetryEnabled = true;

  public static synchronized boolean isEnabled() {
    return liveWindowEnabled;
  }

  /**
   * Set the enabled state of LiveWindow. If it's being enabled, turn off the scheduler and remove
   * all the commands from the queue and enable all the components registered for LiveWindow. If
   * it's being disabled, stop all the registered components and reenable the scheduler. TODO: add
   * code to disable PID loops when enabling LiveWindow. The commands should reenable the PID loops
   * themselves when they get rescheduled. This prevents arms from starting to move around, etc.
   * after a period of adjusting them in LiveWindow mode.
   */
  public static synchronized void setEnabled(boolean enabled) {
    if (liveWindowEnabled != enabled) {
      Scheduler scheduler = Scheduler.getInstance();
      if (enabled) {
        System.out.println("Starting live window mode.");
        scheduler.disable();
        scheduler.removeAll();
      } else {
        System.out.println("stopping live window mode.");
        for (Component component : components.values()) {
          component.m_builder.stopLiveWindowMode();
        }
        scheduler.enable();
      }
      startLiveWindow = enabled;
      liveWindowEnabled = enabled;
      enabledEntry.setBoolean(enabled);
    }
  }

  /**
   * The run method is called repeatedly to keep the values refreshed on the screen in test mode.
   * @deprecated No longer required
   */
  @Deprecated
  public static void run() {
    updateValues();
  }

  /**
   * Add a Sensor associated with the subsystem and with call it by the given name.
   *
   * @param subsystem The subsystem this component is part of.
   * @param name      The name of this component.
   * @param component A LiveWindowSendable component that represents a sensor.
   * @deprecated Use {@link Sendable#setName(String, String)} instead.
   */
  @Deprecated
  public static synchronized void addSensor(String subsystem, String name, Sendable component) {
    add(component);
    component.setName(subsystem, name);
  }

  /**
   * Add Sensor to LiveWindow. The components are shown with the type and channel like this: Gyro[1]
   * for a gyro object connected to the first analog channel.
   *
   * @param moduleType A string indicating the type of the module used in the naming (above)
   * @param channel    The channel number the device is connected to
   * @param component  A reference to the object being added
   * @deprecated Use {@link edu.wpi.first.wpilibj.SensorBase#setName(String, int)} instead.
   */
  @Deprecated
  public static void addSensor(String moduleType, int channel, Sendable component) {
    add(component);
    component.setName("Ungrouped", moduleType + "[" + channel + "]");
  }

  /**
   * Add an Actuator associated with the subsystem and with call it by the given name.
   *
   * @param subsystem The subsystem this component is part of.
   * @param name      The name of this component.
   * @param component A LiveWindowSendable component that represents a actuator.
   * @deprecated Use {@link Sendable#setName(String, String)} instead.
   */
  @Deprecated
  public static synchronized void addActuator(String subsystem, String name, Sendable component) {
    add(component);
    component.setName(subsystem, name);
  }

  /**
   * Add Actuator to LiveWindow. The components are shown with the module type, slot and channel
   * like this: Servo[1,2] for a servo object connected to the first digital module and PWM port 2.
   *
   * @param moduleType A string that defines the module name in the label for the value
   * @param channel    The channel number the device is plugged into (usually PWM)
   * @param component  The reference to the object being added
   * @deprecated Use {@link edu.wpi.first.wpilibj.SensorBase#setName(String, int)} instead.
   */
  @Deprecated
  public static void addActuator(String moduleType, int channel, Sendable component) {
    add(component);
    component.setName("Ungrouped", moduleType + "[" + channel + "]");
  }

  /**
   * Add Actuator to LiveWindow. The components are shown with the module type, slot and channel
   * like this: Servo[1,2] for a servo object connected to the first digital module and PWM port 2.
   *
   * @param moduleType   A string that defines the module name in the label for the value
   * @param moduleNumber The number of the particular module type
   * @param channel      The channel number the device is plugged into (usually PWM)
   * @param component    The reference to the object being added
   * @deprecated Use {@link edu.wpi.first.wpilibj.SensorBase#setName(String, int, int)} instead.
   */
  @Deprecated
  public static void addActuator(String moduleType, int moduleNumber, int channel,
                                 Sendable component) {
    add(component);
    component.setName("Ungrouped", moduleType + "[" + moduleNumber + "," + channel + "]");
  }

  /**
   * Add a component to the LiveWindow.
   *
   * @param sendable component to add
   */
  public static synchronized void add(Sendable sendable) {
    components.putIfAbsent(sendable, new Component(sendable, null));
  }

  /**
   * Add a child component to a component.
   *
   * @param parent parent component
   * @param child child component
   */
  public static synchronized void addChild(Sendable parent, Object child) {
    Component component = components.get(child);
    if (component == null) {
      component = new Component(null, parent);
      components.put(child, component);
    } else {
      component.m_parent = parent;
    }
    component.m_telemetryEnabled = false;
  }

  /**
   * Remove a component from the LiveWindow.
   *
   * @param sendable component to remove
   */
  public static synchronized void remove(Sendable sendable) {
    Component component = components.remove(sendable);
    if (component != null && isEnabled()) {
      component.m_builder.stopLiveWindowMode();
    }
  }

  /**
   * Enable telemetry for a single component.
   *
   * @param sendable component
   */
  public static synchronized void enableTelemetry(Sendable sendable) {
    // Re-enable global setting in case disableAllTelemetry() was called.
    telemetryEnabled = true;
    Component component = components.get(sendable);
    if (component != null) {
      component.m_telemetryEnabled = true;
    }
  }

  /**
   * Disable telemetry for a single component.
   *
   * @param sendable component
   */
  public static synchronized void disableTelemetry(Sendable sendable) {
    Component component = components.get(sendable);
    if (component != null) {
      component.m_telemetryEnabled = false;
    }
  }

  /**
   * Disable ALL telemetry.
   */
  public static synchronized void disableAllTelemetry() {
    telemetryEnabled = false;
    for (Component component : components.values()) {
      component.m_telemetryEnabled = false;
    }
  }

  /**
   * Tell all the sensors to update (send) their values.
   *
   * <p>Actuators are handled through callbacks on their value changing from the
   * SmartDashboard widgets.
   */
  public static synchronized void updateValues() {
    // Only do this if either LiveWindow mode or telemetry is enabled.
    if (!liveWindowEnabled && !telemetryEnabled) {
      return;
    }

    for (Component component : components.values()) {
      if (component.m_sendable != null && component.m_parent == null
          && (liveWindowEnabled || component.m_telemetryEnabled)) {
        if (component.m_firstTime) {
          // By holding off creating the NetworkTable entries, it allows the
          // components to be redefined. This allows default sensor and actuator
          // values to be created that are replaced with the custom names from
          // users calling setName.
          String name = component.m_sendable.getName();
          if (name.isEmpty()) {
            continue;
          }
          String subsystem = component.m_sendable.getSubsystem();
          NetworkTable ssTable = liveWindowTable.getSubTable(subsystem);
          NetworkTable table;
          // Treat name==subsystem as top level of subsystem
          if (name.equals(subsystem)) {
            table = ssTable;
          } else {
            table = ssTable.getSubTable(name);
          }
          table.getEntry(".name").setString(name);
          component.m_builder.setTable(table);
          component.m_sendable.initSendable(component.m_builder);
          ssTable.getEntry(".type").setString("LW Subsystem");

          component.m_firstTime = false;
        }

        if (startLiveWindow) {
          component.m_builder.startLiveWindowMode();
        }
        component.m_builder.updateTable();
      }
    }

    startLiveWindow = false;
  }
}
