// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.command3;

import static org.wpilib.util.ErrorMessages.requireNonNullParam;

import java.util.Arrays;
import java.util.Collection;
import java.util.HashSet;
import java.util.Objects;
import java.util.Set;
import java.util.function.BooleanSupplier;
import java.util.function.Consumer;
import org.wpilib.annotation.NoDiscard;

/**
 * A builder class for commands. Command configuration is done in stages, where later stages have
 * different configuration options than earlier stages. Commands may only be created after going
 * through every stage, enforcing the presence of required options. All commands <i>must</i> have a
 * set of requirements (which may be empty), a name, and an implementation. The builder stages are
 * defined such that the final stage that creates a command can only be reached after going through
 * the earlier stages to configure those required options.
 *
 * <p>Every stage is immutable; each configuration method returns a new stage and leaves the one it
 * was called on unchanged. A stage can therefore be safely shared and used as the starting point
 * for several different commands.
 *
 * <p>Example usage:
 *
 * <pre>{@code
 * NeedsExecutionBuilderStage withRequirements =
 *   StagedCommandBuilder.requiring(mechanism1, mechanism2);
 * NeedsNameBuilderStage withExecution = withRequirements.executing(coroutine -> ...);
 * Command exampleCommand = withExecution.named("Example Command");
 * }</pre>
 *
 * <p>Because every method on the builders returns a builder object, these calls can be chained to
 * cut down on verbosity and make the code easier to read. This is the recommended style:
 *
 * <pre>{@code
 * Command exampleCommand =
 *   StagedCommandBuilder
 *     .requiring(mechanism1, mechanism2)
 *     .executing(coroutine -> ...)
 *     .named("Example Command");
 * }</pre>
 *
 * <p>And can be cut down even further by using helper methods provided by the library:
 *
 * <pre>{@code
 * Command exampleCommand =
 *   Command
 *     .requiring(mechanism1, mechanism2)
 *     .executing(coroutine -> ...)
 *     .named("Example Command");
 * }</pre>
 */
@NoDiscard
public final class StagedCommandBuilder {
  private static final Runnable NO_OP = () -> {};

  /**
   * The stage where requirements are specified. Holds only the requirements gathered so far.
   *
   * @param requirements The immutable set of requirements.
   */
  private record ExecutionStage(Set<Mechanism> requirements) implements NeedsExecutionBuilderStage {
    @Override
    public NeedsExecutionBuilderStage requiring(Mechanism requirement, Mechanism... extra) {
      requireNonNullParam(requirement, "requirement", "StagedCommandBuilder.requiring");
      requireNonNullParam(extra, "extra", "StagedCommandBuilder.requiring");

      var all = new HashSet<>(requirements);
      all.add(requirement);
      addAllChecked(all, "extra", Arrays.asList(extra));
      return new ExecutionStage(Set.copyOf(all));
    }

    @Override
    public NeedsExecutionBuilderStage requiring(Collection<Mechanism> additional) {
      requireNonNullParam(additional, "requirements", "StagedCommandBuilder.requiring");

      var all = new HashSet<>(requirements);
      addAllChecked(all, "requirements", additional);
      return new ExecutionStage(Set.copyOf(all));
    }

    @Override
    public NeedsNameBuilderStage executing(Consumer<Coroutine> impl) {
      requireNonNullParam(impl, "impl", "StagedCommandBuilder.executing");

      return new NameStage(requirements, impl, NO_OP, NO_OP, Command.DEFAULT_PRIORITY, null);
    }

    private static void addAllChecked(
        Set<Mechanism> target, String paramName, Iterable<Mechanism> items) {
      int i = 0;
      for (var item : items) {
        requireNonNullParam(item, paramName + "[" + i + "]", "StagedCommandBuilder.requiring");
        target.add(item);
        i++;
      }
    }
  }

