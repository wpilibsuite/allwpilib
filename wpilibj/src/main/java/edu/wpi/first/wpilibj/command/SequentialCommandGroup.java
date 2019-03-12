package edu.wpi.first.wpilibj.command;

import java.util.*;

public class SequentialCommandGroup implements ICommand {

    private final List<ICommand> m_commands = new ArrayList<>();
    private Queue<ICommand> m_commandQueue;

    public SequentialCommandGroup(ICommand[] commands) {
        m_commands.addAll(List.of(commands));
    }

    @Override
    public void initialize() {
        m_commandQueue = new ArrayDeque<>(m_commands);
        m_commandQueue.peek().initialize();
    }

    @Override
    public void execute() {
        m_commandQueue.peek().execute();
        if (m_commandQueue.peek().isFinished()) {
            m_commandQueue.remove().end();
            if (!m_commandQueue.isEmpty()) {
                m_commandQueue.peek().initialize();
            }
        }
    }

    @Override
    public void interrupted() {
        m_commandQueue.peek().interrupted();
    }

    @Override
    public void end() {
    }

    @Override
    public boolean isFinished() {
        return m_commandQueue.isEmpty();
    }

    @Override
    public Collection<Subsystem> getRequirements() {
        Collection<Subsystem> requirements = new HashSet<>();
        m_commands.forEach(command -> requirements.addAll(command.getRequirements()));
        return requirements;
    }
}
