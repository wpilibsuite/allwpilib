package edu.wpi.first.wpilibj3.command.async;

@FunctionalInterface
public interface EndCondition {
  default void reset() {}

  boolean reached();

  static EndCondition not(EndCondition condition) {
    return new EndCondition() {
      @Override
      public void reset() {
        condition.reset();
      }

      @Override
      public boolean reached() {
        return !condition.reached();
      }
    };
  }

  default EndCondition or(EndCondition other) {
    return new ComposedEndCondition(this, other, (a, b) -> a || b);
  }

  default EndCondition and(EndCondition other) {
    return new ComposedEndCondition(this, other, (a, b) -> a && b);
  }

  default EndCondition xor(EndCondition other) {
    return new ComposedEndCondition(this, other, (a, b) -> a ^ b);
  }

  default EndCondition nor(EndCondition other) {
    return new ComposedEndCondition(this, other, (a, b) -> !(a || b));
  }

  default EndCondition nand(EndCondition other) {
    return new ComposedEndCondition(this, other, (a, b) -> !(a && b));
  }
}
