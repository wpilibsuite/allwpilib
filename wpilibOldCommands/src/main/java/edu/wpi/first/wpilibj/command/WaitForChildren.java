// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj.command;

/**
 * This command will only finish if whatever {@link CommandGroup} it is in has no active children.
 * If it is not a part of a {@link CommandGroup}, then it will finish immediately. If it is itself
 * an active child, then the {@link CommandGroup} will never end.
 *
 * <p>This class is useful for the situation where you want to allow anything running in parallel
 * to finish, before continuing in the main {@link CommandGroup} sequence.
 */
public class WaitForChildren extends Command {
  @Override
  protected boolean isFinished() {
    return getGroup() == null || getGroup().m_children.isEmpty();
  }
}
