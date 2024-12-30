#pragma once

// These are parameter variable scales. They have the units of the parameters
// themselves, so the optimizer sees x/SCALE_X for each parameter. I.e. as far
// as the optimizer is concerned, the scale of each variable is 1. This doesn't
// need to be precise; just need to get all the variables to be within the same
// order of magnitute. This is important because the dogleg solve treats the
// trust region as a ball in state space, and this ball is isotropic, and has a
// radius that applies in every direction
//
// Can be visualized like this:
//
//   b0,x0,J0 = mrcal.optimizer_callback(**optimization_inputs)[:3]
//   J0 = J0.toarray()
//   ss = np.sum(np.abs(J0), axis=-2)
//   gp.plot(ss, _set=mrcal.plotoptions_state_boundaries(**optimization_inputs))
//
// This visualizes the overall effect of each variable. If the scales aren't
// tuned properly, some variables will have orders of magnitude stronger
// response than others, and the optimization problem won't converge well.
//
// The scipy.optimize.least_squares() function claims to be able to estimate
// these automatically, without requiring these hard-coded values from the user.
// See the description of the "x_scale" argument:
//
//   https://docs.scipy.org/doc/scipy/reference/generated/scipy.optimize.least_squares.html
//
// Supposedly this paper describes the method:
//
//   J. J. More, "The Levenberg-Marquardt Algorithm: Implementation and Theory,"
//   Numerical Analysis, ed. G. A. Watson, Lecture Notes in Mathematics 630,
//   Springer Verlag, pp. 105-116, 1977.
//
// Please somebody look at this
#define SCALE_INTRINSICS_FOCAL_LENGTH 500.0
#define SCALE_INTRINSICS_CENTER_PIXEL 20.0
#define SCALE_ROTATION_CAMERA         (0.1 * M_PI/180.0)
#define SCALE_TRANSLATION_CAMERA      1.0
#define SCALE_ROTATION_FRAME          (15.0 * M_PI/180.0)
#define SCALE_TRANSLATION_FRAME       1.0
#define SCALE_POSITION_POINT          SCALE_TRANSLATION_FRAME
#define SCALE_CALOBJECT_WARP          0.01
#define SCALE_DISTORTION              1.0
