package edu.wpi.first.wpilibj.command;


import java.util.Collection;
import java.util.HashMap;
import java.util.HashSet;
import java.util.Map;

public class ParallelCommandGroup implements ICommand {

    private final Map<ICommand, Boolean> m_commands = new HashMap<>();

    public ParallelCommandGroup(ICommand[] commands) {
        for (ICommand command : commands) {
            m_commands.put(command, true);
        }
    }

    @Override
    public void initialize() {
        for (ICommand command : m_commands.keySet()) {
            command.initialize();
            m_commands.replace(command, true);
        }
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
            if (m_commands.get(command)) {
                command.interrupted();
            }
        }
    }

    @Override
    public void end() {
    }

    @Override
    public boolean isFinished() {
        return !m_commands.values().contains(true);
    }

    @Override
    public Collection<Subsystem> getRequirements() {
        Collection<Subsystem> requirements = new HashSet<>();
        for (ICommand command : m_commands.keySet()) {
            requirements.addAll(command.getRequirements());
        }
        return requirements;
    }
}
