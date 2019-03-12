package edu.wpi.first.wpilibj.command;

import edu.wpi.first.wpilibj.smartdashboard.SendableBuilder;

import java.util.Collection;
import java.util.HashSet;

public interface ICommand {

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
        return SchedulerNew.getInstance().isRunning(this);
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
