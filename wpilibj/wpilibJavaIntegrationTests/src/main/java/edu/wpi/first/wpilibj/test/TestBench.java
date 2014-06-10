/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008-2014. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/
package edu.wpi.first.wpilibj.test;

import java.util.ArrayList;
import java.util.Arrays;
import java.util.Collection;
import java.util.Iterator;
import java.util.List;

import edu.wpi.first.wpilibj.CANJaguar;
import edu.wpi.first.wpilibj.DigitalInput;
import edu.wpi.first.wpilibj.DigitalOutput;
import edu.wpi.first.wpilibj.Gyro;
import edu.wpi.first.wpilibj.Jaguar;
import edu.wpi.first.wpilibj.Servo;
import edu.wpi.first.wpilibj.Talon;
import edu.wpi.first.wpilibj.Victor;
import edu.wpi.first.wpilibj.can.CANTimeoutException;
import edu.wpi.first.wpilibj.fixtures.DIOCrossConnectFixture;
import edu.wpi.first.wpilibj.fixtures.MotorEncoderFixture;
import edu.wpi.first.wpilibj.fixtures.TiltPanCameraFixture;

/**
 * This class provides access to all of the elements on the test bench, for use
 * in fixtures. This class is a singleton, you should use {@link #getInstance()}
 * to obtain a reference to the {@code TestBench}.
 * 
 * TODO: This needs to be updated to the most recent test bench setup.
 * 
 * @author Fredric Silberberg
 */
public final class TestBench {

	/**
	 * The time that it takes to have a motor go from rotating at full speed to
	 * completely stopped
	 */
	public static final double MOTOR_STOP_TIME = 0.15;
	
	
	//THESE MUST BE IN INCREMENTING ORDER
	public static final int DIOCrossConnectA1 = 6;
	public static final int DIOCrossConnectA2 = 7;
	public static final int DIOCrossConnectB1 = 8;
	public static final int DIOCrossConnectB2 = 9;
	
	/** The Singleton instance of the Test Bench */
	private static TestBench instance = null;

	private CANJaguar canJag = null; // This is declared externally because it
										// does not have a free method

	/**
	 * The single constructor for the TestBench. This method is private in order
	 * to prevent multiple TestBench objects from being allocated
	 */
	private TestBench() {
	}

	/**
	 * Constructs a new set of objects representing a connected set of Talon
	 * controlled Motors and an encoder
	 * 
	 * @return a freshly allocated Talon, Encoder pair
	 */
	public MotorEncoderFixture getTalonPair() {
		Talon talon = new Talon(0);
		DigitalInput encA1 = new DigitalInput(0);
		DigitalInput encB1 = new DigitalInput(1);

		MotorEncoderFixture talonPair = new MotorEncoderFixture(talon, encA1,
				encB1);
		return talonPair;
	}

	/**
	 * Constructs a new set of objects representing a connected set of Victor
	 * controlled Motors and an encoder
	 * 
	 * @return a freshly allocated Victor, Encoder pair
	 */
	public MotorEncoderFixture getVictorPair() {
		Victor vic = new Victor(1);
		DigitalInput encA2 = new DigitalInput(2);
		DigitalInput encB2 = new DigitalInput(3);
		MotorEncoderFixture vicPair = new MotorEncoderFixture(vic, encA2, encB2);
		return vicPair;
	}

	/**
	 * Constructs a new set of objects representing a connected set of Jaguar
	 * controlled Motors and an encoder
	 * 
	 * @return a freshly allocated Jaguar, Encoder pair
	 */
	public MotorEncoderFixture getJaguarPair() {
		Jaguar jag = new Jaguar(2);
		DigitalInput encA3 = new DigitalInput(4);
		DigitalInput encB3 = new DigitalInput(5);
		MotorEncoderFixture jagPair = new MotorEncoderFixture(jag, encA3, encB3);
		return jagPair;
	}

	/**
	 * Constructs a new set of objects representing a connected set of CANJaguar
	 * controlled Motors and an encoder<br>
	 * Note: The CANJaguar is not freshly allocated because the CANJaguar lacks
	 * a free() method
	 * 
	 * @return an existing CANJaguar and a freshly allocated Encoder
	 */
	public MotorEncoderFixture getCanJaguarPair() {

		DigitalInput encA4 = new DigitalInput(6);
		DigitalInput encB4 = new DigitalInput(7);
		MotorEncoderFixture canPair;
		if (canJag == null) { // Again this is because the CanJaguar does not
								// have a free method
			try {
				canJag = new CANJaguar(1);
			} catch (CANTimeoutException e) {
				e.printStackTrace();
			}
		}
		canPair = new MotorEncoderFixture(canJag, encA4, encB4);
		assert canPair != null;
		return canPair;
	}