  /**
   * The final stage, where the command's implementation is set and optional configuration is
   * available. Callbacks are never null, and the end condition is null if there is none.
   */
  private record NameStage(
      Set<Mechanism> requirements,
      Consumer<Coroutine> impl,
      Runnable onCancel,
      Runnable onExit,
      int priority,
      BooleanSupplier endCondition)
      implements NeedsNameBuilderStage {
    @Override
    public NeedsNameBuilderStage whenCanceled(Runnable onCancel) {
      return new NameStage(
          requirements,
          impl,
          Objects.requireNonNullElse(onCancel, NO_OP),
          onExit,
          priority,
          endCondition);
    }

    @Override
    public NeedsNameBuilderStage whenExited(Runnable onExit) {
      return new NameStage(
          requirements,
          impl,
          onCancel,
          Objects.requireNonNullElse(onExit, NO_OP),
          priority,
          endCondition);
    }

    @Override
    public NeedsNameBuilderStage withPriority(int priority) {
      return new NameStage(requirements, impl, onCancel, onExit, priority, endCondition);
    }

    @Override
    public NeedsNameBuilderStage until(BooleanSupplier endCondition) {
      return new NameStage(requirements, impl, onCancel, onExit, priority, endCondition);
    }

    @Override
    public Command named(String name) {
      requireNonNullParam(name, "name", "StagedCommandBuilder.named");

      var command = new BuilderBackedCommand(name, requirements, impl, onCancel, onExit, priority);

      if (endCondition == null) {
        return command;
      }

      // if there is an end condition we have to return a race group instead of the command
      // directly,
      // because we can't modify the command body to check the end condition
      return new ParallelGroupBuilder().requiring(command).until(endCondition).named(name);
    }
  }

  // not a record because commands themselves need identity
  private static final class BuilderBackedCommand implements Command {
    private final String m_name;
    private final Set<Mechanism> m_requirements;
    private final Consumer<Coroutine> m_impl;
    private final Runnable m_onCancel;
    private final Runnable m_onExit;
    private final int m_priority;

    private BuilderBackedCommand(
        String name,
        Set<Mechanism> requirements,
        Consumer<Coroutine> impl,
        Runnable onCancel,
        Runnable onExit,
        int priority) {
      m_name = name;
      m_requirements = requirements;
      m_impl = impl;
      m_onCancel = onCancel;
      m_onExit = onExit;
      m_priority = priority;
    }

    @Override
    public void run(Coroutine coroutine) {
      m_impl.accept(coroutine);
    }

    @Override
    public void onCancel() {
      m_onCancel.run();
    }

    @Override
    public void onExit() {
      m_onExit.run();
    }

    @Override
    public String name() {
      return m_name;
    }

    @Override
    public Set<Mechanism> requirements() {
      return m_requirements;
    }

    @Override
    public int priority() {
      return m_priority;
    }

    @Override
    public String toString() {
      return name();
    }
  }

  private StagedCommandBuilder() {
    // Utility class
  }

  /**
   * Explicitly marks the command as requiring no mechanisms. Unless overridden later with {@link
   * NeedsExecutionBuilderStage#requiring(Mechanism, Mechanism...)} or a similar method, the built
   * command will not have ownership over any mechanisms when it runs. Use this for commands that
   * don't need to own a mechanism, such as a gyro zeroing command, that does some kind of cleanup
   * task without needing to control something.
   *
   * @return A builder object that can be used to further configure the command.
   */
  public static NeedsExecutionBuilderStage noRequirements() {
    return new ExecutionStage(Set.of());
  }

  /**
   * Marks the command as requiring one or more mechanisms. If only a single mechanism is required,
   * prefer a factory function like {@link Mechanism#run(Consumer)} or similar - it will
   * automatically require the mechanism, instead of it needing to be explicitly specified.
   *
   * @param requirement The first required mechanism. Cannot be null.
   * @param extra Any optional extra required mechanisms. May be empty, but cannot be null or
   *     contain null values.
   * @return A builder object that can be used to further configure the command.
   */
  public static NeedsExecutionBuilderStage requiring(Mechanism requirement, Mechanism... extra) {
    return noRequirements().requiring(requirement, extra);
  }

  /**
   * Marks the command as requiring zero or more mechanisms. If only a single mechanism is required,
   * prefer a factory function like {@link Mechanism#run(Consumer)} or similar - it will
   * automatically require the mechanism, instead of it needing to be explicitly specified.
   *
   * @param requirements A collection of required mechanisms. May be empty, but cannot be null or
   *     contain null values.
   * @return A builder object that can be used to further configure the command.
   */
  public static NeedsExecutionBuilderStage requiring(Collection<Mechanism> requirements) {
    return noRequirements().requiring(requirements);
  }
}
