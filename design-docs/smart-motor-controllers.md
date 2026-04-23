# Smart Motor Controller API

## 1. Motivation

Currently, WPILib has a "dumb" motor controller API that lives in the `MotorController` class, and contains five functions: `setThrottle`, `setVoltage`, `getThrottle`, `setInverted`, and `disable`. These five functions are not really enough to get much done with, and as such vendors (and other WPILib motor classes such as `ExpansionHubMotor`) have created smarter motor controller APIs that allow users to do more powerful control using onboard PID and feedforward and such.

We wish to provide a standard "smart" motor controller API that all vendors can implement, and that users can rely on to be present regardless of the specific motor controller they are using. This will allow users to write more powerful control code without having to worry about vendor-specific APIs, and will also allow us to provide better documentation and examples for how to use these features.

## 2. Background

### 2. FRC

(todo)

### 2.2 FTC

Currently, all motor control in FTC uses the either the REV Expansion Hub (or the REV Control Hub which is the same thing but it also has an Android board so that teams don't need to have an Android phone on their robot), or technically the SparkMini motor controller but those are rarely used.

For the builtin Expansion Hub motor controllers, the FTC SDK has the `DcMotor` and `DcMotorEx` classes. All motors are `DcMotorEx` objects (this is because the SDK had to support the Modern Robotics control system which did not support certain things with its motor controllers), which have the ability to do the following: 
- set a power (aka a duty cycle or "throttle" as we call it now) between -1 and 1 (where 0 is stopped, 1 is full power in the positive direction, and -1 is full power in the reverse direction),
- set a velocity in ticks per second**
- set a target position in ticks**
- reset the encoder position to zero
- set the motor to brake or coast
- invert the direction of the motor
- set the PIDF coefficients for the motor
 
- Note that the FTC SDK manually requires users to change the run mode of the motor when trying to do these things, also the run mode names make very little sense. WPILib does not do this for its Expansion Hub APIs. 
- **Note also that the builtin motor controllers on the Expansion Hub run their PID loops at a fixed 20 Hz rate for some reason.

## 3. Things The API Should Support

Thus we think the API should support the following operations:
- Set the throttle (aka power) of the motor between -1 and 1
- Set the voltage of the motor between -Vbat and Vbat
- Get the throttle (aka power) of the motor
- Get the voltage of the motor
- Set a position setpoint in ticks
- Get the current position of the motor in ticks
- Set a velocity setpoint in ticks per second
- Get the current velocity of the motor in ticks per second
- Set the brake mode of the motor to coast or brake
- Invert the direction of the motor
- Follow another motor controller
