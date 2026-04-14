// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.command3;

import static org.wpilib.util.ErrorMessages.requireNonNullParam;

import java.util.ArrayList;
import java.util.List;
import java.util.Set;
import java.util.function.BooleanSupplier;
import java.util.function.Supplier;
import org.wpilib.annotation.NoDiscard;
import org.wpilib.annotation.PostConstructionInitializer;

/**
 * A declarative state machine that can be used to implement complex command routines. State machine
 * setup should be done in stages: first, a state machine is created and its name is set; second,
 * states are added to the state machine using {@link #addState(Command)}; third, transitions
 * between states can be specified using {@link State#switchTo(State)}:
 *
 * <pre>{@code
 * // Declare the state machine
 * StateMachine stateMachine = new StateMachine("Example State Machine");
 *
 * // Declare states
 * State state1 = stateMachine.addState(...);
 * State state2 = stateMachine.addState(...);
 * State state3 = stateMachine.addState(...);
 *
 * // Set initial state
 * stateMachine.setInitialState(state1);
 *
 * // Declare transitions
 * state1.switchTo(state2).when(...);
 * state2.switchTo(state3).when(...);
 * }</pre>
 *
 * <p>Every state in a state machine runs a single command. While a state's command is running, the
 * state machine will continually check all transitions that can be triggered from that state. If a
 * transition is triggered, the state machine will cancel the state's command and move to the next
 * state as defined by that transition. If no transition is triggered by the time the command
 * completes, the state machine will exit unless a {@link
 * TransitionNeedsConditionStage#whenComplete()} transition was specified from that state:
 *
 * <pre>{@code
 * // switch from state1 to state2 when foo is true
 * state1.switchTo(state2).when(() -> foo == true);
 *
 * // but if foo never becomes true, switch to state3 when state1 finishes
 * state1.switchTo(state3).whenComplete();
 *
 * // no transitions are defined from state2 or state3,
 * // so the state machine will exit when either state completes
 * }</pre>
 */
public final class StateMachine implements Command {
  private final String m_name;
  private State m_initialState = null;
  private final List<State> m_states = new ArrayList<>();

  /**
   * Creates a new state machine.
   *
   * @param name The name of the state machine. Cannot be null. This will appear in telemetry as the
   *     {@link Command#name() name} of the state machine.
   */
  public StateMachine(String name) {
    requireNonNullParam(name, "name", "StateMachine");
    m_name = name;
  }

  @Override
  public String name() {
    return m_name;
  }

  @Override
  public Set<Mechanism> requirements() {
    // The machine itself doesn't have any requirements. Commands bound to the various states that
    // the machine moves through may have requirements, however.
    return Set.of();
  }

  /**
   * Adds a new state to the state machine. State transitions can be specified on the new state
   * using {@link State#switchTo(State)}.
   *
   * @param command The command for the state to execute. Cannot be null.
   * @return The newly created state.
   */
  @NoDiscard
  public State addState(Command command) {
    requireNonNullParam(command, "command", "StateMachine.addState");
    var state = new State(this, command);
    m_states.add(state);
    return state;
  }

  /**
   * Sets up a transition from any of the given states to a specific state. If no states are given,
   * the transition will apply to all states in the state machine <i>at the time this method is
   * called</i>.
   *
   * <pre>{@code
   * stateMachine.switchFromAny(state1, state2, state3).to(state4).when(() -> foo == true);
   *
   * // Functionally equivalent to:
   * state1.switchTo(state4).when(() -> foo == true);
   * state2.switchTo(state4).when(() -> foo == true);
   * state3.switchTo(state4).when(() -> foo == true);
   *
   * // Set up an early exit condition from any state
   * stateMachine.switchFromAny().toExitStateMachine().when(() -> bar == true);
   *
   * // Functionally equivalent to:
   * state1.exitStateMachine().when(() -> bar == true);
   * state2.exitStateMachine().when(() -> bar == true);
   * state3.exitStateMachine().when(() -> bar == true);
   * state4.exitStateMachine().when(() -> bar == true);
   * }</pre>
   *
   * @param states The states to transition from.
   * @return A builder for the transition.
   */
  public TransitionNeedsTargetStage switchFromAny(State... states) {
    if (states.length == 0) {
      return new TransitionNeedsTargetStage(List.copyOf(m_states));
    } else {
      return new TransitionNeedsTargetStage(List.of(states));
    }
  }

