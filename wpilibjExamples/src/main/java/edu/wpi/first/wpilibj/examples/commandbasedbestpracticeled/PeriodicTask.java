package frc.robot;

/**
Wrapper for TimeRobot.addPeriodic() to pass for other classes usage

Example Usage:
file Robot.java:
new RobotContainer(this::addPeriodic);

file RobotConatiner.java:
RobotContainer(PeriodicTask periodicTask) {
   periodicTask.register( ()->System.out.println("periodically printing"), 1., 0.01);
  }

 */
@FunctionalInterface
public interface PeriodicTask {
    /**
     * @param callback - task to run periodically
     * @param periodSeconds - time between runs
     * @param offsetSeconds - offset from standard loop start time
     */
    void register(Runnable callback, double periodSeconds , double offsetSeconds);
}