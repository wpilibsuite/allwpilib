// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.commands3;

import static edu.wpi.first.util.ErrorMessages.requireNonNullParam;

import java.util.Arrays;
import java.util.Collection;
import java.util.HashSet;
import java.util.Set;
import java.util.function.BooleanSupplier;
import java.util.function.Consumer;

/**
 * A builder class for commands. Command configuration is done in stages, where later stages have
 * different configuration options than earlier stages. Commands may only be created after going
 * through every stage, enforcing the presence of required options. All commands <i>must</i> have a
 * set of requirements (which may be empty), a name, and an implementation. The builder stages are
 * defined such that the final stage that creates a command can only be reached after going through
 * the earlier stages to configure those required options.
 *
 * <p>Example usage:
 *
 * <pre>{@code
 * StagedCommandBuilder start = new StagedCommandBuilder();
 * HasRequirementsCommandBuilder withRequirements = start.requiring(mechanism1, mechanism2);
 * HasExecutionCommandBuilder withExecution = withRequirements.executing(coroutine -> ...);
 * TerminalCommandBuilder terminal = withExecution.withName("Example Command");
 * Command exampleCommand = terminal.make();
 * }</pre>
 *
 * <p>Because every method on the builders returns a builder object, these calls can be chained to
 * cut down on verbosity and make the code easier to read. This is the recommended style:
 *
 * <pre>{@code
 * Command exampleCommand =
 *   new StagedCommandBuilder()
 *     .requiring(mechanism1, mechanism2)
 *     .executing(coroutine -> ...)
 *     .withName("Example Command")
 *     .make();
 * }</pre>
 *
 * <p>And can be cut down even further by using helper methods provided by the library:
 *
 * <pre>{@code
 * Command exampleCommand =
 *   Command
 *     .requiring(mechanism1, mechanism2)
 *     .executing(coroutine -> ...)
 *     .named("Example Command")
 * }</pre>
 */
public class StagedCommandBuilder implements StartingCommandBuilder {
  private final Set<Mechanism> m_requirements = new HashSet<>();
  private Consumer<Coroutine> m_impl;
  private Runnable m_onCancel = () -> {};
  private String m_name;
  private int m_priority = Command.DEFAULT_PRIORITY;
  private BooleanSupplier m_endCondition;

  private Command m_builtCommand;

  // Using internal anonymous classes to implement the staged builder APIs, but all backed by the
  // state of the enclosing StagedCommandBuilder object

  private final HasRequirementsCommandBuilder m_requirementsView =
      new HasRequirementsCommandBuilder() {
        @Override
        public HasRequirementsCommandBuilder withPriority(int priority) {
          throwIfAlreadyBuilt();

          m_priority = priority;
          return this;
        }

        @Override
        public HasRequirementsCommandBuilder requiring(Mechanism requirement) {
          throwIfAlreadyBuilt();

          requireNonNullParam(requirement, "requirement", "StagedCommandBuilder.requiring");
          m_requirements.add(requirement);
          return this;
        }

        @Override
        public HasRequirementsCommandBuilder requiring(Mechanism requirement, Mechanism... extra) {
          throwIfAlreadyBuilt();

          requireNonNullParam(requirement, "requirement", "StagedCommandBuilder.requiring");
          requireNonNullParam(extra, "extra", "StagedCommandBuilder.requiring");

          for (int i = 0; i < extra.length; i++) {
            requireNonNullParam(extra[i], "extra[" + i + "]", "StagedCommandBuilder.requiring");
          }

          m_requirements.add(requirement);
          m_requirements.addAll(Arrays.asList(extra));
          return this;
        }

        @Override
        public HasRequirementsCommandBuilder requiring(Collection<Mechanism> requirements) {
          throwIfAlreadyBuilt();

          requireNonNullParam(requirements, "requirements", "StagedCommandBuilder.requiring");
          m_requirements.addAll(requirements);
          return this;
        }

        @Override
        public HasExecutionCommandBuilder executing(Consumer<Coroutine> impl) {
          throwIfAlreadyBuilt();

          requireNonNullParam(impl, "impl", "StagedCommandBuilder.executing");
          m_impl = impl;
          return m_executionView;
        }

        @Override
        public HasRequirementsCommandBuilder whenCanceled(Runnable onCancel) {
          throwIfAlreadyBuilt();

          requireNonNullParam(onCancel, "onCancel", "StagedCommandBuilder.whenCanceled");
          m_onCancel = onCancel;
          return this;
        }
      };

