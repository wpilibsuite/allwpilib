#pragma once

#include "../_attribute.h"

// Extra functions I'm using in mrcal. I'm going to replace this whole library
// eventually, to make things nicer. These new functions will be a part of the
// replacement, and I'm not going to be thorough and I'm not going to add tests
// until I do that.

#define restrict __restrict

// Upper triangle is stored, in the usual row-major order.
MRCAL_ATTRIBUTE((unused))
static
int index_sym33(int i, int j)
{
    // In the top-right triangle I have i<=j, and
    //   index = N*i+j - sum(i, i=0..i)
    //         = N*i+j - (i+1)*i/2
    //         = (N*2 - i - 1)*i/2 + j
    const int N=3;
    if(i<=j) return (N*2-i-1)*i/2 + j;
    else     return (N*2-j-1)*j/2 + i;
}
MRCAL_ATTRIBUTE((unused))
static
int index_sym33_assume_upper(int i, int j)
{
    const int N=3;
    return (N*2-i-1)*i/2 + j;
}

// Upper triangle is stored, in the usual row-major order.
MRCAL_ATTRIBUTE((unused))
static
int index_sym66(int i, int j)
{
    // In the top-right triangle I have i<=j, and
    //   index = N*i+j - sum(i, i=0..i)
    //         = N*i+j - (i+1)*i/2
    //         = (N*2 - i - 1)*i/2 + j
    const int N=6;
    if(i<=j) return (N*2-i-1)*i/2 + j;
    else     return (N*2-j-1)*j/2 + i;
}
MRCAL_ATTRIBUTE((unused))
static
int index_sym66_assume_upper(int i, int j)
{
    const int N=6;
    return (N*2-i-1)*i/2 + j;
}
MRCAL_ATTRIBUTE((unused))
static
void mul_gen33_gen33insym66(// output
                            double* restrict P, int P_strideelems0, int P_strideelems1,
                            // input
                            const double* A, int A_strideelems0, int A_strideelems1,
                            const double* Bsym66, int B_i0, int B_j0,
                            const double scale)
{
    for(int iout=0; iout<3; iout++)
        for(int jout=0; jout<3; jout++)
        {
            P[iout*P_strideelems0 + jout*P_strideelems1] = 0;
            for(int k=0; k<3; k++)
            {
                P[iout*P_strideelems0 + jout*P_strideelems1] +=
                    A[iout*A_strideelems0 + k*A_strideelems1] *
                    Bsym66[index_sym66(k+B_i0, jout+B_j0)];
            }
            P[iout*P_strideelems0 + jout*P_strideelems1] *= scale;
        }
}
// Assumes the output is symmetric, and only computes the upper triangle
MRCAL_ATTRIBUTE((unused))
static
void mul_gen33_gen33_into33insym66_accum(// output
                                         double* restrict Psym66, int P_i0, int P_j0,
                                         // input
                                         const double* A, int A_strideelems0, int A_strideelems1,
                                         const double* B, int B_strideelems0, int B_strideelems1,
                                         const double scale)
{
    for(int iout=0; iout<3; iout++)
        for(int jout=0; jout<3; jout++)
        {
            if(jout + P_j0 < iout + P_i0)
            {
                // Wrong triangle. Set it up to look at the right triangle
                // during the next loop interation
                jout = iout + P_i0 - P_j0 - 1;
                continue;
            }

            for(int k=0; k<3; k++)
            {
                Psym66[index_sym66_assume_upper(iout+P_i0, jout+P_j0)] +=
                    A[iout*A_strideelems0 + k   *A_strideelems1] *
                    B[k   *B_strideelems0 + jout*B_strideelems1] * scale;
            }
        }
}
MRCAL_ATTRIBUTE((unused))
static
void set_gen33_from_gen33insym66_accum(// output
                                       double* restrict P, int P_strideelems0, int P_strideelems1,
                                       // input
                                       const double* Msym66, int M_i0, int M_j0,
                                       const double scale)
{
    for(int iout=0; iout<3; iout++)
        for(int jout=0; jout<3; jout++)
            P[iout*P_strideelems0 + jout*P_strideelems1] +=
                Msym66[index_sym66(iout+M_i0, jout+M_j0)] * scale;
}
// Assumes the output is symmetric, and only computes the upper triangle
MRCAL_ATTRIBUTE((unused))
static
void set_33insym66_from_gen33_accum(// output
                                    double* restrict Psym66, int P_i0, int P_j0,
                                    // input
                                    const double* M, int M_strideelems0, int M_strideelems1,
                                    const double scale)
{
    for(int iout=0; iout<3; iout++)
        for(int jout=0; jout<3; jout++)
        {
            if(jout + P_j0 < iout + P_i0)
            {
                // Wrong triangle. Set it up to look at the right triangle
                // during the next loop interation
                jout = iout + P_i0 - P_j0 - 1;
                continue;
            }

            Psym66[index_sym66_assume_upper(iout+P_i0, jout+P_j0)] +=
                M[iout*M_strideelems0 + jout*M_strideelems1] * scale;
        }
}

