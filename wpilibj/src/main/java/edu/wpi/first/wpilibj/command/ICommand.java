package edu.wpi.first.wpilibj.command;

import java.util.Collection;
import java.util.HashSet;

import edu.wpi.first.wpilibj.Sendable;
import edu.wpi.first.wpilibj.smartdashboard.SendableBuilder;

public interface ICommand extends Sendable {

  void initialize();

  void execute();

  default void interrupted() {
    end();
  }

  void end();

  default boolean isFinished() {
    return false;
  }

  default Collection<Subsystem> getRequirements() {
    return new HashSet<>();
  }

  default void start(boolean interruptible) {
    SchedulerNew.getInstance().scheduleCommand(this, interruptible);
  }

  default void cancel() {
    SchedulerNew.getInstance().cancelCommand(this);
  }

  default boolean isRunning() {
    return SchedulerNew.getInstance().isScheduled(this);
  }

  default boolean requires(Subsystem requirement) {
    return getRequirements().contains(requirement);
  }

  default boolean getRunWhenDisabled() {
    return false;
  }

  default String getName() {
    return this.getClass().getSimpleName();
  }

  @Override
  default void setName(String name) {
  }

  @Override
  default void setName(String subsystem, String name) {
  }

  @Override
  default String getSubsystem() {
    return "Ungrouped";
  }

  @Override
  default void setSubsystem(String subsystem) {
  }

  /**
   * Initializes this sendable.  Useful for allowing implementations to easily extend SendableBase.
   *
   * @param builder the builder used to construct this sendable
   */
  default void initSendable(SendableBuilder builder) {
    builder.setSmartDashboardType("Command");
    builder.addStringProperty(".name", this::getName, null);
    builder.addBooleanProperty("running", this::isRunning, value -> {
      if (value) {
        if (!isRunning()) {
          start(true);
        }
      } else {
        if (isRunning()) {
          cancel();
        }
      }
    });
  }
}