  /**
   * Sets the initial state for the state machine. This must be called before the state machine is
   * scheduled. Failure to do so will result in an {@link IllegalStateException} being thrown when
   * the state machine is started. Usage of this method is enforced by the WPILib compiler plugin;
   * creating a state machine and neglecting to call this method will result in a compilation error.
   *
   * @param initialState The new initial state. Cannot be null.
   * @see PostConstructionInitializer
   */
  @PostConstructionInitializer
  public void setInitialState(State initialState) {
    requireNonNullParam(initialState, "initialState", "StateMachine.setInitialState");
    if (!this.equals(initialState.m_stateMachine)) {
      throw new IllegalArgumentException("Cannot set initial state in a different state machine");
    }
    m_initialState = initialState;
  }

  @Override
  public void run(Coroutine coroutine) {
    if (m_initialState == null) {
      throw new IllegalStateException(
          m_name + " does not have an initial state. Use .setInitialState() to provide one.");
    }

    var currentState = m_initialState;

    outer_loop:
    while (currentState != null) {
      final var currentCommand = currentState.command();
      coroutine.fork(currentCommand);
      currentState.runEnterCallbacks();
      boolean didYield = false;

      while (coroutine.scheduler().isRunning(currentCommand)) {
        for (var transition : currentState.transitions()) {
          if (transition.shouldTransition()) {
            // Cancel the current state's command and move to the next state specified by the
            // transition. Break the state loop early to avoid an unnecessary yield() call and
            // allow the next state's command to start in the same loop iteration that the
            // previous state completed. If the next state is null, the state machine will exit
            // immediately.
            // Note: to prevent infinite loops when states transition to themselves, we require
            // the transition signal to be a rising edge on the user-supplied condition to ensure
            // that the transition is only triggered once per loop iteration.
            currentState.runExitCallbacks();
            coroutine.scheduler().cancel(currentCommand);
            currentState = verifyState(transition.nextState());
            continue outer_loop;
          }
        }

        // Yield after checking all transitions.
        // Note: this will be skipped if a transition is triggered.
        coroutine.yield();
        didYield = true;
      }

      // Move to the next configured state if no transition was hit before the command completed.
      // We need to be careful about states with oneshot commands; they will complete immediately
      // in the `fork()` call above and never enter the `while` loop and thus never yield.
      // Therefore, we inject a yield call at the end here to ensure that the state machine will
      // always yield once per state. This has a downside of adding extra loop cycles to states that
      // may not need them (and has slightly different behavior to SequentialCommandGroup, which
      // runs commands as fast as possible).
      currentState.runExitCallbacks();
      currentState = verifyState(currentState.nextState());
      if (!didYield && currentState != null) {
        // No need to yield if we're exiting the state machine
        coroutine.yield();
      }
    }
  }

  private State verifyState(State next) {
    if (next == null || this.equals(next.m_stateMachine)) {
      // OK
      return next;
    }

    // Bad user setup
    throw new IllegalStateException(
        "The next state does not belong to this state machine. Check the state for "
            + next.command().name());
  }

  /**
   * A state in a state machine. Each state has a command that will be run when it is active. States
   * can transition to other states when some condition is met when that state is active, or
   * automatically transition to another state when it completes if no transition conditions were
   * met. A state with no transitions will never transition to another state, and will cause the
   * state machine to exit when the state completes; likewise, a state with no incoming transitions
   * will never be active.
   */
  public static final class State {
    /** The state machine that this state belongs to. */
    private final StateMachine m_stateMachine;

    /** The command that will run when this state is active. */
    private final Command m_command;

    /** The possible states to transition to when this state completes. */
    private final List<Completion> m_completions = new ArrayList<>();

    /** The state to transition to by default when this state completes. May be null. */
    private Supplier<State> m_defaultNextState = () -> null;

