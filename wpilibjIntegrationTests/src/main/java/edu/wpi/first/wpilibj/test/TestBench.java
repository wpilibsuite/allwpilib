/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008-2017. All Rights Reserved.                        */
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

import edu.wpi.first.wpilibj.AnalogGyro;
import edu.wpi.first.wpilibj.AnalogInput;
import edu.wpi.first.wpilibj.AnalogOutput;
import edu.wpi.first.wpilibj.DigitalInput;
import edu.wpi.first.wpilibj.Jaguar;
import edu.wpi.first.wpilibj.PIDSource;
import edu.wpi.first.wpilibj.Relay;
import edu.wpi.first.wpilibj.Servo;
import edu.wpi.first.wpilibj.Talon;
import edu.wpi.first.wpilibj.Victor;
import edu.wpi.first.wpilibj.filters.LinearDigitalFilter;
import edu.wpi.first.wpilibj.fixtures.AnalogCrossConnectFixture;
import edu.wpi.first.wpilibj.fixtures.DIOCrossConnectFixture;
import edu.wpi.first.wpilibj.fixtures.FilterNoiseFixture;
import edu.wpi.first.wpilibj.fixtures.FilterOutputFixture;
import edu.wpi.first.wpilibj.fixtures.MotorEncoderFixture;
import edu.wpi.first.wpilibj.fixtures.RelayCrossConnectFixture;
import edu.wpi.first.wpilibj.fixtures.TiltPanCameraFixture;

/**
 * This class provides access to all of the elements on the test bench, for use in fixtures. This
 * class is a singleton, you should use {@link #getInstance()} to obtain a reference to the {@code
 * TestBench}.
 */
public final class TestBench {

  /**
   * The time that it takes to have a motor go from rotating at full speed to completely stopped.
   */
  public static final double MOTOR_STOP_TIME = 0.25;

  public static final int kTalonChannel = 10;
  public static final int kVictorChannel = 1;
  public static final int kJaguarChannel = 2;

  /* TiltPanCamera Channels */
  public static final int kGyroChannel = 0;
  public static final double kGyroSensitivity = 0.013;
  public static final int kTiltServoChannel = 9;
  public static final int kPanServoChannel = 8;


  /* PowerDistributionPanel channels */
  public static final int kJaguarPDPChannel = 6;
  public static final int kVictorPDPChannel = 8;
  public static final int kTalonPDPChannel = 10;

  // THESE MUST BE IN INCREMENTING ORDER
  public static final int DIOCrossConnectB2 = 9;
  public static final int DIOCrossConnectB1 = 8;
  public static final int DIOCrossConnectA2 = 7;
  public static final int DIOCrossConnectA1 = 6;

  // Filter constants
  public static final double kStdDev = 10.0;
  public static final double kFilterStep = 0.005;
  public static final double kFilterTime = 2.0;
  public static final double kSinglePoleIIRTimeConstant = 0.015915;
  public static final double kSinglePoleIIRExpectedOutput = -3.2172003;
  public static final double kHighPassTimeConstant = 0.006631;
  public static final double kHighPassExpectedOutput = 10.074717;
  public static final int kMovAvgTaps = 6;
  public static final double kMovAvgExpectedOutput = -10.191644;

  /**
   * The Singleton instance of the Test Bench.
   */
  private static TestBench instance = null;

  /**
   * The single constructor for the TestBench. This method is private in order to prevent multiple
   * TestBench objects from being allocated.
   */
  protected TestBench() {
  }

