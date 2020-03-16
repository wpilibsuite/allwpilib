package edu.wpi.first.wpilibj.math;

import edu.wpi.first.wpiutil.math.MatBuilder;
import edu.wpi.first.wpiutil.math.Matrix;
import edu.wpi.first.wpiutil.math.Nat;
import edu.wpi.first.wpiutil.math.numbers.N2;
import org.junit.Assert;
import org.junit.Test;


public class MatrixExpTest {

    @Test
    public void testMatrixExp() {

        Matrix<N2, N2> mat = new MatBuilder<>(Nat.N2(), Nat.N2()).fill(1.2, 5.6, 3.0, 4.0);
        var exped = StateSpaceUtils.exp(mat);

        Assert.assertEquals(346.557, exped.get(0, 0), 0.1);
        Assert.assertEquals(661.735, exped.get(0, 1), 0.1);
        Assert.assertEquals(354.501, exped.get(1, 0), 0.1);
        Assert.assertEquals(677.425, exped.get(1, 1), 0.1);
    }

}
