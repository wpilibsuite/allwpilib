// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.command3.button;

import java.util.Objects;
import org.wpilib.command3.Command;
import org.wpilib.command3.Scheduler;
import org.wpilib.command3.Trigger;
import org.wpilib.driverstation.DriverStation;
import org.wpilib.driverstation.Gamepad;
import org.wpilib.event.EventLoop;

/**
 * A version of {@link Gamepad} with {@link Trigger} factories for command-based.
 *
 * @see Gamepad
 */
@SuppressWarnings("MethodName")
public class CommandGamepad {
  private final CommandGenericHID m_hid;
  private final Gamepad m_gamepad;

  /**
   * Construct an instance of a controller.
   *
   * @param port The port index on the Driver Station that the controller is plugged into.
   */
  public CommandGamepad(int port) {
    this(Scheduler.getDefault(), port);
  }

  /**
   * Construct an instance of a controller. Commands bound to buttons on the controller will be
   * scheduled on the {@link Scheduler#getDefault() default scheduler} using its default event loop.
   *
   * @param gamepad The Gamepad object to use for this controller.
   */
  public CommandGamepad(Gamepad gamepad) {
    this(Scheduler.getDefault(), gamepad);
  }

  /**
   * Construct an instance of a controller.
   *
   * @param scheduler The scheduler that should execute the triggered commands.
   * @param port The port index on the Driver Station that the controller is plugged into.
   */
  public CommandGamepad(Scheduler scheduler, int port) {
    m_hid = CommandGenericHID.getCommandGenericHID(scheduler, port);
    m_gamepad = DriverStation.getGamepad(port);
  }

  /**
   * Construct an instance of a controller. Commands bound to buttons on the controller will be
   * scheduled on the given scheduler using its default event loop.
   *
   * @param scheduler The scheduler that should execute the triggered commands.
   * @param gamepad The Gamepad object to use for this controller.
   */
  public CommandGamepad(Scheduler scheduler, Gamepad gamepad) {
    m_gamepad = Objects.requireNonNull(gamepad, "Provided gamepad cannot be null");
    m_hid = new CommandGenericHID(scheduler, m_gamepad.getHID());
  }

  /**
   * Get the underlying CommandGenericHID object.
   *
   * @return the wrapped CommandGenericHID object
   */
  public CommandGenericHID getHID() {
    return m_hid;
  }

  /**
   * Get the underlying Gamepad object.
   *
   * @return the wrapped Gamepad object
   */
  public Gamepad getGamepad() {
    return m_gamepad;
  }

  /**
   * Constructs an event instance around this button's digital signal.
   *
   * @param button the button index
   * @return an event instance representing the button's digital signal attached to the {@link
   *     Scheduler#getDefaultEventLoop() default scheduler button loop}.
   * @see #button(int, EventLoop)
   */
  public Trigger button(int button) {
    return m_hid.button(button);
  }

  /**
   * Constructs an event instance around this button's digital signal.
   *
   * @param button the button index
   * @param loop the event loop instance to attach the event to.
   * @return an event instance representing the button's digital signal attached to the given loop.
   */
  public Trigger button(int button, EventLoop loop) {
    return m_hid.button(button, loop);
  }

  /**
   * Constructs a Trigger instance around the Face Down button's digital signal.
   *
   * @return a Trigger instance representing the Face Down button's digital signal attached to the
   *     {@link Scheduler#getDefaultEventLoop() default scheduler event loop} on the scheduler
   *     passed to the controller's constructor, or the {@link Scheduler#getDefault default
   *     scheduler} if a scheduler was not explicitly provided.
   * @see #faceDown(EventLoop)
   */
  public Trigger faceDown() {
    return faceDown(m_hid.getScheduler().getDefaultEventLoop());
  }

  /**
   * Constructs a Trigger instance around the Face Down button's digital signal.
   *
   * @param loop the event loop instance to attach the event to.
   * @return a Trigger instance representing the Face Down button's digital signal attached to the
   *     given loop.
   */
  public Trigger faceDown(EventLoop loop) {
    return button(Gamepad.Button.FACE_DOWN.value, loop);
  }

  /**
   * Constructs a Trigger instance around the Face Right button's digital signal.
   *
   * @return a Trigger instance representing the Face Right button's digital signal attached to the
   *     {@link Scheduler#getDefaultEventLoop() default scheduler event loop} on the scheduler
   *     passed to the controller's constructor, or the {@link Scheduler#getDefault default
   *     scheduler} if a scheduler was not explicitly provided.
   * @see #faceRight(EventLoop)
   */
  public Trigger faceRight() {
    return faceRight(m_hid.getScheduler().getDefaultEventLoop());
  }

