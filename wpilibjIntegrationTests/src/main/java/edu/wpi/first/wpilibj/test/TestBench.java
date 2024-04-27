// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj.test;

import edu.wpi.first.wpilibj.AnalogGyro;
import edu.wpi.first.wpilibj.AnalogInput;
import edu.wpi.first.wpilibj.AnalogOutput;
import edu.wpi.first.wpilibj.DigitalInput;
import edu.wpi.first.wpilibj.Relay;
import edu.wpi.first.wpilibj.Servo;
import edu.wpi.first.wpilibj.fixtures.AnalogCrossConnectFixture;
import edu.wpi.first.wpilibj.fixtures.DIOCrossConnectFixture;
import edu.wpi.first.wpilibj.fixtures.MotorEncoderFixture;
import edu.wpi.first.wpilibj.fixtures.RelayCrossConnectFixture;
import edu.wpi.first.wpilibj.fixtures.TiltPanCameraFixture;
import edu.wpi.first.wpilibj.motorcontrol.Jaguar;
import edu.wpi.first.wpilibj.motorcontrol.Talon;
import edu.wpi.first.wpilibj.motorcontrol.Victor;
import java.io.PrintStream;
import java.util.ArrayList;
import java.util.Collection;
import java.util.List;

/** This class provides access to all of the elements on the test bench, for use in fixtures. */
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

  /* PowerDistribution channels */
  public static final int kJaguarPDPChannel = 6;
  public static final int kVictorPDPChannel = 8;
  public static final int kTalonPDPChannel = 10;

  // THESE MUST BE IN INCREMENTING ORDER
  public static final int DIOCrossConnectB2 = 9;
  public static final int DIOCrossConnectB1 = 8;
  public static final int DIOCrossConnectA2 = 7;
  public static final int DIOCrossConnectA1 = 6;

  /** The Singleton instance of the Test Bench. */
  @SuppressWarnings("unused")
  private static TestBench instance = null;

  /**
   * The single constructor for the TestBench. This method is private in order to prevent multiple
   * TestBench objects from being allocated.
   */
  private TestBench() {}

  /**
   * Constructs a new set of objects representing a connected set of Talon controlled Motors and an
   * encoder.
   *
   * @return a freshly allocated Talon, Encoder pair
   */
  public static MotorEncoderFixture<Talon> getTalonPair() {
    return new MotorEncoderFixture<>() {
      @Override
      protected Talon giveMotorController() {
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
  }

  /**
   * Constructs a new set of objects representing a connected set of Victor controlled Motors and an
   * encoder.
   *
   * @return a freshly allocated Victor, Encoder pair
   */
  public static MotorEncoderFixture<Victor> getVictorPair() {
    return new MotorEncoderFixture<>() {
      @Override
      protected Victor giveMotorController() {
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
  }

  /**
   * Constructs a new set of objects representing a connected set of Jaguar controlled Motors and an
   * encoder.
   *
   * @return a freshly allocated Jaguar, Encoder pair
   */
  public static MotorEncoderFixture<Jaguar> getJaguarPair() {
    return new MotorEncoderFixture<>() {
      @Override
      protected Jaguar giveMotorController() {
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
  }

  /**
   * Constructs a new set of two Servo's and a Gyroscope.
   *
   * @return a freshly allocated Servo's and a freshly allocated Gyroscope
   */
  public static TiltPanCameraFixture getTiltPanCam() {
    return new TiltPanCameraFixture() {
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
  }

  public static DIOCrossConnectFixture getDIOCrossConnectFixture(int inputPort, int outputPort) {
    return new DIOCrossConnectFixture(inputPort, outputPort);
  }

  /** Gets two lists of possible DIO pairs for the two pairs. */
  private static List<List<Integer[]>> getDIOCrossConnect() {
    List<List<Integer[]>> pairs = new ArrayList<>();
    List<Integer[]> setA =
        List.of(
            new Integer[][] {
              {DIOCrossConnectA1, DIOCrossConnectA2},
              {DIOCrossConnectA2, DIOCrossConnectA1}
            });
    pairs.add(setA);

    List<Integer[]> setB =
        List.of(
            new Integer[][] {
              {DIOCrossConnectB1, DIOCrossConnectB2},
              {DIOCrossConnectB2, DIOCrossConnectB1}
            });
    pairs.add(setB);
    // NOTE: IF MORE DIOCROSSCONNECT PAIRS ARE ADDED ADD THEM HERE
    return pairs;
  }

  /** Returns the analog I/O cross-connect fixture. */
  public static AnalogCrossConnectFixture getAnalogCrossConnectFixture() {
    return new AnalogCrossConnectFixture() {
      @Override
      protected AnalogOutput giveAnalogOutput() {
        return new AnalogOutput(0);
      }

      @Override
      protected AnalogInput giveAnalogInput() {
        return new AnalogInput(2);
      }
    };
  }

  /** Returns the relay cross-connect fixture. */
  public static RelayCrossConnectFixture getRelayCrossConnectFixture() {
    return new RelayCrossConnectFixture() {
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
  }

  /**
   * Return a single Collection containing all of the DIOCrossConnectFixtures in all two pair
   * combinations.
   *
   * @return pairs of DIOCrossConnectFixtures
   */
  public static Collection<Integer[]> getDIOCrossConnectCollection() {
    Collection<Integer[]> pairs = new ArrayList<>();
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
  private static Collection<Integer[]> getPairArray(
      List<Integer[]> listA, List<Integer[]> listB, boolean flip) {
    Collection<Integer[]> encoderPortPairs = new ArrayList<>();
    for (Integer[] portPairsA : listA) {
      Integer[] inputs = new Integer[5];
      inputs[0] = portPairsA[0]; // InputA
      inputs[1] = portPairsA[1]; // InputB

      for (Integer[] portPairsB : listB) {
        inputs[2] = portPairsB[0]; // OutputA
        inputs[3] = portPairsB[1]; // OutputB
        inputs[4] = flip ? 0 : 1; // The flip bit
      }

      ArrayList<Integer[]> construtorInput = new ArrayList<>();
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
  public static Collection<Integer[]> getEncoderDIOCrossConnectCollection() {
    Collection<Integer[]> encoderPortPairs = new ArrayList<>();
    assert getDIOCrossConnect().size() == 2;
    encoderPortPairs.addAll(
        getPairArray(getDIOCrossConnect().get(0), getDIOCrossConnect().get(1), false));
    encoderPortPairs.addAll(
        getPairArray(getDIOCrossConnect().get(1), getDIOCrossConnect().get(0), false));
    assert encoderPortPairs.size() == 8;
    return encoderPortPairs;
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
