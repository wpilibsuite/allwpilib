package edu.wpi.first.wpilibj.command;

import edu.wpi.first.wpilibj.smartdashboard.SendableBuilder;

import java.util.Collection;
import java.util.HashSet;

public interface ICommand {

    void initialize();

    void execute();

    void interrupted();

    void end();

    default boolean isFinished() {
        return false;
    }

    default Collection<Subsystem> getRequirements() {
        return new HashSet<>();
    }

    default void start() {
        //TODO: add this to scheduler here
    }

    default void cancel() {
        //TODO: scheduler call to cancel from here
    }

    default boolean isRunning() {
        //TODO: get this from the scheduler to return the necessary value
        return false;
    }

    default boolean doesRequire(Subsystem requirement) {
        return getRequirements().contains(requirement);
    }

    default boolean getRunWhenDisabled() {
        return false;
    }

    default String getName() {
        return this.getClass().getSimpleName();
    }

    default boolean isParented() {
        //TODO: Get this from scheduler to return the correct value
        return false;
    }

    default void initSendable(SendableBuilder builder) {
        builder.setSmartDashboardType("Command");
        builder.addStringProperty(".name", this::getName, null);
        builder.addBooleanProperty("running", this::isRunning, value -> {
            if (value) {
                if (!isRunning()) {
                    start();
                }
            } else {
                if (isRunning()) {
                    cancel();
                }
            }
        });
        builder.addBooleanProperty(".isParented", this::isParented, null);
    }
}
