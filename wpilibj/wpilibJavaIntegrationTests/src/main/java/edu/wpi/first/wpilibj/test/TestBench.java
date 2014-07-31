/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008-2014. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/
package edu.wpi.first.wpilibj.test;

import java.io.PrintStream;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.Collection;
import java.util.List;

import edu.wpi.first.wpilibj.AnalogInput;
import edu.wpi.first.wpilibj.AnalogOutput;
import edu.wpi.first.wpilibj.CANJaguar;
import edu.wpi.first.wpilibj.DigitalInput;
import edu.wpi.first.wpilibj.DigitalOutput;
import edu.wpi.first.wpilibj.Gyro;
import edu.wpi.first.wpilibj.Jaguar;
import edu.wpi.first.wpilibj.Relay;
import edu.wpi.first.wpilibj.Servo;
import edu.wpi.first.wpilibj.Talon;
import edu.wpi.first.wpilibj.Victor;
import edu.wpi.first.wpilibj.fixtures.AnalogCrossConnectFixture;
import edu.wpi.first.wpilibj.fixtures.CANMotorEncoderFixture;
import edu.wpi.first.wpilibj.fixtures.DIOCrossConnectFixture;
import edu.wpi.first.wpilibj.fixtures.MotorEncoderFixture;
import edu.wpi.first.wpilibj.fixtures.RelayCrossConnectFxiture;
import edu.wpi.first.wpilibj.fixtures.TiltPanCameraFixture;
import edu.wpi.first.wpilibj.mockhardware.FakePotentiometerSource;

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
	public static final double MOTOR_STOP_TIME = 0.20;
	
	public static final int kTalonChannel = 10;
	public static final int kVictorChannel = 1;
	public static final int kJaguarChannel = 2;
	
	/*TiltPanCamera Channels */
	public static final int kGyroChannel = 0;
	public static final double kGyroSensitivity = 0.0134897058674;
	public static final int kTiltServoChannel = 9;
	public static final int kPanServoChannel = 8;
	

	/* PowerDistributionPanel channels */
	public static final int kJaguarPDPChannel = 6;
	public static final int kVictorPDPChannel = 8;
	public static final int kTalonPDPChannel = 11;
	public static final int kCANJaguarPDPChannel = 5;
	
	/* CAN ASSOICATED CHANNELS */
	public static final int kCANRelayPowerCycler = 1;
	public static final int kFakeJaguarPotentiometer = 1;
	public static final int kFakeJaguarForwardLimit = 16;
	public static final int kFakeJaguarReverseLimit = 17;
	public static final int kCANJaguarID = 2;

	//THESE MUST BE IN INCREMENTING ORDER
	public static final int DIOCrossConnectB2 = 9;
	public static final int DIOCrossConnectB1 = 8;
	public static final int DIOCrossConnectA2 = 7;
	public static final int DIOCrossConnectA1 = 6;

	/** The Singleton instance of the Test Bench */
	private static TestBench instance = null;

	/**
	 * The single constructor for the TestBench. This method is private in order
	 * to prevent multiple TestBench objects from being allocated
	 */
	protected TestBench() {
	}

	/**
	 * Constructs a new set of objects representing a connected set of Talon
	 * controlled Motors and an encoder
	 *
	 * @return a freshly allocated Talon, Encoder pair
	 */
	public MotorEncoderFixture<Talon> getTalonPair() {

		MotorEncoderFixture<Talon> talonPair = new MotorEncoderFixture<Talon>(){
			@Override
			protected Talon giveSpeedController() {
				return new Talon(kTalonChannel);
			}
			@Override
			protected DigitalInput giveDigitalInputA() {
				return new DigitalInput(0);
			}
			@Override
			protected DigitalInput giveDigitalInputB() {
				return new DigitalInput(1);
			}
			@Override
			public int getPDPChannel() {
				return kTalonPDPChannel;
			}
		};
		return talonPair;
	}

	/**
	 * Constructs a new set of objects representing a connected set of Victor
	 * controlled Motors and an encoder
	 *
	 * @return a freshly allocated Victor, Encoder pair
	 */
	public MotorEncoderFixture<Victor> getVictorPair() {

		MotorEncoderFixture<Victor> vicPair = new MotorEncoderFixture<Victor>(){
			@Override
			protected Victor giveSpeedController() {
				return new Victor(kVictorChannel);
			}

			@Override
			protected DigitalInput giveDigitalInputA() {
				return new DigitalInput(2);
			}

			@Override
			protected DigitalInput giveDigitalInputB() {
				return new DigitalInput(3);
			}
			@Override
			public int getPDPChannel() {
				return kVictorPDPChannel;
			}
		};
		return vicPair;
	}

	/**
	 * Constructs a new set of objects representing a connected set of Jaguar
	 * controlled Motors and an encoder
	 *
	 * @return a freshly allocated Jaguar, Encoder pair
	 */
	public MotorEncoderFixture<Jaguar> getJaguarPair() {
		MotorEncoderFixture<Jaguar> jagPair = new MotorEncoderFixture<Jaguar>(){
			@Override
			protected Jaguar giveSpeedController() {
				return new Jaguar(kJaguarChannel);
			}

			@Override
			protected DigitalInput giveDigitalInputA() {
				return new DigitalInput(4);
			}

			@Override
			protected DigitalInput giveDigitalInputB() {
				return new DigitalInput(5);
			}
			@Override
			public int getPDPChannel() {
				return kJaguarPDPChannel;
			}
		};
		return jagPair;
	}
	
	public class BaseCANMotorEncoderFixture extends CANMotorEncoderFixture{
		@Override
		protected CANJaguar giveSpeedController() {
			return new CANJaguar(kCANJaguarID);
		}
		@Override
		protected DigitalInput giveDigitalInputA() {
			return new DigitalInput(18);
		}
		@Override
		protected DigitalInput giveDigitalInputB() {
			return new DigitalInput(19);
		}
		@Override
		protected FakePotentiometerSource giveFakePotentiometerSource() {
			return new FakePotentiometerSource(kFakeJaguarPotentiometer, 360);
		}
		@Override
		protected DigitalOutput giveFakeForwardLimit() {
			return new DigitalOutput(kFakeJaguarForwardLimit);
		}
		@Override
		protected DigitalOutput giveFakeReverseLimit() {
			return new DigitalOutput(kFakeJaguarReverseLimit);
		}
		/* (non-Javadoc)
		 * @see edu.wpi.first.wpilibj.fixtures.CANMotorEncoderFixture#givePoweCycleRelay()
		 */
		@Override
		protected Relay givePoweCycleRelay() {
			return new Relay(kCANRelayPowerCycler);
		}
		
		@Override
		public int getPDPChannel() {
			return kCANJaguarPDPChannel;
		}
	}

	/**
	 * Constructs a new set of objects representing a connected set of CANJaguar
	 * controlled Motors and an encoder<br>
	 * Note: The CANJaguar is not freshly allocated because the CANJaguar lacks
	 * a free() method
	 *
	 * @return an existing CANJaguar and a freshly allocated Encoder
	 */
	public CANMotorEncoderFixture getCanJaguarPair() {
		CANMotorEncoderFixture canPair = new BaseCANMotorEncoderFixture();
		return canPair;
	}

	/**
	 * Constructs a new set of two Servo's and a Gyroscope.
	 *
	 * @return a freshly allocated Servo's and a freshly allocated Gyroscope
	 */
	public TiltPanCameraFixture getTiltPanCam() {
		TiltPanCameraFixture tpcam = new TiltPanCameraFixture(){
			@Override
			protected Gyro giveGyro() {
				Gyro gyro = new Gyro(kGyroChannel);
				gyro.setSensitivity(kGyroSensitivity);
				return gyro;
			}

			@Override
			protected Servo giveTilt() {
				return new Servo(kTiltServoChannel);
			}

			@Override
			protected Servo givePan() {
				return new Servo(kPanServoChannel);
			}
		};

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

	public static AnalogCrossConnectFixture getAnalogCrossConnectFixture(){
		AnalogCrossConnectFixture analogIO = new AnalogCrossConnectFixture() {
			@Override
			protected AnalogOutput giveAnalogOutput() {
				return new AnalogOutput(0);
			}

			@Override
			protected AnalogInput giveAnalogInput() {
				return new AnalogInput(2);
			}
		};
		return analogIO;
	}

	public static RelayCrossConnectFxiture getRelayCrossConnectFixture(){
		RelayCrossConnectFxiture relay = new RelayCrossConnectFxiture() {

			@Override
			protected Relay giveRelay() {
				return new Relay(0);
			}

			@Override
			protected DigitalInput giveInputTwo() {
				return new DigitalInput(14);
			}

			@Override
			protected DigitalInput giveInputOne() {
				return new DigitalInput(15);
			}
		};
		return relay;
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
	
	/**
	 * Provides access to the output stream for the test system. This should be used instead of System.out
	 * This is gives us a way to implement changes to where the output text of this test system is sent.
	 * @return The test bench global print stream.
	 */
	public static PrintStream out(){
		return System.out;
	}
	
	/**
	 * Provides access to the error stream for the test system. This should be used instead of System.err
	 * This is gives us a way to implement changes to where the output text of this test system is sent.
	 * @return The test bench global print stream.
	 */
	public static PrintStream err(){
		return System.err;
	}
}