  /**
   * Constructs a Trigger instance around the Face Right button's digital signal.
   *
   * @param loop the event loop instance to attach the event to.
   * @return a Trigger instance representing the Face Right button's digital signal attached to the
   *     given loop.
   */
  public Trigger faceRight(EventLoop loop) {
    return button(Gamepad.Button.FACE_RIGHT.value, loop);
  }

  /**
   * Constructs a Trigger instance around the Face Left button's digital signal.
   *
   * @return a Trigger instance representing the Face Left button's digital signal attached to the
   *     {@link Scheduler#getDefaultEventLoop() default scheduler event loop} on the scheduler
   *     passed to the controller's constructor, or the {@link Scheduler#getDefault default
   *     scheduler} if a scheduler was not explicitly provided.
   * @see #faceLeft(EventLoop)
   */
  public Trigger faceLeft() {
    return faceLeft(m_hid.getScheduler().getDefaultEventLoop());
  }

  /**
   * Constructs a Trigger instance around the Face Left button's digital signal.
   *
   * @param loop the event loop instance to attach the event to.
   * @return a Trigger instance representing the Face Left button's digital signal attached to the
   *     given loop.
   */
  public Trigger faceLeft(EventLoop loop) {
    return button(Gamepad.Button.FACE_LEFT.value, loop);
  }

  /**
   * Constructs a Trigger instance around the Face Up button's digital signal.
   *
   * @return a Trigger instance representing the Face Up button's digital signal attached to the
   *     {@link Scheduler#getDefaultEventLoop() default scheduler event loop} on the scheduler
   *     passed to the controller's constructor, or the {@link Scheduler#getDefault default
   *     scheduler} if a scheduler was not explicitly provided.
   * @see #faceUp(EventLoop)
   */
  public Trigger faceUp() {
    return faceUp(m_hid.getScheduler().getDefaultEventLoop());
  }

  /**
   * Constructs a Trigger instance around the Face Up button's digital signal.
   *
   * @param loop the event loop instance to attach the event to.
   * @return a Trigger instance representing the Face Up button's digital signal attached to the
   *     given loop.
   */
  public Trigger faceUp(EventLoop loop) {
    return button(Gamepad.Button.FACE_UP.value, loop);
  }

  /**
   * Constructs a Trigger instance around the Back button's digital signal.
   *
   * @return a Trigger instance representing the Back button's digital signal attached to the {@link
   *     Scheduler#getDefaultEventLoop() default scheduler event loop} on the scheduler passed to
   *     the controller's constructor, or the {@link Scheduler#getDefault default scheduler} if a
   *     scheduler was not explicitly provided.
   * @see #back(EventLoop)
   */
  public Trigger back() {
    return back(m_hid.getScheduler().getDefaultEventLoop());
  }

  /**
   * Constructs a Trigger instance around the Back button's digital signal.
   *
   * @param loop the event loop instance to attach the event to.
   * @return a Trigger instance representing the Back button's digital signal attached to the given
   *     loop.
   */
  public Trigger back(EventLoop loop) {
    return button(Gamepad.Button.BACK.value, loop);
  }

  /**
   * Constructs a Trigger instance around the Guide button's digital signal.
   *
   * @return a Trigger instance representing the Guide button's digital signal attached to the
   *     {@link Scheduler#getDefaultEventLoop() default scheduler event loop} on the scheduler
   *     passed to the controller's constructor, or the {@link Scheduler#getDefault default
   *     scheduler} if a scheduler was not explicitly provided.
   * @see #guide(EventLoop)
   */
  public Trigger guide() {
    return guide(m_hid.getScheduler().getDefaultEventLoop());
  }

  /**
   * Constructs a Trigger instance around the Guide button's digital signal.
   *
   * @param loop the event loop instance to attach the event to.
   * @return a Trigger instance representing the Guide button's digital signal attached to the given
   *     loop.
   */
  public Trigger guide(EventLoop loop) {
    return button(Gamepad.Button.GUIDE.value, loop);
  }

