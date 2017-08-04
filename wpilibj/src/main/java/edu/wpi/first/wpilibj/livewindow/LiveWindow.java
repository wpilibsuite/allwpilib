/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008-2017. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.livewindow;

import java.util.Enumeration;
import java.util.Hashtable;
import java.util.Vector;

import edu.wpi.first.wpilibj.command.Scheduler;
import edu.wpi.first.wpilibj.networktables.NetworkTable;
import edu.wpi.first.wpilibj.tables.ITable;


/**
 * The LiveWindow class is the public interface for putting sensors and actuators on the
 * LiveWindow.
 */
public class LiveWindow {

  private static Vector sensors = new Vector();
  // private static Vector actuators = new Vector();
  private static Hashtable components = new Hashtable();
  private static ITable livewindowTable;
  private static ITable statusTable;
  private static boolean liveWindowEnabled = false;
  private static boolean firstTime = true;

  /**
   * Initialize all the LiveWindow elements the first time we enter LiveWindow mode. By holding off
   * creating the NetworkTable entries, it allows them to be redefined before the first time in
   * LiveWindow mode. This allows default sensor and actuator values to be created that are replaced
   * with the custom names from users calling addActuator and addSensor.
   */
  private static void initializeLiveWindowComponents() {
    System.out.println("Initializing the components first time");
    livewindowTable = NetworkTable.getTable("LiveWindow");
    statusTable = livewindowTable.getSubTable("~STATUS~");
    for (Enumeration e = components.keys(); e.hasMoreElements(); ) {
      LiveWindowSendable component = (LiveWindowSendable) e.nextElement();
      LiveWindowComponent liveWindowComponent = (LiveWindowComponent) components.get(component);
      String subsystem = liveWindowComponent.getSubsystem();
      String name = liveWindowComponent.getName();
      System.out.println("Initializing table for '" + subsystem + "' '" + name + "'");
      livewindowTable.getSubTable(subsystem).putString("~TYPE~", "LW Subsystem");
      ITable table = livewindowTable.getSubTable(subsystem).getSubTable(name);
      table.putString("~TYPE~", component.getSmartDashboardType());
      table.putString("Name", name);
      table.putString("Subsystem", subsystem);
      component.initTable(table);
      if (liveWindowComponent.isSensor()) {
        sensors.addElement(component);
      }
    }
  }

  /**
   * Set the enabled state of LiveWindow. If it's being enabled, turn off the scheduler and remove
   * all the commands from the queue and enable all the components registered for LiveWindow. If
   * it's being disabled, stop all the registered components and reenable the scheduler. TODO: add
   * code to disable PID loops when enabling LiveWindow. The commands should reenable the PID loops
   * themselves when they get rescheduled. This prevents arms from starting to move around, etc.
   * after a period of adjusting them in LiveWindow mode.
   */
  public static void setEnabled(boolean enabled) {
    if (liveWindowEnabled != enabled) {
      if (enabled) {
        System.out.println("Starting live window mode.");
        if (firstTime) {
          initializeLiveWindowComponents();
          firstTime = false;
        }
        Scheduler.getInstance().disable();
        Scheduler.getInstance().removeAll();
        for (Enumeration e = components.keys(); e.hasMoreElements(); ) {
          LiveWindowSendable component = (LiveWindowSendable) e.nextElement();
          component.startLiveWindowMode();
        }
      } else {
        System.out.println("stopping live window mode.");
        for (Enumeration e = components.keys(); e.hasMoreElements(); ) {
          LiveWindowSendable component = (LiveWindowSendable) e.nextElement();
          component.stopLiveWindowMode();
        }
        Scheduler.getInstance().enable();
      }
      liveWindowEnabled = enabled;
      statusTable.putBoolean("LW Enabled", enabled);
    }
  }

  /**
   * The run method is called repeatedly to keep the values refreshed on the screen in test mode.
   */
  public static void run() {
    updateValues();
  }

  /**
   * Add a Sensor associated with the subsystem and with call it by the given name.
   *
   * @param subsystem The subsystem this component is part of.
   * @param name      The name of this component.
   * @param component A LiveWindowSendable component that represents a sensor.
   */
  public static void addSensor(String subsystem, String name, LiveWindowSendable component) {
    components.put(component, new LiveWindowComponent(subsystem, name, true));
  }

  /**
   * Add Sensor to LiveWindow. The components are shown with the type and channel like this: Gyro[1]
   * for a gyro object connected to the first analog channel.
   *
   * @param moduleType A string indicating the type of the module used in the naming (above)
   * @param channel    The channel number the device is connected to
   * @param component  A reference to the object being added
   */
  public static void addSensor(String moduleType, int channel, LiveWindowSendable component) {
    addSensor("Ungrouped", moduleType + "[" + channel + "]", component);
    if (sensors.contains(component)) {
      sensors.removeElement(component);
    }
    sensors.addElement(component);
  }

  /**
   * Add an Actuator associated with the subsystem and with call it by the given name.
   *
   * @param subsystem The subsystem this component is part of.
   * @param name      The name of this component.
   * @param component A LiveWindowSendable component that represents a actuator.
   */
  public static void addActuator(String subsystem, String name, LiveWindowSendable component) {
    components.put(component, new LiveWindowComponent(subsystem, name, false));
  }

  /**
   * Add Actuator to LiveWindow. The components are shown with the module type, slot and channel
   * like this: Servo[1,2] for a servo object connected to the first digital module and PWM port 2.
   *
   * @param moduleType A string that defines the module name in the label for the value
   * @param channel    The channel number the device is plugged into (usually PWM)
   * @param component  The reference to the object being added
   */
  public static void addActuator(String moduleType, int channel, LiveWindowSendable component) {
    addActuator("Ungrouped", moduleType + "[" + channel + "]", component);
  }

  /**
   * Add Actuator to LiveWindow. The components are shown with the module type, slot and channel
   * like this: Servo[1,2] for a servo object connected to the first digital module and PWM port 2.
   *
   * @param moduleType   A string that defines the module name in the label for the value
   * @param moduleNumber The number of the particular module type
   * @param channel      The channel number the device is plugged into (usually PWM)
   * @param component    The reference to the object being added
   */
  public static void addActuator(String moduleType, int moduleNumber, int channel,
                                 LiveWindowSendable component) {
    addActuator("Ungrouped", moduleType + "[" + moduleNumber + "," + channel + "]", component);
  }

  /**
   * Puts all sensor values on the live window.
   */
  private static void updateValues() {
    // TODO: gross - needs to be sped up
    for (int i = 0; i < sensors.size(); i++) {
      LiveWindowSendable lws = (LiveWindowSendable) sensors.elementAt(i);
      lws.updateTable();
    }
    // TODO: Add actuators?
    // TODO: Add better rate limiting.
  }
}
