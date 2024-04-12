package edu.wpi.first.wpilibj3.command.async;

public record ComposedEndCondition(EndCondition a, EndCondition b, Op op) implements EndCondition {
  @FunctionalInterface
  interface Op {
    boolean call(boolean a, boolean b);
  }

  @Override
  public void reset() {
    a.reset();
    b.reset();
  }

  @Override
  public boolean reached() {
    return op.call(a.reached(), b.reached());
  }
}
