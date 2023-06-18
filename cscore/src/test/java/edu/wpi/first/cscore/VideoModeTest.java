package edu.wpi.first.cscore;

import org.junit.jupiter.api.Test;

import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertNotEquals;


public class VideoModeTest {
    @Test
    void equalityTest() {
        VideoMode a = new VideoMode(VideoMode.PixelFormat.kMJPEG, 1920, 1080, 30);
        VideoMode b = new VideoMode(VideoMode.PixelFormat.kMJPEG, 1920, 1080, 30);

        assertEquals(a, b);
        assertNotEquals(a, null);
    }
}
