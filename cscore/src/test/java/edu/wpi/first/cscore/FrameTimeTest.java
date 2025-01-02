// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.cscore;

import static org.junit.jupiter.api.Assertions.assertDoesNotThrow;
import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertTrue;

import edu.wpi.first.cscore.raw.RawSink;
import edu.wpi.first.cscore.raw.RawSource;
import edu.wpi.first.util.PixelFormat;
import edu.wpi.first.util.RawFrame;
import edu.wpi.first.util.TimestampSource;
import org.junit.jupiter.api.Test;

class FrameTimeTest {
  @Test
  void testFrameTimeRoundTrip() {
    // Given a Source
    RawSource source = new RawSource("foobar", PixelFormat.kBGR, 320, 240, 30);

    // And a sink connected to it
    RawSink sink = new RawSink("foobar2");
    sink.setSource(source);

    // And as new frame with a non-zero time
    RawFrame frame = new RawFrame();
    frame.setTimeInfo(12, TimestampSource.kV4lEoF);

    // And a sink waiting for a new frame to arrive
    RawFrame sinkFrame = new RawFrame();
    Thread sinkGrabber =
        new Thread(
            () -> {
              long ret = sink.grabFrame(sinkFrame);
              assertTrue(ret > 0);
            });
    sinkGrabber.start();

    // When a new frame is added to the source
    // (Note: the frame time and time source are overridden by RawSourceImpl, and changing this is
    // spooky)
    // (Matt: without this sleep, the test seemed flakey)
    assertDoesNotThrow(() -> Thread.sleep(10));
    source.putFrame(frame);

    // then the sink gets the image
    assertDoesNotThrow(() -> sinkGrabber.join());

    // and the image time makes sense
    assertTrue(sinkFrame.getTimestamp() > 0);
    assertEquals(TimestampSource.kFrameDequeue, sinkFrame.getTimestampSource());

    frame.close();
    sink.close();
    source.close();
  }
}
