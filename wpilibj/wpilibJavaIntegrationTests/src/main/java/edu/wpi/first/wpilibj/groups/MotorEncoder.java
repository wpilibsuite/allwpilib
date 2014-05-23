package edu.wpi.first.wpilibj.groups;

import edu.wpi.first.wpilibj.Counter;
import edu.wpi.first.wpilibj.DigitalInput;
import edu.wpi.first.wpilibj.DigitalSource;
import edu.wpi.first.wpilibj.Encoder;
import edu.wpi.first.wpilibj.SpeedController;
import edu.wpi.first.wpilibj.Timer;
import edu.wpi.first.wpilibj.test.TestBench;

public class MotorEncoder {
	
	private final SpeedController motor;
	private final Encoder encoder;
	private final Counter counters[] = new Counter[2];
	
	public MotorEncoder(SpeedController motor, DigitalInput aSource, DigitalInput bSource){
		this.motor = motor;
		this.encoder = new Encoder(aSource, bSource);
		counters[0] = new Counter(aSource);
		counters[1] = new Counter(bSource);
		for(Counter c: counters){
			c.start();
		}
	}
	
	public SpeedController getMotor(){
		return motor;
	}
	
	public Encoder getEncoder(){
		return encoder;
	}
	
	public Counter[] getCounters(){
		return counters;
	}
	
	public void reset(){
		motor.set(0);
		Timer.delay(TestBench.MOTOR_STOP_TIME);
		encoder.reset();
		for(Counter c : counters){
			c.reset();
		}
	}

}
