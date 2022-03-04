#include <Eigen/Core>

namespace drake::math {
/* helper functions */
void check_stabilizable(const Eigen::Ref<const Eigen::MatrixXd>& A,
                        const Eigen::Ref<const Eigen::MatrixXd>& B);
void check_detectable(const Eigen::Ref<const Eigen::MatrixXd>& A,
                      const Eigen::Ref<const Eigen::MatrixXd>& Q);

// "Givens rotation" computes an orthogonal 2x2 matrix R such that
// it eliminates the 2nd coordinate of the vector [a,b]', i.e.,
// R * [ a ] = [ a_hat ]
//     [ b ]   [   0   ]
// The implementation is based on
// https://en.wikipedia.org/wiki/Givens_rotation#Stable_calculation
void Givens_rotation(double a, double b, Eigen::Ref<Eigen::Matrix2d> R,
                     double eps = 1e-10);

// The arguments S, T, and Z will be changed.
void swap_block_11(Eigen::Ref<Eigen::MatrixXd> S, Eigen::Ref<Eigen::MatrixXd> T,
                   Eigen::Ref<Eigen::MatrixXd> Z, int p);

// The arguments S, T, and Z will be changed.
void swap_block_21(Eigen::Ref<Eigen::MatrixXd> S, Eigen::Ref<Eigen::MatrixXd> T,
                   Eigen::Ref<Eigen::MatrixXd> Z, int p);

// The arguments S, T, and Z will be changed.
void swap_block_12(Eigen::Ref<Eigen::MatrixXd> S, Eigen::Ref<Eigen::MatrixXd> T,
                   Eigen::Ref<Eigen::MatrixXd> Z, int p);

// The arguments S, T, and Z will be changed.
void swap_block_22(Eigen::Ref<Eigen::MatrixXd> S, Eigen::Ref<Eigen::MatrixXd> T,
                   Eigen::Ref<Eigen::MatrixXd> Z, int p);

// Functionality of "swap_block" function:
// swap the 1x1 or 2x2 blocks pointed by p and q.
// There are four cases: swapping 1x1 and 1x1 matrices, swapping 2x2 and 1x1
// matrices, swapping 1x1 and 2x2 matrices, and swapping 2x2 and 2x2 matrices.
// Algorithms are described in the papers
// "A generalized eigenvalue approach for solving Riccati equations" by P. Van
// Dooren, 1981 ( http://epubs.siam.org/doi/pdf/10.1137/0902010 ), and
// "Numerical Methods for General and Structured Eigenvalue Problems" by
// Daniel Kressner, 2005.
void swap_block(Eigen::Ref<Eigen::MatrixXd> S, Eigen::Ref<Eigen::MatrixXd> T,
                Eigen::Ref<Eigen::MatrixXd> Z, int p, int q, int q_block_size,
                double eps = 1e-10);

// Functionality of "reorder_eigen" function:
// Reorder the eigenvalues of (S,T) such that the top-left n by n matrix has
// stable eigenvalues by multiplying Q's and Z's on the left and the right,
// respectively.
// Stable eigenvalues are inside the unit disk.
//
// Algorithm:
// Go along the diagonals of (S,T) from the top left to the bottom right.
// Once find a stable eigenvalue, push it to top left.
// In implementation, use two pointers, p and q.
// p points to the current block (1x1 or 2x2) and q points to the block with the
// stable eigenvalue(s).
// Push the block pointed by q to the position pointed by p.
// Finish when n stable eigenvalues are placed at the top-left n by n matrix.
// The algorithm for swapping blocks is described in the papers
// "A generalized eigenvalue approach for solving Riccati equations" by P. Van
// Dooren, 1981, and "Numerical Methods for General and Structured Eigenvalue
// Problems" by Daniel Kressner, 2005.
void reorder_eigen(Eigen::Ref<Eigen::MatrixXd> S, Eigen::Ref<Eigen::MatrixXd> T,
                   Eigen::Ref<Eigen::MatrixXd> Z, double eps = 1e-10);
}  // namespace drake::math
