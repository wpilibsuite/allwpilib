package edu.wpi.first.wpilibj.command;

import java.util.*;

public class ParallelCommandGroup implements ICommand {

    private final Map<ICommand, Boolean> m_commands = new HashMap<>();

    public ParallelCommandGroup(ICommand[] commands) {
        for (ICommand command : commands) {
            m_commands.put(command, true);
        }
    }

    @Override
    public void initialize() {
        m_commands.keySet().forEach(ICommand::initialize);
    }

    @Override
    public void execute() {
        for (ICommand command : m_commands.keySet()) {
            if (command.isFinished()) {
                command.end();
                m_commands.replace(command, false);
            } else {
                command.execute();
            }
        }
    }

    @Override
    public void interrupted() {
        for (ICommand command : m_commands.keySet()) {
            if (!m_commands.get(command)) {
                command.interrupted();
            }
        }
    }

    @Override
    public void end() {
        for (ICommand command : m_commands.keySet()) {
            if (!m_commands.get(command)) {
                command.end();
            }
        }
    }

    @Override
    public boolean isFinished() {
        return !m_commands.values().contains(true);
    }

    @Override
    public Collection<Subsystem> getRequirements() {
        Collection<Subsystem> requirements = new HashSet<>();
        m_commands.keySet().forEach(command -> requirements.addAll(command.getRequirements()));
        return requirements;
    }
}
