package edu.wpi.first.wpilibj.math;

import org.junit.Assert;
import org.junit.Test;

import java.io.IOException;

public class DrakeJNITest {

    @Test
    public void testLink() {
        try {
            DrakeJNI.forceLoad();
        } catch (IOException e) {
            e.printStackTrace();
            Assert.fail();
        }
    }
}
