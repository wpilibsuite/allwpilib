/*----------------------------------------------------------------------------*/
/* Copyright (c) 2008-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.livewindow;

import edu.wpi.first.networktables.NetworkTable;
import edu.wpi.first.networktables.NetworkTableEntry;
import edu.wpi.first.networktables.NetworkTableInstance;
import edu.wpi.first.wpilibj.Sendable;
import edu.wpi.first.wpilibj.smartdashboard.SendableRegistry;


/**
 * The LiveWindow class is the public interface for putting sensors and actuators on the
 * LiveWindow.
 */
public class LiveWindow {
  private static class Component {
    boolean m_firstTime = true;
    boolean m_telemetryEnabled = true;
  }

  private static final int dataHandle = SendableRegistry.getDataHandle();
  private static final NetworkTable liveWindowTable =
      NetworkTableInstance.getDefault().getTable("LiveWindow");
  private static final NetworkTable statusTable = liveWindowTable.getSubTable(".status");
  private static final NetworkTableEntry enabledEntry = statusTable.getEntry("LW Enabled");
  private static boolean startLiveWindow;
  private static boolean liveWindowEnabled;
  private static boolean telemetryEnabled = true;

  private static Runnable enabledListener;
  private static Runnable disabledListener;

  private static Component getOrAdd(Sendable sendable) {
    Component data = (Component) SendableRegistry.getData(sendable, dataHandle);
    if (data == null) {
      data = new Component();
      SendableRegistry.setData(sendable, dataHandle, data);
    }
    return data;
  }

  private LiveWindow() {
    throw new UnsupportedOperationException("This is a utility class!");
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
   * Set the enabled state of LiveWindow. If it's being enabled, turn off the scheduler and remove
   * all the commands from the queue and enable all the components registered for LiveWindow. If
   * it's being disabled, stop all the registered components and reenable the scheduler. TODO: add
   * code to disable PID loops when enabling LiveWindow. The commands should reenable the PID loops
   * themselves when they get rescheduled. This prevents arms from starting to move around, etc.
   * after a period of adjusting them in LiveWindow mode.
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
        SendableRegistry.foreachLiveWindow(dataHandle, cbdata -> {
          cbdata.builder.stopLiveWindowMode();
        });
        if (disabledListener != null) {
          disabledListener.run();
        }
      }
      enabledEntry.setBoolean(enabled);
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
    getOrAdd(sendable).m_telemetryEnabled = true;
  }

  /**
   * Disable telemetry for a single component.
   *
   * @param sendable component
   */
  public static synchronized void disableTelemetry(Sendable sendable) {
    getOrAdd(sendable).m_telemetryEnabled = false;
  }

  /**
   * Disable ALL telemetry.
   */
  public static synchronized void disableAllTelemetry() {
    telemetryEnabled = false;
    SendableRegistry.foreachLiveWindow(dataHandle, cbdata -> {
      if (cbdata.data == null) {
        cbdata.data = new Component();
      }
      ((Component) cbdata.data).m_telemetryEnabled = false;
    });
  }

  /**
   * Tell all the sensors to update (send) their values.
   *
   * <p>Actuators are handled through callbacks on their value changing from the
   * SmartDashboard widgets.
   */
  @SuppressWarnings({"PMD.CyclomaticComplexity", "PMD.NPathComplexity"})
  public static synchronized void updateValues() {
    // Only do this if either LiveWindow mode or telemetry is enabled.
    if (!liveWindowEnabled && !telemetryEnabled) {
      return;
    }

    SendableRegistry.foreachLiveWindow(dataHandle, cbdata -> {
      if (cbdata.sendable == null || cbdata.parent != null) {
        return;
      }

      if (cbdata.data == null) {
        cbdata.data = new Component();
      }

      Component component = (Component) cbdata.data;

      if (!liveWindowEnabled && !component.m_telemetryEnabled) {
        return;
      }

      if (component.m_firstTime) {
        // By holding off creating the NetworkTable entries, it allows the
        // components to be redefined. This allows default sensor and actuator
        // values to be created that are replaced with the custom names from
        // users calling setName.
        if (cbdata.name.isEmpty()) {
          return;
        }
        NetworkTable ssTable = liveWindowTable.getSubTable(cbdata.subsystem);
        NetworkTable table;
        // Treat name==subsystem as top level of subsystem
        if (cbdata.name.equals(cbdata.subsystem)) {
          table = ssTable;
        } else {
          table = ssTable.getSubTable(cbdata.name);
        }
        table.getEntry(".name").setString(cbdata.name);
        cbdata.builder.setTable(table);
        cbdata.sendable.initSendable(cbdata.builder);
        ssTable.getEntry(".type").setString("LW Subsystem");

        component.m_firstTime = false;
      }

      if (startLiveWindow) {
        cbdata.builder.startLiveWindowMode();
      }
      cbdata.builder.updateTable();
    });

    startLiveWindow = false;
  }
}