    /**
     * The transitions that can be triggered from this state. If multiple transitions are triggered
     * at once, the first transition in the list will be used.
     */
    private final List<Transition> m_transitions = new ArrayList<>();

    private final List<Runnable> m_enterCallbacks = new ArrayList<>();
    private final List<Runnable> m_exitCallbacks = new ArrayList<>();

    private State(StateMachine stateMachine, Command command) {
      m_stateMachine = stateMachine;
      m_command = command;
    }

    private Command command() {
      return m_command;
    }

    private List<Transition> transitions() {
      return m_transitions;
    }

    private void addTransition(Transition transition) {
      m_transitions.add(transition);
    }

    /**
     * Sets the next state to transition to when this state completes without having fired a
     * transition first, or if no conditional completion transition has been met.
     *
     * @param nextState A supplier for the next state to transition to. Cannot be null, but may
     *     return null.
     */
    private void setNextState(Supplier<State> nextState) {
      m_defaultNextState = nextState;
    }

    // Custom boolean supplier classes may override .equals to do boolean value comparisons,
    // particularly in Kotlin code. Check reference equality instead to just remove bindings to
    // the same condition object.
    @SuppressWarnings("PMD.CompareObjectsWithEquals")
    private void addCompletion(BooleanSupplier condition, Supplier<State> next) {
      // Remove any preexisting completion with the same condition
      m_completions.removeIf(c -> c.getCondition() == condition);
      m_completions.add(new Completion(next, condition));
    }

    private State nextState() {
      for (var completion : m_completions) {
        if (completion.shouldTransition()) {
          return completion.nextState();
        }
      }

      // No conditional transition has been met, use the default next state.
      // If this was never set or was set to be null, the state machine will exit.
      return m_defaultNextState.get();
    }

    private void runEnterCallbacks() {
      m_enterCallbacks.forEach(Runnable::run);
    }

    private void runExitCallbacks() {
      m_exitCallbacks.forEach(Runnable::run);
    }

    /**
     * Adds a function to be called when this state is entered. Callbacks are invoked immediately
     * after the state's command is scheduled, and are run in the same order they were added.
     *
     * <p>Note: if a callback schedules any commands, those commands will be scoped to the lifetime
     * of the entire state machine, <i>not</i> this state's lifetime.
     *
     * @param callback The callback to run. Cannot be null.
     */
    public void onEnter(Runnable callback) {
      requireNonNullParam(callback, "callback", "State.onEnter");
      m_enterCallbacks.add(callback);
    }

    /**
     * Adds a function to be called when this state is exited. Callbacks are invoked immediately
     * before the state's command is canceled, and are run in the order they were added. If the
     * command finishes naturally, the callbacks are run immediately after it completes and before
     * the next state is entered.
     *
     * @param callback The callback to run. Cannot be null.
     */
    public void onExit(Runnable callback) {
      requireNonNullParam(callback, "callback", "State.onExit");
      m_exitCallbacks.add(callback);
    }

    /**
     * Starts building a transition to the specified state.
     *
     * @param to The state to transition to. Cannot be null.
     * @return A builder for the transition.
     */
    public TransitionNeedsConditionStage switchTo(State to) {
      requireNonNullParam(to, "to", "State.switchTo");
      if (!m_stateMachine.equals(to.m_stateMachine)) {
        throw new IllegalArgumentException(
            "Cannot transition to a state in a different state machine");
      }
      return new TransitionNeedsTargetStage(List.of(this)).to(to);
    }

    /**
     * Starts build a transition to some dynamic state. The supplier will be evaluated at the time
     * the transition's condition is met.
     *
     * @param dynamic The dynamic state supplier. Cannot be null.
     * @return A builder for the transition.
     */
    public TransitionNeedsConditionStage switchTo(Supplier<State> dynamic) {
      requireNonNullParam(dynamic, "dynamic", "State.switchTo");
      // Unfortunately, we can't check up front that the supplier will always return a state for
      // this state machine. The output will need to be checked when the supplier is called
      return new TransitionNeedsTargetStage(List.of(this)).to(dynamic);
    }

