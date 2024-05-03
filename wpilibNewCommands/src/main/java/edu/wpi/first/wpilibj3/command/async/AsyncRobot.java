package edu.wpi.first.wpilibj3.command.async;

import static edu.wpi.first.units.Units.Milliseconds;
import static edu.wpi.first.units.Units.Nanoseconds;
import static edu.wpi.first.units.Units.Seconds;

import edu.wpi.first.units.Measure;
import edu.wpi.first.units.Time;
import edu.wpi.first.wpilibj.IterativeRobotBase;
import java.util.concurrent.ExecutionException;
import java.util.concurrent.Executors;
import java.util.concurrent.atomic.AtomicBoolean;

public class AsyncRobot extends IterativeRobotBase {
  public static final Measure<Time> DEFAULT_PERIOD = Milliseconds.of(20);
  private final Measure<Time> period;
  protected final AsyncScheduler scheduler;

  private final AtomicBoolean ended = new AtomicBoolean(false);

  public AsyncRobot(Measure<Time> period, AsyncScheduler scheduler) {
    super(period.in(Seconds));
    this.period = period;
    this.scheduler = scheduler;
  }

  public AsyncRobot() {
    this(DEFAULT_PERIOD, AsyncScheduler.getInstance());
  }

  @Override
  public void startCompetition() {
    var future =
        Executors
            .newThreadPerTaskExecutor(Thread.ofVirtual().name("main vthread").factory())
            .submit(() -> {
              robotInit();

              while (!ended.get()) {
                sleep();

                loopFunc();
                scheduler.checkForErrors();
              }

              return null;
            });

    try {
      future.get();
    } catch (InterruptedException e) {
      throw new RuntimeException("The main thread was interrupted while running your robot code!", e);
    } catch (ExecutionException e) {
      throw new RuntimeException("Robot's shouldn't quit, but yours did!", e);
    }
  }

  @Override
  public void endCompetition() {
    ended.set(true);
  }

  private void sleep() throws InterruptedException {
    long ms = (long) period.in(Milliseconds);
    int ns = (int) (period.in(Nanoseconds) % 1e6);

    Thread.sleep(ms, ns);
  }
}