  /**
   * Constructs a new set of objects representing a connected set of Talon controlled Motors and an
   * encoder.
   *
   * @return a freshly allocated Talon, Encoder pair
   */
  public MotorEncoderFixture<Talon> getTalonPair() {

    MotorEncoderFixture<Talon> talonPair = new MotorEncoderFixture<Talon>() {
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
   * Constructs a new set of objects representing a connected set of Victor controlled Motors and an
   * encoder.
   *
   * @return a freshly allocated Victor, Encoder pair
   */
  public MotorEncoderFixture<Victor> getVictorPair() {

    MotorEncoderFixture<Victor> vicPair = new MotorEncoderFixture<Victor>() {
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
   * Constructs a new set of objects representing a connected set of Jaguar controlled Motors and an
   * encoder.
   *
   * @return a freshly allocated Jaguar, Encoder pair
   */
  public MotorEncoderFixture<Jaguar> getJaguarPair() {
    MotorEncoderFixture<Jaguar> jagPair = new MotorEncoderFixture<Jaguar>() {
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

  /**
   * Constructs a new set of two Servo's and a Gyroscope.
   *
   * @return a freshly allocated Servo's and a freshly allocated Gyroscope
   */
  public TiltPanCameraFixture getTiltPanCam() {
    TiltPanCameraFixture tpcam = new TiltPanCameraFixture() {
      @Override
      protected AnalogGyro giveGyro() {
        AnalogGyro gyro = new AnalogGyro(kGyroChannel);
        gyro.setSensitivity(kGyroSensitivity);
        return gyro;
      }

      @Override
      protected AnalogGyro giveGyroParam(int center, double offset) {
        AnalogGyro gyro = new AnalogGyro(kGyroChannel, center, offset);
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

  public DIOCrossConnectFixture getDIOCrossConnectFixture(int inputPort, int outputPort) {
    DIOCrossConnectFixture dio = new DIOCrossConnectFixture(inputPort, outputPort);
    return dio;
  }

  /**
   * Gets two lists of possible DIO pairs for the two pairs.
   */
  private List<List<Integer[]>> getDIOCrossConnect() {
    List<List<Integer[]>> pairs = new ArrayList<List<Integer[]>>();
    List<Integer[]> setA =
        Arrays.asList(new Integer[][]{
            {new Integer(DIOCrossConnectA1), new Integer(DIOCrossConnectA2)},
            {new Integer(DIOCrossConnectA2), new Integer(DIOCrossConnectA1)}});
    pairs.add(setA);

    List<Integer[]> setB =
        Arrays.asList(new Integer[][]{
            {new Integer(DIOCrossConnectB1), new Integer(DIOCrossConnectB2)},
            {new Integer(DIOCrossConnectB2), new Integer(DIOCrossConnectB1)}});
    pairs.add(setB);
    // NOTE: IF MORE DIOCROSSCONNECT PAIRS ARE ADDED ADD THEM HERE
    return pairs;
  }

  @SuppressWarnings("JavadocMethod")
  public static AnalogCrossConnectFixture getAnalogCrossConnectFixture() {
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

  @SuppressWarnings("JavadocMethod")
  public static RelayCrossConnectFixture getRelayCrossConnectFixture() {
    RelayCrossConnectFixture relay = new RelayCrossConnectFixture() {

      @Override
      protected Relay giveRelay() {
        return new Relay(0);
      }

      @Override
      protected DigitalInput giveInputTwo() {
        return new DigitalInput(18);
      }

      @Override
      protected DigitalInput giveInputOne() {
        return new DigitalInput(19);
      }
    };
    return relay;
  }

  /**
   * Return a single Collection containing all of the DIOCrossConnectFixtures in all two pair
   * combinations.
   *
   * @return pairs of DIOCrossConnectFixtures
   */
  public Collection<Integer[]> getDIOCrossConnectCollection() {
    Collection<Integer[]> pairs = new ArrayList<Integer[]>();
    for (Collection<Integer[]> collection : getDIOCrossConnect()) {
      pairs.addAll(collection);
    }
    return pairs;
  }

  /**
   * Gets an array of pairs for the encoder to use using two different lists.
   *
   * @param flip whether this encoder needs to be flipped
   * @return A list of different inputs to use for the tests
   */
  private Collection<Integer[]> getPairArray(List<Integer[]> listA, List<Integer[]> listB,
                                             boolean flip) {
    Collection<Integer[]> encoderPortPairs = new ArrayList<Integer[]>();
    for (Integer[] portPairsA : listA) {
      Integer[] inputs = new Integer[5];
      inputs[0] = portPairsA[0]; // InputA
      inputs[1] = portPairsA[1]; // InputB

      for (Integer[] portPairsB : listB) {
        inputs[2] = portPairsB[0]; // OutputA
        inputs[3] = portPairsB[1]; // OutputB
        inputs[4] = flip ? 0 : 1; // The flip bit
      }

      ArrayList<Integer[]> construtorInput = new ArrayList<Integer[]>();
      construtorInput.add(inputs);

      inputs = inputs.clone();
      for (Integer[] portPairsB : listB) {
        inputs[2] = portPairsB[1]; // OutputA
        inputs[3] = portPairsB[0]; // OutputB
        inputs[4] = flip ? 0 : 1; // The flip bit
      }
      construtorInput.add(inputs);
      encoderPortPairs.addAll(construtorInput);
    }
    return encoderPortPairs;
  }

  /**
   * Constructs the list of inputs to be used for the encoder test.
   *
   * @return A collection of different input pairs to use for the encoder
   */
  public Collection<Integer[]> getEncoderDIOCrossConnectCollection() {
    Collection<Integer[]> encoderPortPairs = new ArrayList<Integer[]>();
    assert getDIOCrossConnect().size() == 2;
    encoderPortPairs.addAll(getPairArray(getDIOCrossConnect().get(0), getDIOCrossConnect().get(1),
        false));
    encoderPortPairs.addAll(getPairArray(getDIOCrossConnect().get(1), getDIOCrossConnect().get(0),
        false));
    assert (encoderPortPairs.size() == 8);
    return encoderPortPairs;
  }

  /**
   * Constructs a new set of objects representing a single-pole IIR filter with a noisy data source.
   *
   * @return a single-pole IIR filter with a noisy data source
   */
  public FilterNoiseFixture<LinearDigitalFilter> getSinglePoleIIRNoiseFixture() {
    return new FilterNoiseFixture<LinearDigitalFilter>() {
      @Override
      protected LinearDigitalFilter giveFilter(PIDSource source) {
        return LinearDigitalFilter.singlePoleIIR(source,
            kSinglePoleIIRTimeConstant,
            kFilterStep);
      }
    };
  }

  /**
   * Constructs a new set of objects representing a moving average filter with a noisy data source
   * using a linear digital filter.
   *
   * @return a moving average filter with a noisy data source
   */
  public FilterNoiseFixture<LinearDigitalFilter> getMovAvgNoiseFixture() {
    return new FilterNoiseFixture<LinearDigitalFilter>() {
      @Override
      protected LinearDigitalFilter giveFilter(PIDSource source) {
        return LinearDigitalFilter.movingAverage(source, kMovAvgTaps);
      }
    };
  }

  /**
   * Constructs a new set of objects representing a single-pole IIR filter with a repeatable data
   * source.
   *
   * @return a single-pole IIR filter with a repeatable data source
   */
  public FilterOutputFixture<LinearDigitalFilter> getSinglePoleIIROutputFixture() {
    return new FilterOutputFixture<LinearDigitalFilter>(kSinglePoleIIRExpectedOutput) {
      @Override
      protected LinearDigitalFilter giveFilter(PIDSource source) {
        return LinearDigitalFilter.singlePoleIIR(source,
            kSinglePoleIIRTimeConstant,
            kFilterStep);
      }
    };
  }

  /**
   * Constructs a new set of objects representing a high-pass filter with a repeatable data source.
   *
   * @return a high-pass filter with a repeatable data source
   */
  public FilterOutputFixture<LinearDigitalFilter> getHighPassOutputFixture() {
    return new FilterOutputFixture<LinearDigitalFilter>(kHighPassExpectedOutput) {
      @Override
      protected LinearDigitalFilter giveFilter(PIDSource source) {
        return LinearDigitalFilter.highPass(source, kHighPassTimeConstant,
            kFilterStep);
      }
    };
  }

  /**
   * Constructs a new set of objects representing a moving average filter with a repeatable data
   * source using a linear digital filter.
   *
   * @return a moving average filter with a repeatable data source
   */
  public FilterOutputFixture<LinearDigitalFilter> getMovAvgOutputFixture() {
    return new FilterOutputFixture<LinearDigitalFilter>(kMovAvgExpectedOutput) {
      @Override
      protected LinearDigitalFilter giveFilter(PIDSource source) {
        return LinearDigitalFilter.movingAverage(source, kMovAvgTaps);
      }
    };
  }

  /**
   * Gets the singleton of the TestBench. If the TestBench is not already allocated in constructs an
   * new instance of it. Otherwise it returns the existing instance.
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
   * Provides access to the output stream for the test system. This should be used instead of
   * System.out This is gives us a way to implement changes to where the output text of this test
   * system is sent.
   *
   * @return The test bench global print stream.
   */
  public static PrintStream out() {
    return System.out;
  }

  /**
   * Provides access to the error stream for the test system. This should be used instead of
   * System.err This is gives us a way to implement changes to where the output text of this test
   * system is sent.
   *
   * @return The test bench global print stream.
   */
  public static PrintStream err() {
    return System.err;
  }
}
