package edu.wpi.first.wpilibj.command;

import java.util.*;

public class SequentialCommandGroup implements ICommand {

    private final Queue<ICommand> m_commands = new ArrayDeque<>();

    public SequentialCommandGroup(ICommand... commands) {
        m_commands.addAll(List.of(commands));
    }

    @Override
    public void initialize() {
        m_commands.peek().initialize();
    }

    @Override
    public void execute() {
        m_commands.peek().execute();
        if (m_commands.peek().isFinished()) {
            m_commands.remove().end();
            if(!m_commands.isEmpty()) {
                m_commands.peek().initialize();
            }
        }
    }

    @Override
    public void interrupted() {
        m_commands.peek().interrupted();
    }

    @Override
    public void end() {
    }

    @Override
    public boolean isFinished() {
        return m_commands.isEmpty();
    }

    @Override
    public Collection<Subsystem> getRequirements() {
        Collection<Subsystem> requirements = new HashSet<>();
        m_commands.forEach(command -> requirements.addAll(command.getRequirements()));
        return requirements;
    }
}