  /**
   * Constructs a Trigger instance around the Start button's digital signal.
   *
   * @return a Trigger instance representing the Start button's digital signal attached to the
   *     {@link Scheduler#getDefaultEventLoop() default scheduler event loop} on the scheduler
   *     passed to the controller's constructor, or the {@link Scheduler#getDefault default
   *     scheduler} if a scheduler was not explicitly provided.
   * @see #start(EventLoop)
   */
  public Trigger start() {
    return start(m_hid.getScheduler().getDefaultEventLoop());
  }

  /**
   * Constructs a Trigger instance around the Start button's digital signal.
   *
   * @param loop the event loop instance to attach the event to.
   * @return a Trigger instance representing the Start button's digital signal attached to the given
   *     loop.
   */
  public Trigger start(EventLoop loop) {
    return button(Gamepad.Button.START.value, loop);
  }

  /**
   * Constructs a Trigger instance around the left stick button's digital signal.
   *
   * @return a Trigger instance representing the left stick button's digital signal attached to the
   *     {@link Scheduler#getDefaultEventLoop() default scheduler event loop} on the scheduler
   *     passed to the controller's constructor, or the {@link Scheduler#getDefault default
   *     scheduler} if a scheduler was not explicitly provided.
   * @see #leftStick(EventLoop)
   */
  public Trigger leftStick() {
    return leftStick(m_hid.getScheduler().getDefaultEventLoop());
  }

  /**
   * Constructs a Trigger instance around the left stick button's digital signal.
   *
   * @param loop the event loop instance to attach the event to.
   * @return a Trigger instance representing the left stick button's digital signal attached to the
   *     given loop.
   */
  public Trigger leftStick(EventLoop loop) {
    return button(Gamepad.Button.LEFT_STICK.value, loop);
  }

  /**
   * Constructs a Trigger instance around the right stick button's digital signal.
   *
   * @return a Trigger instance representing the right stick button's digital signal attached to the
   *     {@link Scheduler#getDefaultEventLoop() default scheduler event loop} on the scheduler
   *     passed to the controller's constructor, or the {@link Scheduler#getDefault default
   *     scheduler} if a scheduler was not explicitly provided.
   * @see #rightStick(EventLoop)
   */
  public Trigger rightStick() {
    return rightStick(m_hid.getScheduler().getDefaultEventLoop());
  }

  /**
   * Constructs a Trigger instance around the right stick button's digital signal.
   *
   * @param loop the event loop instance to attach the event to.
   * @return a Trigger instance representing the right stick button's digital signal attached to the
   *     given loop.
   */
  public Trigger rightStick(EventLoop loop) {
    return button(Gamepad.Button.RIGHT_STICK.value, loop);
  }

  /**
   * Constructs a Trigger instance around the right bumper button's digital signal.
   *
   * @return a Trigger instance representing the right bumper button's digital signal attached to
   *     the {@link Scheduler#getDefaultEventLoop() default scheduler event loop} on the scheduler
   *     passed to the controller's constructor, or the {@link Scheduler#getDefault default
   *     scheduler} if a scheduler was not explicitly provided.
   * @see #leftBumper(EventLoop)
   */
  public Trigger leftBumper() {
    return leftBumper(m_hid.getScheduler().getDefaultEventLoop());
  }

  /**
   * Constructs a Trigger instance around the right bumper button's digital signal.
   *
   * @param loop the event loop instance to attach the event to.
   * @return a Trigger instance representing the right bumper button's digital signal attached to
   *     the given loop.
   */
  public Trigger leftBumper(EventLoop loop) {
    return button(Gamepad.Button.LEFT_BUMPER.value, loop);
  }

  /**
   * Constructs a Trigger instance around the right bumper button's digital signal.
   *
   * @return a Trigger instance representing the right bumper button's digital signal attached to
   *     the {@link Scheduler#getDefaultEventLoop() default scheduler event loop} on the scheduler
   *     passed to the controller's constructor, or the {@link Scheduler#getDefault default
   *     scheduler} if a scheduler was not explicitly provided.
   * @see #rightBumper(EventLoop)
   */
  public Trigger rightBumper() {
    return rightBumper(m_hid.getScheduler().getDefaultEventLoop());
  }

  /**
   * Constructs a Trigger instance around the right bumper button's digital signal.
   *
   * @param loop the event loop instance to attach the event to.
   * @return a Trigger instance representing the right bumper button's digital signal attached to
   *     the given loop.
   */
  public Trigger rightBumper(EventLoop loop) {
    return button(Gamepad.Button.RIGHT_BUMPER.value, loop);
  }

