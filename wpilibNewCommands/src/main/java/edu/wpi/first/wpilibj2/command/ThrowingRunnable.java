package edu.wpi.first.wpilibj2.command;

@FunctionalInterface
public interface ThrowingRunnable {
  void run() throws Exception;
}
