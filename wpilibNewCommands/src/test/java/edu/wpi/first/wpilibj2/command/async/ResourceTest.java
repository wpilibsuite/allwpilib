package edu.wpi.first.wpilibj2.command.async;

import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertSame;
import static org.junit.jupiter.api.Assertions.assertThrows;
import static org.junit.jupiter.api.Assertions.assertTrue;

import java.util.concurrent.Callable;
import java.util.concurrent.ExecutionException;
import java.util.concurrent.atomic.AtomicBoolean;
import java.util.concurrent.atomic.AtomicLong;
import org.junit.jupiter.api.RepeatedTest;
import org.junit.jupiter.api.Test;
import org.junit.jupiter.api.Timeout;

@Timeout(1) // 1 second timeout
class ResourceTest {
  private static <T> Callable<T> sleepImpl() {
    return () -> {
      Thread.sleep(Long.MAX_VALUE);
      return null;
    };
  }

  @Test
  void runsIdleByDefault() {
    var resource = new R1();
    assertSame(resource.getIdleTask(), resource.getCurrentTask());
  }

  @RepeatedTest(10)
  void runsCustomDefaultTask() {
    var resource = new R1();
    var d1 = new Task<>("d1", resource, sleepImpl());
    var d2 = new Task<>("d2", resource, sleepImpl());

    resource.setDefaultTask(d1);
    assertSame(d1, resource.getCurrentTask(), "Initial idle task should be replaced");

    resource.setDefaultTask(d2);
    assertSame(d2, resource.getCurrentTask(), "First default task should be replaced");
  }

  @RepeatedTest(10)
  void interruptsExistingTask() throws Exception {
    var resource = new R1();

    var firstTaskCount = new AtomicLong(0);

    var firstTask = new Task<>("first", resource, 0, () -> {
      while (true) {
        firstTaskCount.incrementAndGet();
        Thread.sleep(1);
      }
    });

    var secondTask = new Task<>("second", resource, 100, () -> {
      Thread.sleep(5);
      return "the second";
    });

    resource.runTask(firstTask);
    assertSame(firstTask, resource.getCurrentTask());
    Thread.sleep(100);

    resource.runTask(secondTask);

    // cache after cancellation to avoid one last increment sneaking in
    var finalCount = firstTaskCount.get();

    assertSame(secondTask, resource.getCurrentTask());

    Thread.sleep(20); // wait a little to be sure to accumulate more iterations
    assertEquals(finalCount, firstTaskCount.get(), "First task should stop running");
  }

  @RepeatedTest(10)
  void queuesDefaultOnSuccess() throws Exception {
    var resource = new R1();
    var defaultTask = new Task<>("default", resource, sleepImpl());

    var someTaskRan = new AtomicBoolean(false);
    var someTask = new Task<>("some task", resource, () -> {
      someTaskRan.set(true);
      return "the result";
    });

    resource.setDefaultTask(defaultTask);
    assertSame(defaultTask, resource.getCurrentTask(), "Configured default task should be running");

    var result = resource.runTask(someTask).get();
    assertEquals("the result", result);
    assertTrue(someTaskRan.get());

    assertSame(defaultTask, resource.getCurrentTask(), "Configured default task should be requeued");
  }

  @RepeatedTest(10)
  void queuesDefaultOnError() {
    var resource = new R1();
    var defaultTask = new Task<>("default", resource, sleepImpl());
    resource.setDefaultTask(defaultTask);

    var badTask = new Task<>("bad", resource, () -> {
      throw new RuntimeException("got an error");
    });

    var f = resource.runTask(badTask);
    assertThrows(ExecutionException.class, f::get);
    assertSame(defaultTask, resource.getCurrentTask());
  }

  static final class R1 extends Resource<R1> {
    public R1() {
      super("R1");
    }
  }
}