  /**
   * Constructs a Trigger instance around the D-pad up button's digital signal.
   *
   * @return a Trigger instance representing the D-pad up button's digital signal attached to the
   *     {@link Scheduler#getDefaultEventLoop() default scheduler event loop} on the scheduler
   *     passed to the controller's constructor, or the {@link Scheduler#getDefault default
   *     scheduler} if a scheduler was not explicitly provided.
   * @see #dpadUp(EventLoop)
   */
  public Trigger dpadUp() {
    return dpadUp(m_hid.getScheduler().getDefaultEventLoop());
  }

  /**
   * Constructs a Trigger instance around the D-pad up button's digital signal.
   *
   * @param loop the event loop instance to attach the event to.
   * @return a Trigger instance representing the D-pad up button's digital signal attached to the
   *     given loop.
   */
  public Trigger dpadUp(EventLoop loop) {
    return button(Gamepad.Button.DPAD_UP.value, loop);
  }

  /**
   * Constructs a Trigger instance around the D-pad down button's digital signal.
   *
   * @return a Trigger instance representing the D-pad down button's digital signal attached to the
   *     {@link Scheduler#getDefaultEventLoop() default scheduler event loop} on the scheduler
   *     passed to the controller's constructor, or the {@link Scheduler#getDefault default
   *     scheduler} if a scheduler was not explicitly provided.
   * @see #dpadDown(EventLoop)
   */
  public Trigger dpadDown() {
    return dpadDown(m_hid.getScheduler().getDefaultEventLoop());
  }

  /**
   * Constructs a Trigger instance around the D-pad down button's digital signal.
   *
   * @param loop the event loop instance to attach the event to.
   * @return a Trigger instance representing the D-pad down button's digital signal attached to the
   *     given loop.
   */
  public Trigger dpadDown(EventLoop loop) {
    return button(Gamepad.Button.DPAD_DOWN.value, loop);
  }

  /**
   * Constructs a Trigger instance around the D-pad left button's digital signal.
   *
   * @return a Trigger instance representing the D-pad left button's digital signal attached to the
   *     {@link Scheduler#getDefaultEventLoop() default scheduler event loop} on the scheduler
   *     passed to the controller's constructor, or the {@link Scheduler#getDefault default
   *     scheduler} if a scheduler was not explicitly provided.
   * @see #dpadLeft(EventLoop)
   */
  public Trigger dpadLeft() {
    return dpadLeft(m_hid.getScheduler().getDefaultEventLoop());
  }

  /**
   * Constructs a Trigger instance around the D-pad left button's digital signal.
   *
   * @param loop the event loop instance to attach the event to.
   * @return a Trigger instance representing the D-pad left button's digital signal attached to the
   *     given loop.
   */
  public Trigger dpadLeft(EventLoop loop) {
    return button(Gamepad.Button.DPAD_LEFT.value, loop);
  }

  /**
   * Constructs a Trigger instance around the D-pad right button's digital signal.
   *
   * @return a Trigger instance representing the D-pad right button's digital signal attached to the
   *     {@link Scheduler#getDefaultEventLoop() default scheduler event loop} on the scheduler
   *     passed to the controller's constructor, or the {@link Scheduler#getDefault default
   *     scheduler} if a scheduler was not explicitly provided.
   * @see #dpadRight(EventLoop)
   */
  public Trigger dpadRight() {
    return dpadRight(m_hid.getScheduler().getDefaultEventLoop());
  }

  /**
   * Constructs a Trigger instance around the D-pad right button's digital signal.
   *
   * @param loop the event loop instance to attach the event to.
   * @return a Trigger instance representing the D-pad right button's digital signal attached to the
   *     given loop.
   */
  public Trigger dpadRight(EventLoop loop) {
    return button(Gamepad.Button.DPAD_RIGHT.value, loop);
  }

  /**
   * Constructs a Trigger instance around the Miscellaneous 1 button's digital signal.
   *
   * @return a Trigger instance representing the Miscellaneous 1 button's digital signal attached to
   *     the {@link Scheduler#getDefaultEventLoop() default scheduler event loop} on the scheduler
   *     passed to the controller's constructor, or the {@link Scheduler#getDefault default
   *     scheduler} if a scheduler was not explicitly provided.
   * @see #misc1(EventLoop)
   */
  public Trigger misc1() {
    return misc1(m_hid.getScheduler().getDefaultEventLoop());
  }

