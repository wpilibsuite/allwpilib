package edu.wpi.first.wpilibj.command;

import java.util.*;

public class CommandRunner {

    Map<ICommand, CommandState> m_scheduledCommands = new HashMap<>();
    Map<Subsystem, ICommand> m_requirements = new HashMap<>();
    Collection<Subsystem> m_subsystems;

    public CommandRunner(Collection<Subsystem> subsystems) {
        m_subsystems = subsystems;
    }

    public void scheduleCommand(ICommand command, boolean interruptable) {

        Collection<Subsystem> requirements = command.getRequirements();

        if (Collections.disjoint(m_requirements.keySet(), requirements)) {
            command.initialize();
            for (Subsystem requirement : requirements) {
                CommandState scheduledCommand = new CommandState(interruptable);
                m_scheduledCommands.put(command, scheduledCommand);
                m_requirements.put(requirement, command);
            }
        } else {
            boolean allInterruptible = true;
            for (Subsystem requirement : requirements) {
                if (m_requirements.keySet().contains(requirement)) {
                    allInterruptible &= m_scheduledCommands.get(m_requirements.get(requirement)).isInterruptible();
                }
            }
            if (allInterruptible) {
                for (Subsystem requirement : requirements) {
                    ICommand toInterrupt = m_requirements.get(requirement);
                    if (toInterrupt != null) {
                        toInterrupt.interrupted();
                        m_scheduledCommands.remove(toInterrupt);
                        m_requirements.keySet().removeAll(toInterrupt.getRequirements());
                    }
                }
            }
        }
    }

    public void run() {
        for (ICommand command : m_scheduledCommands.keySet()) {
            command.execute();
            if (command.isFinished()) {
                command.end();
                m_scheduledCommands.remove(command);
                m_requirements.keySet().removeAll(command.getRequirements());
            }
        }

        for (Subsystem subsystem : m_subsystems) {
            if (!m_requirements.containsKey(subsystem)) {
                if (subsystem.getDefaultICommand() != null) {
                    scheduleCommand(subsystem.getDefaultICommand(), true);
                }
            }
        }
    }

    public void cancelCommand(ICommand command) {
        command.interrupted();
        m_scheduledCommands.remove(command);
        m_requirements.keySet().removeAll(command.getRequirements());
    }

    public double timeSinceInitialized(ICommand command) {
        CommandState commandState = m_scheduledCommands.get(command);
        if (commandState != null) {
            return commandState.timeSinceInitialized();
        } else {
            return -1;
        }
    }

    public boolean isCompleted(ICommand command) {
        CommandState commandState = m_scheduledCommands.get(command);
        if (commandState != null) {
            return commandState.isCompleted();
        } else {
            return false;
        }
    }

    public boolean isRunning(ICommand command) {
        return m_scheduledCommands.containsKey(command);
    }
}