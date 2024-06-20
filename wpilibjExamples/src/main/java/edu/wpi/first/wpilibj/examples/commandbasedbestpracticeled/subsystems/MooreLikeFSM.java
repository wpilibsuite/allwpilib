package edu.wpi.first.wpilibj.examples.commandbasedbestpracticeled.subsystems;

import edu.wpi.first.wpilibj.Timer;
import edu.wpi.first.wpilibj.examples.commandbasedbestpracticeled.subsystems.RobotSignals.LEDView;
import edu.wpi.first.wpilibj.LEDPattern;
import edu.wpi.first.wpilibj.util.Color;
import edu.wpi.first.wpilibj2.command.Command;
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
  enum State {MyState1, MyState2, MyState3, MyState4, MyState5, MyState6, MyState7, MyState8};

  // Since triggering is based on the clock the initial state is not fully relevant
  // because we have to wait for the clock to roll around to the right time and then it
  // triggers the state following the initial state. Too simple to work exactly as you might
  // expect but it's set so fast you might not notice it's not "right".
  static State initialState = State.MyState1;
  static State currentState = initialState;

  /**
   * These commands (factories) define the states. They can't be put into the State enum because
   * enums are static and these commands in general are non-static especially with the automatic
   * "this" subsystem requirement.
   * 
   * The states have to record their "currentState" for use in the transition since there is no
   * other good way to get automatically the current state.
   * 
   * @return Command that records its state name and turns on the correct LED
   */

  public final Command scanner1() {
    return
      runOnce(()->currentState = State.MyState1)      
      .andThen(()->{
        LEDPattern currentStateSignal = oneLEDSmeared(0, Color.kRed, Color.kBlack);
        m_robotSignals.setSignal(currentStateSignal).schedule();
      });
  }

  public final Command scanner2() {
    return
      runOnce(()->currentState = State.MyState2)      
      .andThen(()->{
        LEDPattern currentStateSignal = oneLEDSmeared(1, Color.kRed, Color.kBlack);
        m_robotSignals.setSignal(currentStateSignal).schedule();
      });
  }

  public final Command scanner3() {
    return
      runOnce(()->currentState = State.MyState3)      
      .andThen(()->{
        LEDPattern currentStateSignal = oneLEDSmeared(2, Color.kRed, Color.kBlack);
        m_robotSignals.setSignal(currentStateSignal).schedule();
      });
  }

  public final Command scanner4() {
    return
      runOnce(()->currentState = State.MyState4)      
      .andThen(()->{
        LEDPattern currentStateSignal = oneLEDSmeared(3, Color.kRed, Color.kBlack);
        m_robotSignals.setSignal(currentStateSignal).schedule();
      });
  }

  public final Command scanner5() {
    return
      runOnce(()->currentState = State.MyState5)      
      .andThen(()->{
        LEDPattern currentStateSignal = oneLEDSmeared(4, Color.kRed, Color.kBlack);
        m_robotSignals.setSignal(currentStateSignal).schedule();
      });
  }

  public final Command scanner6() {
    return
      runOnce(()->currentState = State.MyState6)      
      .andThen(()->{
        LEDPattern currentStateSignal = oneLEDSmeared(5, Color.kRed, Color.kBlack);
        m_robotSignals.setSignal(currentStateSignal).schedule();
      });
    }

  public final Command scanner7() {
    return
      runOnce(()->currentState = State.MyState7)      
      .andThen(()->{
        LEDPattern currentStateSignal = oneLEDSmeared(6, Color.kRed, Color.kBlack);
        m_robotSignals.setSignal(currentStateSignal).schedule();
      });
  }

  public final Command scanner8() {
    return
      runOnce(()->currentState = State.MyState8)      
      .andThen(()->{
        LEDPattern currentStateSignal = oneLEDSmeared(7, Color.kRed, Color.kBlack);
        m_robotSignals.setSignal(currentStateSignal).schedule();
      });
  }

  /**
   * Transitions are defined as a Trigger that checks the current state value AND the clock value
   * to trigger the state defining Command
   * 
   * There may be potential optimizations. The triggers for this FSM could be in their own
   * EventLoop and polled only when the subsystem is activated. If an event is unique to a
   * transition the check for the current state would not be necessary.
   */

  /**
   * Instantiate and activate all Triggers
   * 
   * current state, trigger, next state
   */
  private final void createTriggers() {
    new Trigger(
      ()-> currentState == State.MyState1 && (int) (Timer.getFPGATimestamp()*10.0 % 14.0) == 0)
        .onTrue(scanner2());

    new Trigger(
      ()-> currentState == State.MyState2 && (int) (Timer.getFPGATimestamp()*10.0 % 14.0) == 1)
        .onTrue(scanner3());

    new Trigger(
      ()-> currentState == State.MyState3 && (int) (Timer.getFPGATimestamp()*10.0 % 14.0) == 2)
        .onTrue(scanner4());

    new Trigger(
      ()-> currentState == State.MyState4 && (int) (Timer.getFPGATimestamp()*10.0 % 14.0) == 3)
        .onTrue(scanner5());

    new Trigger(
      ()-> currentState == State.MyState5 && (int) (Timer.getFPGATimestamp()*10.0 % 14.0) == 4)
        .onTrue(scanner6());

    new Trigger(
      ()-> currentState == State.MyState6 && (int) (Timer.getFPGATimestamp()*10.0 % 14.0) == 5)
        .onTrue(scanner7());

    new Trigger(
      ()-> currentState == State.MyState7 && (int) (Timer.getFPGATimestamp()*10.0 % 14.0) == 6)
        .onTrue(scanner8());

    new Trigger(
      ()-> currentState == State.MyState8 && (int) (Timer.getFPGATimestamp()*10.0 % 14.0) == 7)
        .onTrue(scanner7());

    new Trigger(
      ()-> currentState == State.MyState7 && (int) (Timer.getFPGATimestamp()*10.0 % 14.0) == 8)
        .onTrue(scanner6());

    new Trigger(
      ()-> currentState == State.MyState6 && (int) (Timer.getFPGATimestamp()*10.0 % 14.0) == 9)
        .onTrue(scanner5());

    new Trigger(
      ()-> currentState == State.MyState5 && (int) (Timer.getFPGATimestamp()*10.0 % 14.0) == 10)
        .onTrue(scanner4());

    new Trigger(
      ()-> currentState == State.MyState4 && (int) (Timer.getFPGATimestamp()*10.0 % 14.0) == 11)
        .onTrue(scanner3());

    new Trigger(
      ()-> currentState == State.MyState3 && (int) (Timer.getFPGATimestamp()*10.0 % 14.0) == 12)
        .onTrue(scanner2());

    new Trigger(
      ()-> currentState == State.MyState2 && (int) (Timer.getFPGATimestamp()*10.0 % 14.0) == 13)
        .onTrue(scanner1());
  }

  /**
   * CTOR
   * 
   */
  public MooreLikeFSM(LEDView robotSignals) {
    this.m_robotSignals = robotSignals;
    createTriggers();
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
  static final LEDPattern oneLEDSmeared(int index, Color colorForeground, Color colorBackground) {
    return (reader, writer) -> {
      int bufLen = reader.getLength();
      final int slightlyDim = 180;
      final int dim = 120;
      for (int led = 0; led < bufLen; led++) {
        if(led == index) {
          writer.setLED(led, colorForeground);              
        } else if((led == index-2 && index-2 >= 0) || (led == index+2 && index+2 < bufLen)) {
          writer.setRGB(led, (int) (colorForeground.red * dim), (int) (colorForeground.green * dim), (int) (colorForeground.blue * dim));
        } else if((led == index-1 && index-1 >= 0) || (led == index+1 && index+1 < bufLen)) {
          writer.setRGB(led, (int) (colorForeground.red * slightlyDim), (int) (colorForeground.green * slightlyDim), (int) (colorForeground.blue * slightlyDim));
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

  /**
   * Run before commands and triggers
   */
  public void runBeforeCommands() {}

  /**
   * Run after commands and triggers
   */
  public void runAfterCommands() {}
}