  /**
   * Constructs a Trigger instance around the Miscellaneous 1 button's digital signal.
   *
   * @param loop the event loop instance to attach the event to.
   * @return a Trigger instance representing the Miscellaneous 1 button's digital signal attached to
   *     the given loop.
   */
  public Trigger misc1(EventLoop loop) {
    return button(Gamepad.Button.MISC_1.value, loop);
  }

  /**
   * Constructs a Trigger instance around the Right Paddle 1 button's digital signal.
   *
   * @return a Trigger instance representing the Right Paddle 1 button's digital signal attached to
   *     the {@link Scheduler#getDefaultEventLoop() default scheduler event loop} on the scheduler
   *     passed to the controller's constructor, or the {@link Scheduler#getDefault default
   *     scheduler} if a scheduler was not explicitly provided.
   * @see #rightPaddle1(EventLoop)
   */
  public Trigger rightPaddle1() {
    return rightPaddle1(m_hid.getScheduler().getDefaultEventLoop());
  }

  /**
   * Constructs a Trigger instance around the Right Paddle 1 button's digital signal.
   *
   * @param loop the event loop instance to attach the event to.
   * @return a Trigger instance representing the Right Paddle 1 button's digital signal attached to
   *     the given loop.
   */
  public Trigger rightPaddle1(EventLoop loop) {
    return button(Gamepad.Button.RIGHT_PADDLE_1.value, loop);
  }

  /**
   * Constructs a Trigger instance around the Left Paddle 1 button's digital signal.
   *
   * @return a Trigger instance representing the Left Paddle 1 button's digital signal attached to
   *     the {@link Scheduler#getDefaultEventLoop() default scheduler event loop} on the scheduler
   *     passed to the controller's constructor, or the {@link Scheduler#getDefault default
   *     scheduler} if a scheduler was not explicitly provided.
   * @see #leftPaddle1(EventLoop)
   */
  public Trigger leftPaddle1() {
    return leftPaddle1(m_hid.getScheduler().getDefaultEventLoop());
  }

  /**
   * Constructs a Trigger instance around the Left Paddle 1 button's digital signal.
   *
   * @param loop the event loop instance to attach the event to.
   * @return a Trigger instance representing the Left Paddle 1 button's digital signal attached to
   *     the given loop.
   */
  public Trigger leftPaddle1(EventLoop loop) {
    return button(Gamepad.Button.LEFT_PADDLE_1.value, loop);
  }

  /**
   * Constructs a Trigger instance around the Right Paddle 2 button's digital signal.
   *
   * @return a Trigger instance representing the Right Paddle 2 button's digital signal attached to
   *     the {@link Scheduler#getDefaultEventLoop() default scheduler event loop} on the scheduler
   *     passed to the controller's constructor, or the {@link Scheduler#getDefault default
   *     scheduler} if a scheduler was not explicitly provided.
   * @see #rightPaddle2(EventLoop)
   */
  public Trigger rightPaddle2() {
    return rightPaddle2(m_hid.getScheduler().getDefaultEventLoop());
  }

  /**
   * Constructs a Trigger instance around the Right Paddle 2 button's digital signal.
   *
   * @param loop the event loop instance to attach the event to.
   * @return a Trigger instance representing the Right Paddle 2 button's digital signal attached to
   *     the given loop.
   */
  public Trigger rightPaddle2(EventLoop loop) {
    return button(Gamepad.Button.RIGHT_PADDLE_2.value, loop);
  }

  /**
   * Constructs a Trigger instance around the Left Paddle 2 button's digital signal.
   *
   * @return a Trigger instance representing the Left Paddle 2 button's digital signal attached to
   *     the {@link Scheduler#getDefaultEventLoop() default scheduler event loop} on the scheduler
   *     passed to the controller's constructor, or the {@link Scheduler#getDefault default
   *     scheduler} if a scheduler was not explicitly provided.
   * @see #leftPaddle2(EventLoop)
   */
  public Trigger leftPaddle2() {
    return leftPaddle2(m_hid.getScheduler().getDefaultEventLoop());
  }

  /**
   * Constructs a Trigger instance around the Left Paddle 2 button's digital signal.
   *
   * @param loop the event loop instance to attach the event to.
   * @return a Trigger instance representing the Left Paddle 2 button's digital signal attached to
   *     the given loop.
   */
  public Trigger leftPaddle2(EventLoop loop) {
    return button(Gamepad.Button.LEFT_PADDLE_2.value, loop);
  }