    /**
     * Starts building a transition that will exit the state machine when triggered, rather than
     * moving to a different state.
     *
     * @return A builder for the transition.
     */
    public TransitionNeedsConditionStage exitStateMachine() {
      return new TransitionNeedsConditionStage(List.of(this), () -> null);
    }
  }

  /**
   * A builder for a transition from one state to another. Use {@link #to(State)} to specify the
   * target state to transition to.
   */
  @NoDiscard("Use .to() to specify the target state")
  public static final class TransitionNeedsTargetStage {
    private final List<State> m_from;

    private TransitionNeedsTargetStage(List<State> from) {
      m_from = from;
    }

    /**
     * Specifies the target state to transition to.
     *
     * @param to The state to transition to. Cannot be null.
     * @return A builder to specify the transition condition.
     */
    public TransitionNeedsConditionStage to(State to) {
      requireNonNullParam(to, "to", "NeedsTargetTransitionBuilder.to");
      for (var state : m_from) {
        if (!state.m_stateMachine.equals(to.m_stateMachine)) {
          throw new IllegalArgumentException(
              "Cannot transition to a state in a different state machine");
        }
      }
      return new TransitionNeedsConditionStage(m_from, () -> to);
    }

    /**
     * Specifies a dynamic target state to transition to. The supplier will be evaluated at the time
     * the transition condition is met.
     *
     * @param dynamic A dynamic supplier for next states. Cannot be null.
     * @return A builder to specify the transition condition.
     */
    public TransitionNeedsConditionStage to(Supplier<State> dynamic) {
      requireNonNullParam(dynamic, "dynamic", "NeedsTargetTransitionBuilder.to");
      return new TransitionNeedsConditionStage(m_from, dynamic);
    }

    /**
     * Specifies the transition will exit the state machine when triggered, rather than moving to a
     * different state.
     *
     * @return A builder to specify the transition condition.
     */
    public TransitionNeedsConditionStage toExitStateMachine() {
      return new TransitionNeedsConditionStage(m_from, () -> null);
    }
  }

  /**
   * A builder to set conditions for a transition from one state to another. Use {@link
   * #when(BooleanSupplier)} to make the transition occur when some external condition becomes true,
   * or use {@link #whenComplete()} to make the transition occur when the originating state
   * completes without having reached any other transitions first.
   */
  @NoDiscard("Use .when() or .whenComplete() to specify the transition condition")
  public static final class TransitionNeedsConditionStage {
    private final List<State> m_originatingStates;

    // Note: A null result from the supplier indicates that the transition will cause the state
    //       machine to exit
    private final Supplier<State> m_targetStateSupplier;

    private TransitionNeedsConditionStage(List<State> from, Supplier<State> to) {
      requireNonNullParam(from, "from", "TransitionNeedsConditionStage");
      requireNonNullParam(to, "to", "TransitionNeedsConditionStage");

      m_originatingStates = from;
      m_targetStateSupplier = to;
    }

    /**
     * Adds a transition that will be triggered when the specified condition becomes true.
     *
     * <p><strong>NOTE: this had no effect if the originating state is a one-shot command without a
     * yield.</strong> Use {@link #whenComplete()} instead for transitions from one-shot commands.
     *
     * <p>If multiple transitions are triggered in the same scheduler loop iteration, the first
     * transition will fire and the rest will be ignored.
     *
     * <pre>{@code
     * StateMachine stateMachine = new StateMachine("Example State Machine");
     * State state1 = stateMachine.addState(...);
     * State state2 = stateMachine.addState(...);
     * State state3 = stateMachine.addState(...);
     *
     * state1.switchTo(state2).when(() -> foo == true);
     *
     * // never triggers because the first transition will be evaluated first
     * state1.switchTo(state3).when(() -> foo == true);
     * }</pre>
     *
     * @param condition The condition that will trigger the transition. Cannot be null.
     */
    public void when(BooleanSupplier condition) {
      requireNonNullParam(condition, "condition", "NeedsConditionTransitionBuilder.when");
      var transition = new Transition(m_targetStateSupplier, condition);
      m_originatingStates.forEach(originatingState -> originatingState.addTransition(transition));
    }

