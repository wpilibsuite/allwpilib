/*----------------------------------------------------------------------------*/
/* Copyright (c) 2020 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpiutil.math;

import edu.wpi.first.wpiutil.math.numbers.N2;
import org.ejml.simple.SimpleMatrix;
import org.junit.jupiter.api.Assertions;
import org.junit.jupiter.api.Test;

public class MatrixExpTest {
    @Test
    public void testMatrixExp() {

        Matrix<N2, N2> mat = new MatBuilder<>(Nat.N2(), Nat.N2()).fill(1.2, 5.6, 3.0, 4.0);
        var exped = Drake.exp(mat);

        Assertions.assertEquals(346.557, exped.get(0, 0), 0.1);
        Assertions.assertEquals(661.735, exped.get(0, 1), 0.1);
        Assertions.assertEquals(354.501, exped.get(1, 0), 0.1);
        Assertions.assertEquals(677.425, exped.get(1, 1), 0.1);
    }

}
