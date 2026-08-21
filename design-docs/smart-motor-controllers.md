# Smart Motor Controller API

## 1. Motivation

Currently, WPILib has a "dumb" motor controller API that lives in the `MotorController` class, and contains five functions: `setThrottle`, `setVoltage`, `getThrottle`, `setInverted`, and `disable`. These five functions are not really enough to get much done with, and as such vendors (and other WPILib motor classes such as `ExpansionHubMotor`) have created smarter motor controller APIs that allow users to do more powerful control using onboard PID and feedforward and such.

We wish to provide a standard "smart" motor controller API that all vendors can implement, and that users can rely on to be present regardless of the specific motor controller they are using. This will allow users to write more powerful control code without having to worry about vendor-specific APIs, and will also allow us to provide better documentation and examples for how to use these features.

## 2. Background

### 2.1 FRC

The "dumb" motor controller API evolved from the older age of FRC when PWM motor controllers and brushed motors were more common, without any form of feedback to the user. The motor controller API allowed users to think less about vendor specific code and WPILib examples to be widely applicable to teams. With the rise of CAN motor controllers teams were still able to use the motor controller API for simple tasks, with vendor APIs being available to take advantage of more advanced features. Unfortunately with the rise of brushless motors this API is now essentially useless. With the vast majority of FRC motor controllers used today featuring things like in built position control or current limiting vendor APIs have essentially become required.

Across most vendors there is a relatively standard baseline of features teams have come to expect for brushless motor controllers today. These APIs differ across vendors but the following are common features provided by the 2 most used vendor dependencies in FRC, Phoenix6 and REVLib:
- set a duty cycle
- set a voltage
- onboard position control (PID, SVGA feedforward, motion profile, 1khz update rate)
- onboard velocity control (PID, SVGA feedforward, 1khz update rate)
- stator current limits
- invert motor direction
- follow another motor
- read duty cycle, voltage, position, velocity or current
- soft and hard mechanism limits

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

### 3.1. Basic Control

The API should support basic control of the motor: setThrottle (-1 to 1) and setVoltage (-Vbat to Vbat). It should also support getting the current throttle and voltage, setting the zero power behavior (brake or coast), and inverting the direction of the motor.

### 3.2. Position Control

The API should support position control of the motor: setPositionSetpoint in rotations and getPosition in rotations. This should ideally be implemented using a feedback and feedforward loop; as such there should be methods to set position constants, including P, I, D, and S, and continuous input control.

- Open question: gravity feedforward? Because the gravity feedforward is different for elevators vs arms, I'm not sure if we should include it in the API.

### 3.3. Velocity Control

The API should support velocity control of the motor: setVelocitySetpoint in rotations per second and getVelocity in rotations per second. Similarly, should ideally be implemented using a feedback and feedforward loop; as such there should be methods to set velocity constants, including P, I, D, S, V, and A.

- Open question: gravity feedforward? see above.

### 3.4. Following Behavior

The API should support following behavior, where one motor controller can follow another motor controller.

### 3.5. Other Data

The API should be able to set the encoder resolution (such that it can convert between encoder ticks and rotations in order to allow users to use rotations as the units for position and velocity control). It should also allow the user to get the current draw of the motor.

- Open question: should we create an Encoder API and add it as a field to the motor controller / a `getEncoder()` method instead of `getPosition()` and `getVelocity()` methods? This would also allow users to be able to use alternate encoders for motors if they so choose.