	/**
	 * Constructs a new set of two Servo's and a Gyroscope.
	 * 
	 * @return a freshly allocated Servo's and a freshly allocated Gyroscope
	 */
	public TiltPanCameraFixture getTiltPanCam() {
		Gyro gyro = new Gyro(0);
		gyro.setSensitivity(.007); // If a different gyroscope is used this
									// value will be different

		Servo tilt = new Servo(9);
		Servo pan = new Servo(8);

		TiltPanCameraFixture tpcam = new TiltPanCameraFixture(tilt, pan, gyro);

		return tpcam;
	}

	public DIOCrossConnectFixture getDIOCrossConnectFixture(int inputPort,
			int outputPort) {
		DIOCrossConnectFixture dio = new DIOCrossConnectFixture(inputPort, outputPort);
		return dio;
	}
	
	/**
	 * Gets two lists of possible DIO pairs for the two pairs
	 * @return
	 */
	private List<List<Integer[]>> getDIOCrossConnect(){
		List<List<Integer[]>> pairs = new ArrayList<List<Integer[]>>();
		List<Integer[]> setA = Arrays.asList(new Integer[][] {
			{ new Integer(DIOCrossConnectA1), new Integer(DIOCrossConnectA2) },
			{ new Integer(DIOCrossConnectA2), new Integer(DIOCrossConnectA1) }
		});
		pairs.add(setA);
		
		List<Integer[]> setB = Arrays.asList(new Integer[][] {
			{ new Integer(DIOCrossConnectB1), new Integer(DIOCrossConnectB2) },
			{ new Integer(DIOCrossConnectB2), new Integer(DIOCrossConnectB1) }
		});
		pairs.add(setB);
		//NOTE: IF MORE DIOCROSSCONNECT PAIRS ARE ADDED ADD THEM HERE
		return pairs;
	}

	/**
	 * Return a single Collection containing all of the DIOCrossConnectFixtures in all two pair combinations
	 * @return
	 */
	public Collection<Integer[]> getDIOCrossConnectCollection() {
		Collection<Integer[]> pairs = new ArrayList<Integer[]>();
		for(Collection<Integer[]> collection : getDIOCrossConnect()){
			pairs.addAll(collection);
		}
		return pairs;
	}
	
	/**
	 * Gets an array of pairs for the encoder to use using two different lists
	 * @param listA
	 * @param listB
	 * @param flip whether this encoder needs to be flipped
	 * @return A list of different inputs to use for the tests
	 */
	private Collection<Integer[]> getPairArray(List<Integer[]> listA, List<Integer[]> listB, boolean flip){
		Collection<Integer[]> encoderPortPairs = new ArrayList<Integer[]>();
		for (Integer[] portPairsA : listA) {
			ArrayList<Integer[]> construtorInput = new ArrayList<Integer[]>();
			Integer inputs[] = new Integer[5];
			inputs[0] = portPairsA[0]; // InputA
			inputs[1] = portPairsA[1]; // InputB

			for (Integer[] portPairsB : listB) {
				inputs[2] = portPairsB[0]; 	// OutputA
				inputs[3] = portPairsB[1]; 	// OutputB
				inputs [4] = flip? 0 : 1;  	// The flip bit
			}
			
			construtorInput.add(inputs);

			inputs = inputs.clone();
			for (Integer[] portPairsB : listB) {
				inputs[2] = portPairsB[1]; 	// OutputA
				inputs[3] = portPairsB[0]; 	// OutputB
				inputs [4] = flip? 0 : 1;	//The flip bit
			}
			construtorInput.add(inputs);
			encoderPortPairs.addAll(construtorInput);
		}
		return encoderPortPairs;
	}
	
	/**
	 * Constructs the list of inputs to be used for the encoder test
	 * @return A collection of different input pairs to use for the encoder
	 */
	public Collection<Integer[]> getEncoderDIOCrossConnectCollection() {
		Collection<Integer[]> encoderPortPairs = new ArrayList<Integer[]>();
		assert getDIOCrossConnect().size() == 2;
		encoderPortPairs.addAll(getPairArray(getDIOCrossConnect().get(0), getDIOCrossConnect().get(1), false));
		encoderPortPairs.addAll(getPairArray(getDIOCrossConnect().get(1), getDIOCrossConnect().get(0), false));
		assert (encoderPortPairs.size() == 8);
		return encoderPortPairs;
	}

	/**
	 * Gets the singleton of the TestBench. If the TestBench is not already
	 * allocated in constructs an new instance of it. Otherwise it returns the
	 * existing instance.
	 * 
	 * @return The Singleton instance of the TestBench
	 */
	public static TestBench getInstance() {
		if (instance == null) {
			instance = new TestBench();
		}
		return instance;
	}
}
