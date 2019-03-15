package edu.wpi.first.wpilibj.experimental.command;

import java.util.HashSet;
import java.util.Set;

import edu.wpi.first.wpilibj.Sendable;
import edu.wpi.first.wpilibj.smartdashboard.SendableBuilder;

@SuppressWarnings("PMD.TooManyMethods")
public interface Command extends Sendable {

  void initialize();

  void execute();

  default void interrupted() {
    end();
  }

  void end();

  default boolean isFinished() {
    return false;
  }

  default Set<Subsystem> getRequirements() {
    return new HashSet<>();
  }

  default Command withTimeout(double seconds) {
    return new ParallelCommandRace(this, new WaitCommand(seconds));
  }

  default void schedule(boolean interruptible) {
    CommandScheduler.getInstance().scheduleCommand(this, interruptible);
  }

  default void schedule() {
    schedule(true);
  }

  default void cancel() {
    CommandScheduler.getInstance().cancelCommand(this);
  }

  default boolean isScheduled() {
    return CommandScheduler.getInstance().isScheduled(this);
  }

  default boolean requires(Subsystem requirement) {
    return getRequirements().contains(requirement);
  }

  default boolean runsWhenDisabled() {
    return false;
  }

  @Override
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
  @Override
  default void initSendable(SendableBuilder builder) {
    builder.setSmartDashboardType("Command");
    builder.addStringProperty(".name", this::getName, null);
    builder.addBooleanProperty("running", this::isScheduled, value -> {
      if (value) {
        if (!isScheduled()) {
          schedule(true);
        }
      } else {
        if (isScheduled()) {
          cancel();
        }
      }
    });
  }
}
