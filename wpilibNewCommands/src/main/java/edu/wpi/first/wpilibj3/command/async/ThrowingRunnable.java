package edu.wpi.first.wpilibj3.command.async;

@FunctionalInterface
public interface ThrowingRunnable {
  void run() throws Exception;
}