  /**
   * Constructs a Trigger instance around the Touchpad button's digital signal.
   *
   * @return a Trigger instance representing the Touchpad button's digital signal attached to the
   *     {@link Scheduler#getDefaultEventLoop() default scheduler event loop} on the scheduler
   *     passed to the controller's constructor, or the {@link Scheduler#getDefault default
   *     scheduler} if a scheduler was not explicitly provided.
   * @see #touchpad(EventLoop)
   */
  public Trigger touchpad() {
    return touchpad(m_hid.getScheduler().getDefaultEventLoop());
  }

  /**
   * Constructs a Trigger instance around the Touchpad button's digital signal.
   *
   * @param loop the event loop instance to attach the event to.
   * @return a Trigger instance representing the Touchpad button's digital signal attached to the
   *     given loop.
   */
  public Trigger touchpad(EventLoop loop) {
    return button(Gamepad.Button.TOUCHPAD.value, loop);
  }

  /**
   * Constructs a Trigger instance around the Miscellaneous 2 button's digital signal.
   *
   * @return a Trigger instance representing the Miscellaneous 2 button's digital signal attached to
   *     the {@link Scheduler#getDefaultEventLoop() default scheduler event loop} on the scheduler
   *     passed to the controller's constructor, or the {@link Scheduler#getDefault default
   *     scheduler} if a scheduler was not explicitly provided.
   * @see #misc2(EventLoop)
   */
  public Trigger misc2() {
    return misc2(m_hid.getScheduler().getDefaultEventLoop());
  }

  /**
   * Constructs a Trigger instance around the Miscellaneous 2 button's digital signal.
   *
   * @param loop the event loop instance to attach the event to.
   * @return a Trigger instance representing the Miscellaneous 2 button's digital signal attached to
   *     the given loop.
   */
  public Trigger misc2(EventLoop loop) {
    return button(Gamepad.Button.MISC_2.value, loop);
  }

  /**
   * Constructs a Trigger instance around the Miscellaneous 3 button's digital signal.
   *
   * @return a Trigger instance representing the Miscellaneous 3 button's digital signal attached to
   *     the {@link Scheduler#getDefaultEventLoop() default scheduler event loop} on the scheduler
   *     passed to the controller's constructor, or the {@link Scheduler#getDefault default
   *     scheduler} if a scheduler was not explicitly provided.
   * @see #misc3(EventLoop)
   */
  public Trigger misc3() {
    return misc3(m_hid.getScheduler().getDefaultEventLoop());
  }

  /**
   * Constructs a Trigger instance around the Miscellaneous 3 button's digital signal.
   *
   * @param loop the event loop instance to attach the event to.
   * @return a Trigger instance representing the Miscellaneous 3 button's digital signal attached to
   *     the given loop.
   */
  public Trigger misc3(EventLoop loop) {
    return button(Gamepad.Button.MISC_3.value, loop);
  }

  /**
   * Constructs a Trigger instance around the Miscellaneous 4 button's digital signal.
   *
   * @return a Trigger instance representing the Miscellaneous 4 button's digital signal attached to
   *     the {@link Scheduler#getDefaultEventLoop() default scheduler event loop} on the scheduler
   *     passed to the controller's constructor, or the {@link Scheduler#getDefault default
   *     scheduler} if a scheduler was not explicitly provided.
   * @see #misc4(EventLoop)
   */
  public Trigger misc4() {
    return misc4(m_hid.getScheduler().getDefaultEventLoop());
  }

  /**
   * Constructs a Trigger instance around the Miscellaneous 4 button's digital signal.
   *
   * @param loop the event loop instance to attach the event to.
   * @return a Trigger instance representing the Miscellaneous 4 button's digital signal attached to
   *     the given loop.
   */
  public Trigger misc4(EventLoop loop) {
    return button(Gamepad.Button.MISC_4.value, loop);
  }

  /**
   * Constructs a Trigger instance around the Miscellaneous 5 button's digital signal.
   *
   * @return a Trigger instance representing the Miscellaneous 5 button's digital signal attached to
   *     the {@link Scheduler#getDefaultEventLoop() default scheduler event loop} on the scheduler
   *     passed to the controller's constructor, or the {@link Scheduler#getDefault default
   *     scheduler} if a scheduler was not explicitly provided.
   * @see #misc5(EventLoop)
   */
  public Trigger misc5() {
    return misc5(m_hid.getScheduler().getDefaultEventLoop());
  }

