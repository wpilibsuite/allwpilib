package edu.wpi.first.wpilibj.commands.wpilibj2.paralleldeadlinegroup;

import edu.wpi.first.wpilibj2.command.InstantCommand;
import edu.wpi.first.wpilibj2.command.ParallelDeadlineGroup;

// NOTE:  Consider using this command inline, rather than writing a subclass.  For more
// information, see:
// https://docs.wpilib.org/en/latest/docs/software/commandbased/convenience-features.html
public class ReplaceMeParallelDeadlineGroup extends ParallelDeadlineGroup {

  public ReplaceMeParallelDeadlineGroup() {
    super(
        // Add your deadline command here
        new InstantCommand()
        //Add your other commands here
    );
  }
}