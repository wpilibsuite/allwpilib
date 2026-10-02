// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.math.autodiff;

import org.ejml.data.DMatrixSparseCSC;
import org.ejml.data.DMatrixSparseTriplet;
import org.ejml.ops.DConvertMatrixStruct;
import org.ejml.simple.SimpleMatrix;

/**
 * Wrapper for sparse matrix triplets from JNI.
 *
 * <p>We can't use DMatrixSparseTriplet because it doesn't have a method for bulk-initialization
 * from triplet arrays.
 *
 * @param rows Triplet rows.
 * @param cols Triplet columns.
 * @param values Triplet values.
 */
public record NativeSparseTriplets(int[] rows, int[] cols, double[] values) {
  /**
   * Returns a SimpleMatrix wrapper for this set of triplets.
   *
   * @param rows Number of rows in sparse SimpleMatrix.
   * @param cols Number of columns in sparse SimpleMatrix.
   * @return A SimpleMatrix wrapper for this set of triplets.
   */
  public SimpleMatrix toSimpleMatrix(int rows, int cols) {
    var ejmlTriplets = new DMatrixSparseTriplet(rows, cols, values().length);
    for (int i = 0; i < values().length; ++i) {
      ejmlTriplets.addItem(rows()[i], cols()[i], values()[i]);
    }
    return new SimpleMatrix(DConvertMatrixStruct.convert(ejmlTriplets, (DMatrixSparseCSC) null));
  }
}