  /**
   * Constructs a Trigger instance around the Miscellaneous 5 button's digital signal.
   *
   * @param loop the event loop instance to attach the event to.
   * @return a Trigger instance representing the Miscellaneous 5 button's digital signal attached to
   *     the given loop.
   */
  public Trigger misc5(EventLoop loop) {
    return button(Gamepad.Button.MISC_5.value, loop);
  }

  /**
   * Constructs a Trigger instance around the Miscellaneous 6 button's digital signal.
   *
   * @return a Trigger instance representing the Miscellaneous 6 button's digital signal attached to
   *     the {@link Scheduler#getDefaultEventLoop() default scheduler event loop} on the scheduler
   *     passed to the controller's constructor, or the {@link Scheduler#getDefault default
   *     scheduler} if a scheduler was not explicitly provided.
   * @see #misc6(EventLoop)
   */
  public Trigger misc6() {
    return misc6(m_hid.getScheduler().getDefaultEventLoop());
  }

  /**
   * Constructs a Trigger instance around the Miscellaneous 6 button's digital signal.
   *
   * @param loop the event loop instance to attach the event to.
   * @return a Trigger instance representing the Miscellaneous 6 button's digital signal attached to
   *     the given loop.
   */
  public Trigger misc6(EventLoop loop) {
    return button(Gamepad.Button.MISC_6.value, loop);
  }

  /**
   * Constructs a Trigger instance around the axis value of the left trigger. The returned trigger
   * will be true when the axis value is greater than {@code threshold}.
   *
   * @param threshold the minimum axis value for the returned {@link Trigger} to be true. This value
   *     should be in the range [0, 1] where 0 is the unpressed state of the axis.
   * @param loop the event loop instance to attach the Trigger to.
   * @return a Trigger instance that is true when the left trigger's axis exceeds the provided
   *     threshold, attached to the given event loop
   */
  public Trigger leftTrigger(double threshold, EventLoop loop) {
    return m_hid.axisGreaterThan(Gamepad.Axis.LEFT_TRIGGER.value, threshold, loop);
  }

  /**
   * Constructs a Trigger instance around the axis value of the left trigger. The returned trigger
   * will be true when the axis value is greater than {@code threshold}.
   *
   * @param threshold the minimum axis value for the returned {@link Trigger} to be true. This value
   *     should be in the range [0, 1] where 0 is the unpressed state of the axis.
   * @return a Trigger instance that is true when the left trigger's axis exceeds the provided
   *     threshold, attached to the {@link Scheduler#getDefaultEventLoop() default scheduler button
   *     loop}.
   */
  public Trigger leftTrigger(double threshold) {
    return leftTrigger(threshold, m_hid.getScheduler().getDefaultEventLoop());
  }

  /**
   * Constructs a Trigger instance around the axis value of the left trigger. The returned trigger
   * will be true when the axis value is greater than 0.5.
   *
   * @return a Trigger instance that is true when the left trigger's axis exceeds 0.5, attached to
   *     the {@link Scheduler#getDefaultEventLoop() default scheduler event loop} on the scheduler
   *     passed to the controller's constructor, or the {@link Scheduler#getDefault default
   *     scheduler} if a scheduler was not explicitly provided.
   */
  public Trigger leftTrigger() {
    return leftTrigger(0.5);
  }

  /**
   * Constructs a Trigger instance around the axis value of the right trigger. The returned trigger
   * will be true when the axis value is greater than {@code threshold}.
   *
   * @param threshold the minimum axis value for the returned {@link Trigger} to be true. This value
   *     should be in the range [0, 1] where 0 is the unpressed state of the axis.
   * @param loop the event loop instance to attach the Trigger to.
   * @return a Trigger instance that is true when the right trigger's axis exceeds the provided
   *     threshold, attached to the given event loop
   */
  public Trigger rightTrigger(double threshold, EventLoop loop) {
    return m_hid.axisGreaterThan(Gamepad.Axis.RIGHT_TRIGGER.value, threshold, loop);
  }

  /**
   * Constructs a Trigger instance around the axis value of the right trigger. The returned trigger
   * will be true when the axis value is greater than {@code threshold}.
   *
   * @param threshold the minimum axis value for the returned {@link Trigger} to be true. This value
   *     should be in the range [0, 1] where 0 is the unpressed state of the axis.
   * @return a Trigger instance that is true when the right trigger's axis exceeds the provided
   *     threshold, attached to the {@link Scheduler#getDefaultEventLoop() default scheduler button
   *     loop}.
   */
  public Trigger rightTrigger(double threshold) {
    return rightTrigger(threshold, m_hid.getScheduler().getDefaultEventLoop());
  }

