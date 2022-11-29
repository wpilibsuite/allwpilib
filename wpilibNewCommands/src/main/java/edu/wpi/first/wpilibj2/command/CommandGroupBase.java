// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj2.command;

/**
 * A base for CommandGroups.
 *
 * <p>This class is provided by the NewCommands VendorDep
 *
 * @deprecated This class is an empty abstraction. Inherit directly from CommandBase/Command.
 */
@Deprecated(forRemoval = true)
public abstract class CommandGroupBase extends CommandBase {

  /**
   * Adds the given commands to the command group.
   *
   * @param commands The commands to add.
   */
  public abstract void addCommands(Command... commands);

  /**
   * Factory method for {@link SequentialCommandGroup}, included for brevity/convenience.
   *
   * @param commands the commands to include
   * @return the command group
   * @deprecated Replace with {@link Commands#sequence(Command...)}
   */
  @Deprecated
  public static SequentialCommandGroup sequence(Command... commands) {
    return new SequentialCommandGroup(commands);
  }

  /**
   * Factory method for {@link ParallelCommandGroup}, included for brevity/convenience.
   *
   * @param commands the commands to include
   * @return the command group
   * @deprecated Replace with {@link Commands#parallel(Command...)}
   */
  @Deprecated
  public static ParallelCommandGroup parallel(Command... commands) {
    return new ParallelCommandGroup(commands);
  }

  /**
   * Factory method for {@link ParallelRaceGroup}, included for brevity/convenience.
   *
   * @param commands the commands to include
   * @return the command group
   * @deprecated Replace with {@link Commands#race(Command...)}
   */
  @Deprecated
  public static ParallelRaceGroup race(Command... commands) {
    return new ParallelRaceGroup(commands);
  }

  /**
   * Factory method for {@link ParallelDeadlineGroup}, included for brevity/convenience.
   *
   * @param deadline the deadline command
   * @param commands the commands to include
   * @return the command group
   * @deprecated Replace with {@link Commands#deadline(Command, Command...)}
   */
  @Deprecated
  public static ParallelDeadlineGroup deadline(Command deadline, Command... commands) {
    return new ParallelDeadlineGroup(deadline, commands);
  }
}
