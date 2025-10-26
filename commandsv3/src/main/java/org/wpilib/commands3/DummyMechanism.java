package org.wpilib.commands3;

/** A dummy mechanism that allows inline scheduler and name specification, for use in unit tests. */
class DummyMechanism implements Mechanism {
  private final String m_name;
  private final Scheduler m_scheduler;

  /**
   * Creates a dummy mechanism.
   *
   * @param name The name of this dummy mechanism.
   * @param scheduler The registered scheduler. Cannot be null.
   */
  DummyMechanism(String name, Scheduler scheduler) {
    m_name = name;
    m_scheduler = scheduler;
  }

  @Override
  public String getName() {
    return m_name;
  }

  @Override
  public Scheduler getRegisteredScheduler() {
    return m_scheduler;
  }
}