  /**
   * Constructs a Trigger instance around the axis value of the right trigger. The returned trigger
   * will be true when the axis value is greater than 0.5.
   *
   * @return a Trigger instance that is true when the right trigger's axis exceeds 0.5, attached to
   *     the {@link Scheduler#getDefaultEventLoop() default scheduler event loop} on the scheduler
   *     passed to the controller's constructor, or the {@link Scheduler#getDefault default
   *     scheduler} if a scheduler was not explicitly provided.
   */
  public Trigger rightTrigger() {
    return rightTrigger(0.5);
  }

  /**
   * Get the X axis value of left side of the controller. Right is positive.
   *
   * @return The axis value.
   */
  public double getLeftX() {
    return m_gamepad.getLeftX();
  }

  /**
   * Get the Y axis value of left side of the controller. Back is positive.
   *
   * @return The axis value.
   */
  public double getLeftY() {
    return m_gamepad.getLeftY();
  }

  /**
   * Get the X axis value of right side of the controller. Right is positive.
   *
   * @return The axis value.
   */
  public double getRightX() {
    return m_gamepad.getRightX();
  }

  /**
   * Get the Y axis value of right side of the controller. Back is positive.
   *
   * @return The axis value.
   */
  public double getRightY() {
    return m_gamepad.getRightY();
  }

  /**
   * Get the left trigger value of the controller. Note that this axis is bound to the range of [0,
   * 1] as opposed to the usual [-1, 1].
   *
   * @return The axis value.
   */
  public double getLeftTrigger() {
    return m_gamepad.getLeftTrigger();
  }

  /**
   * Get the right trigger value of the controller. Note that this axis is bound to the range of [0,
   * 1] as opposed to the usual [-1, 1].
   *
   * @return The axis value.
   */
  public double getRightTrigger() {
    return m_gamepad.getRightTrigger();
  }

  /**
   * Run the left rumble motor. On most controllers, this is the low-frequency motor.
   *
   * @param value The normalized value (0 to 1) to set the rumble to
   * @return A command that will run the left rumble motor at the given value until interrupted.
   */
  public Command rumbleLeft(double value) {
    return m_hid.rumbleLeft(value);
  }

  /**
   * Run the right rumble motor. On most controllers, this is the high-frequency motor.
   *
   * @param value The normalized value (0 to 1) to set the rumble to
   * @return A command that will run the right rumble motor at the given value until interrupted.
   */
  public Command rumbleRight(double value) {
    return m_hid.rumbleRight(value);
  }

  /**
   * Run both rumble motors.
   *
   * @param value The normalized value (0 to 1) to set the rumble to
   * @return A command that will run the rumble motors at the given value until interrupted.
   */
  public Command rumbleBoth(double value) {
    return m_hid.rumbleBoth(value);
  }

  /**
   * Run the left trigger rumble motor, on controllers that have one.
   *
   * @param value The normalized value (0 to 1) to set the rumble to
   * @return A command that will run the left trigger rumble motor at the given value until
   *     interrupted.
   */
  public Command rumbleLeftTrigger(double value) {
    return m_hid.rumbleLeftTrigger(value);
  }

  /**
   * Run the right trigger rumble motor, on controllers that have one.
   *
   * @param value The normalized value (0 to 1) to set the rumble to
   * @return A command that will run the right trigger rumble motor at the given value until
   *     interrupted.
   */
  public Command rumbleRightTrigger(double value) {
    return m_hid.rumbleRightTrigger(value);
  }

  /**
   * Run both trigger rumble motors, on controllers that have them.
   *
   * @param value The normalized value (0 to 1) to set the rumble to
   * @return A command that will run both trigger rumble motors at the given value until
   *     interrupted.
   */
  public Command rumbleTriggers(double value) {
    return m_hid.rumbleBoth(value);
  }

  /**
   * Set the LEDs, on controllers that have them.
   *
   * @param r The red value (0-255)
   * @param g The green value (0-255)
   * @param b The blue value (0-255)
   * @return A command that will set the LEDs to the given values until interrupted.
   */
  public Command setLeds(int r, int g, int b) {
    return m_hid.setLeds(r, g, b);
  }
}