  private final HasExecutionCommandBuilder m_executionView =
      new HasExecutionCommandBuilder() {
        @Override
        public HasExecutionCommandBuilder whenCanceled(Runnable onCancel) {
          throwIfAlreadyBuilt();

          requireNonNullParam(onCancel, "onCancel", "StagedCommandBuilder.whenCanceled");
          m_onCancel = onCancel;
          return this;
        }

        @Override
        public HasExecutionCommandBuilder withPriority(int priority) {
          throwIfAlreadyBuilt();

          m_priority = priority;
          return this;
        }

        @Override
        public HasExecutionCommandBuilder until(BooleanSupplier endCondition) {
          throwIfAlreadyBuilt();

          m_endCondition = endCondition; // allowed to be null
          return this;
        }

        @Override
        public TerminalCommandBuilder withName(String name) {
          throwIfAlreadyBuilt();

          requireNonNullParam(name, "name", "StagedCommandBuilder.withName");
          m_name = name;
          return m_terminalView;
        }
      };

  private final TerminalCommandBuilder m_terminalView =
      new TerminalCommandBuilder() {
        @Override
        public TerminalCommandBuilder whenCanceled(Runnable onCancel) {
          throwIfAlreadyBuilt();

          requireNonNullParam(onCancel, "onCancel", "StagedCommandBuilder.whenCanceled");
          m_onCancel = onCancel;
          return this;
        }

        @Override
        public TerminalCommandBuilder withPriority(int priority) {
          throwIfAlreadyBuilt();

          m_priority = priority;
          return this;
        }

        @Override
        public Command make() {
          throwIfAlreadyBuilt();

          var command = new BuilderBackedCommand(StagedCommandBuilder.this);

          if (m_endCondition == null) {
            // No custom end condition, just return the raw command
            m_builtCommand = command;
          } else {
            // A custom end condition is implemented as a race group, since we cannot modify the
            // command
            // body to inject the end condition.
            m_builtCommand =
                new ParallelGroupBuilder().requiring(command).until(m_endCondition).named(m_name);
          }

          return m_builtCommand;
        }
      };

  private static final class BuilderBackedCommand implements Command {
    private final StagedCommandBuilder m_builder;

    private BuilderBackedCommand(StagedCommandBuilder builder) {
      m_builder = builder;
    }

    @Override
    public void run(Coroutine coroutine) {
      m_builder.m_impl.accept(coroutine);
    }

    @Override
    public void onCancel() {
      m_builder.m_onCancel.run();
    }

    @Override
    public String name() {
      return m_builder.m_name;
    }

    @Override
    public Set<Mechanism> requirements() {
      return m_builder.m_requirements;
    }

    @Override
    public int priority() {
      return m_builder.m_priority;
    }

    @Override
    public String toString() {
      return name();
    }
  }

  @Override
  public HasRequirementsCommandBuilder noRequirements() {
    throwIfAlreadyBuilt();

    return m_requirementsView;
  }

  @Override
  public HasRequirementsCommandBuilder requiring(Mechanism requirement, Mechanism... extra) {
    throwIfAlreadyBuilt();

    requireNonNullParam(requirement, "requirement", "StagedCommandBuilder.requiring");
    requireNonNullParam(extra, "extra", "StagedCommandBuilder.requiring");

    for (int i = 0; i < extra.length; i++) {
      requireNonNullParam(extra[i], "extra[" + i + "]", "StagedCommandBuilder.requiring");
    }

    m_requirements.add(requirement);
    m_requirements.addAll(Arrays.asList(extra));
    return m_requirementsView;
  }

  @Override
  public HasRequirementsCommandBuilder requiring(Collection<Mechanism> requirements) {
    throwIfAlreadyBuilt();

    requireNonNullParam(requirements, "requirements", "StagedCommandBuilder.requiring");
    int i = 0;
    for (var mechanism : requirements) {
      requireNonNullParam(mechanism, "requirements[" + i + "]", "StagedCommandBuilder.requiring");
      i++;
    }

    m_requirements.addAll(requirements);
    return m_requirementsView;
  }

  @Override
  public StartingCommandBuilder withPriority(int priority) {
    throwIfAlreadyBuilt();

    m_priority = priority;
    return this;
  }

  // Prevent builders from being mutated after command creation
  // Weird things could happen like changing requirements, priority level, or even the command
  // implementation itself if we didn't prohibit it.
  private void throwIfAlreadyBuilt() {
    if (m_builtCommand != null) {
      throw new IllegalStateException("Command builders cannot be reused");
    }
  }
}