    /**
     * Adds a transition to the target state when the originating state completes without having
     * triggered any other transitions first. If this is called multiple times for the same
     * originating state, later calls will override the previous transitions. Any {@link
     * #whenCompleteAnd} transitions will take precedence over {@code whenComplete} transitions if
     * their conditions are met when the state exits.
     *
     * <pre>{@code
     * StateMachine stateMachine = new StateMachine("Example State Machine");
     * State state1 = stateMachine.addState(...);
     * State state2 = stateMachine.addState(...);
     * State state3 = stateMachine.addState(...);
     *
     * state1.switchTo(state2).whenComplete();
     * state1.switchTo(state3).whenComplete(); // Overrides the previous transition
     * state1.exitStateMachine().whenCompleteAnd(...); // Takes precedence if the condition is met
     * }</pre>
     */
    public void whenComplete() {
      m_originatingStates.forEach(state -> state.setNextState(m_targetStateSupplier));
    }

    /**
     * Similar to {@link #when(BooleanSupplier)}, but only triggers when the originating state
     * completes <i>and</i> some other condition is also met. {@code whenCompleteAnd} transitions
     * will be evaluated in declaration order and take precedence over any {@link #whenComplete()}
     * transitions that have been specified.
     *
     * <pre>{@code
     * StateMachine stateMachine = new StateMachine("Example State Machine");
     * State state1 = stateMachine.addState(...);
     * State state2 = stateMachine.addState(...);
     * State state3 = stateMachine.addState(...);
     *
     * state1.switchTo(state2).whenComplete();
     * state1.switchTo(state3).whenComplete(); // Overrides the previous transition
     * state1.exitStateMachine().whenCompleteAnd(...); // Takes precedence if the condition is met
     * }</pre>
     *
     * @param condition The condition that will trigger the transition.
     */
    public void whenCompleteAnd(BooleanSupplier condition) {
      requireNonNullParam(condition, "condition", "NeedsConditionTransitionBuilder.whenComplete");
      m_originatingStates.forEach(state -> state.addCompletion(condition, m_targetStateSupplier));
    }
  }

  /**
   * Similar to {@link Transition}, but does not track the state of the condition. This is intended
   * to only be checked once, when the originating state completes.
   */
  private static final class Completion {
    private final Supplier<State> m_nextSupplier;
    private final BooleanSupplier m_condition;

    /**
     * Creates a new completion object.
     *
     * @param next A supplier for the state to transition to when the originating state completes.
     * @param condition The condition that will trigger the transition.
     */
    private Completion(Supplier<State> next, BooleanSupplier condition) {
      m_nextSupplier = next;
      m_condition = condition;
    }

    private boolean shouldTransition() {
      return m_condition.getAsBoolean();
    }

    public State nextState() {
      return m_nextSupplier.get();
    }

    public BooleanSupplier getCondition() {
      return m_condition;
    }
  }

  /**
   * Similar to {@link Completion}, but tracks the state of the condition to avoid infinite loops.
   * This is intended to be checked every loop while the originating state is active.
   */
  private static final class Transition {
    /** The state to transition to. */
    private final Supplier<State> m_nextSupplier;

    /** The condition that will trigger the transition. */
    private final BooleanSupplier m_condition;

    private boolean m_previousSignal = false;

    private Transition(Supplier<State> next, BooleanSupplier condition) {
      m_nextSupplier = next;
      m_condition = condition;
    }

    /** Checks if the transition should be triggered. */
    private boolean shouldTransition() {
      // Wrap the condition in a rising edge detector so that it will only trigger a single time per
      // loop iteration. This prevents issues with a state transitioning to itself like so:
      // state1.switchTo(state1).when(() -> foo == true);
      // If the condition is itself a rising edge detector, this wrapping is redundant but harmless.
      boolean currentValue = m_condition.getAsBoolean();
      boolean isRisingEdge = currentValue && !m_previousSignal;
      m_previousSignal = currentValue;
      return isRisingEdge;
    }

    private State nextState() {
      return m_nextSupplier.get();
    }
  }
}