#define _MUL_CORE(doreset) do {                                         \
    for(int iout=0; iout<N; iout++)                                     \
    {                                                                   \
        for(int jout=0; jout<L; jout++)                                 \
        {                                                               \
            if(doreset) P[iout*P_strideelems0 + jout*P_strideelems1] = 0.0; \
            for(int k=0; k<M; k++)                                      \
                P[iout*P_strideelems0 + jout*P_strideelems1] +=         \
                    A[iout*A_strideelems0 + k   *A_strideelems1] *      \
                    B[k   *B_strideelems0 + jout*B_strideelems1] *      \
                    scale;                                              \
        }                                                               \
    }                                                                   \
} while(0)

// Matrix multiplication. Dimensions (N,L) <- (N,M) * (M,L)
MRCAL_ATTRIBUTE((unused))
static
void mul_genNM_genML(// output
                     double* restrict P, int P_strideelems0, int P_strideelems1,
                     // input
                     int N, int M, int L,
                     const double* restrict A, int A_strideelems0, int A_strideelems1,
                     const double* restrict B, int B_strideelems0, int B_strideelems1,
                     const double scale)
{
    _MUL_CORE(1);
}
MRCAL_ATTRIBUTE((unused))
static
void mul_genNM_genML_accum(// output
                           double* restrict P, int P_strideelems0, int P_strideelems1,
                           // input
                           int N, int M, int L,
                           const double* restrict A, int A_strideelems0, int A_strideelems1,
                           const double* restrict B, int B_strideelems0, int B_strideelems1,
                           const double scale)
{
    _MUL_CORE(0);
}
#undef _MUL_CORE

// Some common cases into convenient macros
#define mul_gen23_gen33(P,A,B,scale,ACCUM)  mul_genNM_genML ## ACCUM(P,3,1, 2,3,3, A,3,1, B,3,1, scale)
#define mul_gen33_gen33(P,A,B,scale,ACCUM)  mul_genNM_genML ## ACCUM(P,3,1, 3,3,3, A,3,1, B,3,1, scale)
#define mul_gen33t_gen33(P,A,B,scale,ACCUM) mul_genNM_genML ## ACCUM(P,3,1, 3,3,3, A,1,3, B,3,1, scale)
#define mul_gen33_gen33t(P,A,B,scale,ACCUM) mul_genNM_genML ## ACCUM(P,3,1, 3,3,3, A,3,1, B,1,3, scale)
#define mul_gen33_vec3(P,A,v,scale,ACCUM)   mul_genNM_genML ## ACCUM(P,1,1, 3,3,1, A,3,1, v,1,1, scale)
#define mul_gen33t_vec3(P,A,v,scale,ACCUM)  mul_genNM_genML ## ACCUM(P,1,1, 3,3,1, A,1,3, v,1,1, scale)
#define mul_vec3t_gen33(P,v,A,scale,ACCUM)  mul_genNM_genML ## ACCUM(P,3,1, 1,3,3, v,3,1, A,3,1, scale)
#define mul_vec3t_gen33t(P,v,A,scale,ACCUM) mul_genNM_genML ## ACCUM(P,3,1, 1,3,3, v,3,1, A,1,3, scale)


MRCAL_ATTRIBUTE((unused))
static inline void mul_vec6_sym66_scaled_strided(double* restrict v, int v_strideelems,
                                                 const double* restrict s,
                                                 const double scale)
{
  double t[5] = {v[0*v_strideelems], v[1*v_strideelems], v[2*v_strideelems], v[3*v_strideelems], v[4*v_strideelems]};
  v[0*v_strideelems] = (s[0]*t[0] + s[1]*t[1] + s[2]*t[2] + s[3]*t[3] + s[4]*t[4] + s[5]*v[5*v_strideelems]) * scale;
  v[1*v_strideelems] = (s[1]*t[0] + s[6]*t[1] + s[7]*t[2] + s[8]*t[3] + s[9]*t[4] + s[10]*v[5*v_strideelems]) * scale;
  v[2*v_strideelems] = (s[2]*t[0] + s[7]*t[1] + s[11]*t[2] + s[12]*t[3] + s[13]*t[4] + s[14]*v[5*v_strideelems]) * scale;
  v[3*v_strideelems] = (s[3]*t[0] + s[8]*t[1] + s[12]*t[2] + s[15]*t[3] + s[16]*t[4] + s[17]*v[5*v_strideelems]) * scale;
  v[4*v_strideelems] = (s[4]*t[0] + s[9]*t[1] + s[13]*t[2] + s[16]*t[3] + s[18]*t[4] + s[19]*v[5*v_strideelems]) * scale;
  v[5*v_strideelems] = (s[5]*t[0] + s[10]*t[1] + s[14]*t[2] + s[17]*t[3] + s[19]*t[4] + s[20]*v[5*v_strideelems]) * scale;
}

MRCAL_ATTRIBUTE((unused))
static inline void mul_genN6_sym66_scaled_strided(int n,
                                                  double* restrict v, int v_strideelems0, int v_strideelems1,
                                                  const double* restrict s,
                                                  const double scale)
{
  for(int i=0; i<n; i++)
      mul_vec6_sym66_scaled_strided(v + v_strideelems0*i, v_strideelems1,
                                    s,
                                    scale);
}
