package edu.wpi.first.hal;

import org.junit.jupiter.params.ParameterizedTest;
import org.junit.jupiter.params.provider.Arguments;
import org.junit.jupiter.params.provider.MethodSource;

import java.util.Set;
import java.util.stream.Stream;

import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.params.provider.Arguments.arguments;

class REVPHFaultsTest {
  @MethodSource
  @ParameterizedTest
  void activeFaultsTest(int bitfield, Set<String> expected) {
    Set<String> activeFaults = new REVPHFaults(bitfield).getActiveFaults();
    assertEquals(expected, activeFaults);
  }

  static Stream<Arguments> activeFaultsTest() {
    return Stream.of(
            arguments(0, Set.of()),
            arguments(0x1, Set.of("Channel0Fault")),
            arguments(0x10000, Set.of("CompressorOverCurrent")),
            arguments(0x40 | 0x1000000, Set.of("CanWarning", "Channel6Fault")),
            arguments(0x200000, Set.of("HardwareFault")),
            arguments(0x400 | 0x1000, Set.of("Channel10Fault", "Channel12Fault"))
    );
  }
}
