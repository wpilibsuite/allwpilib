package edu.wpi.first.wpilibj.examples.commandbasedbestpracticeled.subsystems;

import edu.wpi.first.wpilibj.Timer;
import edu.wpi.first.wpilibj.examples.commandbasedbestpracticeled.subsystems.RobotSignals.LEDView;
import edu.wpi.first.wpilibj.LEDPattern;
import edu.wpi.first.wpilibj.util.Color;
import edu.wpi.first.wpilibj2.command.Command;
import edu.wpi.first.wpilibj2.command.FunctionalCommand;
import edu.wpi.first.wpilibj2.command.SubsystemBase;
import edu.wpi.first.wpilibj2.command.button.Trigger;

/**
 * Demonstration of a Moore-Like FSM example that is similar to composing sequential and parallel
 * command groups. Triggers are used to help control state selection instead of other commands and
 * decorators.
 * 
 * This FSM example sequentially displays eight red LEDs first to last then back last to first
 *   1 -> 2 -> 3 -> 4 -> 5 -> 6 -> 7 -> 8 -> 7 -> 6 -> 5 -> 4 -> 3 -> 2 -> 1 -> 2 ...
 * 
 * The triggers are 1/10 second clock divided into 14 bins for 14 triggers needed for this example
 * of the Knight Rider Kitt Scanner.
 */
public class MooreLikeFSM extends SubsystemBase {

  private LEDView m_robotSignals; // LED view where the output is displayed

  /**
   * Eight state FSM for the eight lights in the Knight Rider Kitt Scanner
   */ 
  private enum State
    {MyState1, MyState2, MyState3, MyState4, MyState5, MyState6, MyState7, MyState8};

  // Since triggering is based on the clock the initial state is not fully relevant
  // because we have to wait for the clock to roll around to the right time and then it
  // triggers the state following the initial state. Too simple to work exactly as you might
  // expect but it's set so fast you might not notice it's not "right".
  private State initialState = State.MyState1;
  private State currentState = initialState;

  /**
   * These commands (factories) define the states.
   * 
   * <p>They can't be put into the State enum because
   * enums are static and these commands in general are non-static especially with the automatic
   * "this" subsystem requirement.
   * 
   * <p>Generally Command factories can be "public" but these are dedicated to this FSM and there is
   * no intention of allowing outside use of them as that can disrupt the proper function of the
   * FSM.
   * 
   * <p>The states have to record their "currentState" for use in the transition since there is no
   * other good way to get automatically the current state.
   * 
   * @return Command that records its state name and turns on the correct LED
   */

  private final Command scanner1() {
    return
      defineState(State.MyState1,
        ()->{
        LEDPattern currentStateSignal = oneLEDSmeared(0, Color.kRed, Color.kBlack);
        m_robotSignals.setSignal(currentStateSignal).schedule();
      });
  }

  private final Command scanner2() {
    return
      defineState(State.MyState2,
        ()->{
        LEDPattern currentStateSignal = oneLEDSmeared(1, Color.kRed, Color.kBlack);
        m_robotSignals.setSignal(currentStateSignal).schedule();
      });
  }

  private final Command scanner3() {
    return
      defineState(State.MyState3,      
        ()->{
        LEDPattern currentStateSignal = oneLEDSmeared(2, Color.kRed, Color.kBlack);
        m_robotSignals.setSignal(currentStateSignal).schedule();
      });
  }

  private final Command scanner4() {
    return
      defineState(State.MyState4,
        ()->{
        LEDPattern currentStateSignal = oneLEDSmeared(3, Color.kRed, Color.kBlack);
        m_robotSignals.setSignal(currentStateSignal).schedule();
      });
  }

  private final Command scanner5() {
    return
      defineState(State.MyState5,
        ()->{
        LEDPattern currentStateSignal = oneLEDSmeared(4, Color.kRed, Color.kBlack);
        m_robotSignals.setSignal(currentStateSignal).schedule();
      });
  }

  private final Command scanner6() {
    return
      defineState(State.MyState6,
        ()->{
        LEDPattern currentStateSignal = oneLEDSmeared(5, Color.kRed, Color.kBlack);
        m_robotSignals.setSignal(currentStateSignal).schedule();
      });
    }

  private final Command scanner7() {
    return
      defineState(State.MyState7,
        ()->{
        LEDPattern currentStateSignal = oneLEDSmeared(6, Color.kRed, Color.kBlack);
        m_robotSignals.setSignal(currentStateSignal).schedule();
      });
  }

  private final Command scanner8() {
    return
      defineState(State.MyState8,
        ()->{
        LEDPattern currentStateSignal = oneLEDSmeared(7, Color.kRed, Color.kBlack);
        m_robotSignals.setSignal(currentStateSignal).schedule();
      });
  }

  /**
   * Transitions for this FSM are defined as a Trigger that checks the current state value AND the
   * clock value to trigger the next-state-defining Command.
   * 
   * Trigger is created here with the current_state and triggering event. The next_state command
   * is added later.
   * 
   * Generally Triggers can be "public" but these are dedicated to this FSM and there is no
   * intention of allowing outside use of them as that can disrupt the proper function of the FSM.
   * 
   * There may be potential optimizations. The triggers for this FSM could be in their own
   * EventLoop and polled only when the subsystem is activated. If an event is unique to a
   * transition the check for the current state would not be necessary.
   */

  private final Trigger t1 = new Trigger(
      ()-> currentState == State.MyState1 && (int) (Timer.getFPGATimestamp()*10.0 % 14.0) == 0);

  private final Trigger t2 = new Trigger(
      ()-> currentState == State.MyState2 && (int) (Timer.getFPGATimestamp()*10.0 % 14.0) == 1);

  private final Trigger t3 = new Trigger(
      ()-> currentState == State.MyState3 && (int) (Timer.getFPGATimestamp()*10.0 % 14.0) == 2);

  private final Trigger t4 = new Trigger(
      ()-> currentState == State.MyState4 && (int) (Timer.getFPGATimestamp()*10.0 % 14.0) == 3);

  private final Trigger t5 = new Trigger(
      ()-> currentState == State.MyState5 && (int) (Timer.getFPGATimestamp()*10.0 % 14.0) == 4);

  private final Trigger t6 = new Trigger(
      ()-> currentState == State.MyState6 && (int) (Timer.getFPGATimestamp()*10.0 % 14.0) == 5);

  private final Trigger t7 = new Trigger(
      ()-> currentState == State.MyState7 && (int) (Timer.getFPGATimestamp()*10.0 % 14.0) == 6);

  private final Trigger t8 = new Trigger(
      ()-> currentState == State.MyState8 && (int) (Timer.getFPGATimestamp()*10.0 % 14.0) == 7);

  private final Trigger t9 = new Trigger(
      ()-> currentState == State.MyState7 && (int) (Timer.getFPGATimestamp()*10.0 % 14.0) == 8);

  private final Trigger t10 = new Trigger(
      ()-> currentState == State.MyState6 && (int) (Timer.getFPGATimestamp()*10.0 % 14.0) == 9);

  private final Trigger t11 = new Trigger(
      ()-> currentState == State.MyState5 && (int) (Timer.getFPGATimestamp()*10.0 % 14.0) == 10);

  private final Trigger t12 = new Trigger(
      ()-> currentState == State.MyState4 && (int) (Timer.getFPGATimestamp()*10.0 % 14.0) == 11);

  private final Trigger t13 = new Trigger(
      ()-> currentState == State.MyState3 && (int) (Timer.getFPGATimestamp()*10.0 % 14.0) == 12);

  private final Trigger t14 = new Trigger(
      ()-> currentState == State.MyState2 && (int) (Timer.getFPGATimestamp()*10.0 % 14.0) == 13);

  /**
   * Activate all Triggers
   * 
   * Trigger has current state and triggering event from above. Add the next_state to it and that
   * activates the trigger in its event loop.
   * 
   * The transition is now completely defined as current_state + event => next_state
   */
  private final void bindTriggers() {
    t1.onTrue(scanner2());
    t2.onTrue(scanner3());
    t3.onTrue(scanner4());
    t4.onTrue(scanner5());
    t5.onTrue(scanner6());
    t6.onTrue(scanner7());
    t7.onTrue(scanner8());
    t8.onTrue(scanner7());
    t9.onTrue(scanner6());
    t10.onTrue(scanner5());
    t11.onTrue(scanner4());
    t12.onTrue(scanner3());
    t13.onTrue(scanner2());
    t14.onTrue(scanner1());
  }

  /**
   * CTOR
   * 
   */
  public MooreLikeFSM(LEDView robotSignals) {
    this.m_robotSignals = robotSignals;
    bindTriggers();
  }

  /**
   * Constructs a command that sets the current state and runs an action every iteration until interrupted.
   *
   * @param state State associated with the command as the current state
   * @param run the action to run every iteration
   * @return the command to run that defines the state
   */
  private final Command defineState(State state, Runnable run) {
    return new FunctionalCommand(
        () -> currentState = state, run, interrupted -> {}, () -> false, this);
  }

  /**
   * Turn on one bright LED in the string view.
   * Turn on its neighbors dimly. It appears smeared.
   * 
   * A simple cheat of the real Knight Rider Kitt Scanner which has a slowly
   * diminishing comet tail.  https://www.youtube.com/watch?v=usui7ECHPNQ
   * 
   * @param index which LED to turn on
   * @param colorForeground color of the on LED
   * @param colorBackground color of the off LEDs
   * @return Pattern to apply to the LED view
   */
  private static final LEDPattern oneLEDSmeared(int index, Color colorForeground, Color colorBackground) {
    return (reader, writer) -> {
      int bufLen = reader.getLength();
      final int slightlyDim = 180;
      final int dim = 120;
      for (int led = 0; led < bufLen; led++) {
        if(led == index) {
          writer.setLED(led, colorForeground);              
        } else if((led == index-2 && index-2 >= 0) || (led == index+2 && index+2 < bufLen)) {
          writer.setRGB(led,
           (int) (colorForeground.red * dim),
           (int) (colorForeground.green * dim),
           (int) (colorForeground.blue * dim));
        } else if((led == index-1 && index-1 >= 0) || (led == index+1 && index+1 < bufLen)) {
          writer.setRGB(led,
           (int) (colorForeground.red * slightlyDim),
           (int) (colorForeground.green * slightlyDim),
           (int) (colorForeground.blue * slightlyDim));
        } else {
          writer.setLED(led, colorBackground);              
        }
      }
    };
  }

  /**
   * Default Command could cause havoc with the FSM - it depends; be careful
   * 
   * @param def Command will be ignored
   * @throws IllegalArgumentException immediately to prevent attempt to use
   */
  @Override
  public void setDefaultCommand(Command def) {
    throw new IllegalArgumentException("Default Command not allowed");
  }

  // Unnecessary before and after stubs for this example.
  // Consider putting security on them if they are actually used.
  // (See AchieveHueGoal for example implementation of security.)

  /**
   * Run before commands and triggers
   */
  public void runBeforeCommands() {}

  /**
   * Run after commands and triggers
   */
  public void runAfterCommands() {}
}
