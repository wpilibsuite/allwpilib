#pragma once

// Extra functions I'm using in mrcal. I'm going to replace this whole library
// eventually, to make things nicer. These new functions will be a part of the
// replacement, and I'm not going to be thorough and I'm not going to add tests
// until I do that.

#define restrict __restrict

// Upper triangle is stored, in the usual row-major order.

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

static
int index_sym33_assume_upper(int i, int j)
{
    const int N=3;
    return (N*2-i-1)*i/2 + j;
}

// Upper triangle is stored, in the usual row-major order.

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

static
int index_sym66_assume_upper(int i, int j)
{
    const int N=6;
    return (N*2-i-1)*i/2 + j;
}

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

static
void set_gen33_from_gen33insym66(// output
                                 double* restrict P, int P_strideelems0, int P_strideelems1,
                                 // input
                                 const double* Msym66, int M_i0, int M_j0,
                                 const double scale)
{
    for(int iout=0; iout<3; iout++)
        for(int jout=0; jout<3; jout++)
            P[iout*P_strideelems0 + jout*P_strideelems1] =
                Msym66[index_sym66(iout+M_i0, jout+M_j0)] * scale;
}
// Assumes the output is symmetric, and only computes the upper triangle

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

// This is completely unreasonable. I'm almost certainly going to replace it

static
double cofactors_sym6(const double* restrict m, double* restrict c)
{
    /*
Just like in libminimath; adding 6x6 version. I use the maxima result verbatim, except:

- mNNN  -> m[NNN]
- XXX^2 -> XXX*XXX

maxima session:

(%i1) display2d : false;

(%i2) sym6 : matrix([m0,m1, m2,  m3, m4,  m5 ],
                    [m1,m6, m7,  m8, m9,  m10],
                    [m2,m7, m11, m12,m13, m14],
                    [m3,m8, m12, m15,m16, m17],
                    [m4,m9, m13, m16,m18, m19],
                    [m5,m10,m14, m17,m19, m20]);

(%i10) num( ev(invert(sym6),detout) );

(%o10) matrix([(-m7*((-m12*((-m16*(m20*m9-m10*m19))+m17*(m19*m9-m10*m18)
                                                   +(m18*m20-m19^2)*m8))
                    +m13*((-m15*(m20*m9-m10*m19))+m17*(m17*m9-m10*m16)
                                                 +(m16*m20-m17*m19)*m8)
                    -m14*((-m15*(m19*m9-m10*m18))+m16*(m17*m9-m10*m16)
                                                 +(m16*m19-m17*m18)*m8)
                    +(m15*(m18*m20-m19^2)-m16*(m16*m20-m17*m19)
                                         +m17*(m16*m19-m17*m18))
                     *m7))
                +m8*((-m11*((-m16*(m20*m9-m10*m19))+m17*(m19*m9-m10*m18)
                                                   +(m18*m20-m19^2)*m8))
                    +m13*((-m12*(m20*m9-m10*m19))+m17*(m14*m9-m10*m13)
                                                 +(m13*m20-m14*m19)*m8)
                    -m14*((-m12*(m19*m9-m10*m18))+m16*(m14*m9-m10*m13)
                                                 +(m13*m19-m14*m18)*m8)
                    +(m12*(m18*m20-m19^2)-m16*(m13*m20-m14*m19)
                                         +m17*(m13*m19-m14*m18))
                     *m7)
                -m9*((-m11*((-m15*(m20*m9-m10*m19))+m17*(m17*m9-m10*m16)
                                                   +(m16*m20-m17*m19)*m8))
                    +m12*((-m12*(m20*m9-m10*m19))+m17*(m14*m9-m10*m13)
                                                 +(m13*m20-m14*m19)*m8)
                    -m14*((-m12*(m17*m9-m10*m16))+m15*(m14*m9-m10*m13)
                                                 +(m13*m17-m14*m16)*m8)
                    +(m12*(m16*m20-m17*m19)-m15*(m13*m20-m14*m19)
                                           +m17*(m13*m17-m14*m16))
                     *m7)
                +m10*((-m11*((-m15*(m19*m9-m10*m18))+m16*(m17*m9-m10*m16)
                                                    +(m16*m19-m17*m18)*m8))
                     +m12*((-m12*(m19*m9-m10*m18))+m16*(m14*m9-m10*m13)
                                                  +(m13*m19-m14*m18)*m8)
                     -m13*((-m12*(m17*m9-m10*m16))+m15*(m14*m9-m10*m13)
                                                  +(m13*m17-m14*m16)*m8)
                     +(m12*(m16*m19-m17*m18)-m15*(m13*m19-m14*m18)
                                            +m16*(m13*m17-m14*m16))
                      *m7)
                +(m11*(m15*(m18*m20-m19^2)-m16*(m16*m20-m17*m19)
                                          +m17*(m16*m19-m17*m18))
                 -m12*(m12*(m18*m20-m19^2)-m16*(m13*m20-m14*m19)
                                          +m17*(m13*m19-m14*m18))
                 +m13*(m12*(m16*m20-m17*m19)-m15*(m13*m20-m14*m19)
                                            +m17*(m13*m17-m14*m16))
                 -m14*(m12*(m16*m19-m17*m18)-m15*(m13*m19-m14*m18)
                                            +m16*(m13*m17-m14*m16)))
                 *m6,
               m2*((-m12*((-m16*(m20*m9-m10*m19))+m17*(m19*m9-m10*m18)
                                                 +(m18*m20-m19^2)*m8))
                  +m13*((-m15*(m20*m9-m10*m19))+m17*(m17*m9-m10*m16)
                                               +(m16*m20-m17*m19)*m8)
                  -m14*((-m15*(m19*m9-m10*m18))+m16*(m17*m9-m10*m16)
                                               +(m16*m19-m17*m18)*m8)
                  +(m15*(m18*m20-m19^2)-m16*(m16*m20-m17*m19)
                                       +m17*(m16*m19-m17*m18))
                   *m7)
                -m3*((-m11*((-m16*(m20*m9-m10*m19))+m17*(m19*m9-m10*m18)
                                                   +(m18*m20-m19^2)*m8))
                    +m13*((-m12*(m20*m9-m10*m19))+m17*(m14*m9-m10*m13)
                                                 +(m13*m20-m14*m19)*m8)
                    -m14*((-m12*(m19*m9-m10*m18))+m16*(m14*m9-m10*m13)
                                                 +(m13*m19-m14*m18)*m8)
                    +(m12*(m18*m20-m19^2)-m16*(m13*m20-m14*m19)
                                         +m17*(m13*m19-m14*m18))
                     *m7)
                +m4*((-m11*((-m15*(m20*m9-m10*m19))+m17*(m17*m9-m10*m16)
                                                   +(m16*m20-m17*m19)*m8))
                    +m12*((-m12*(m20*m9-m10*m19))+m17*(m14*m9-m10*m13)
                                                 +(m13*m20-m14*m19)*m8)
                    -m14*((-m12*(m17*m9-m10*m16))+m15*(m14*m9-m10*m13)
                                                 +(m13*m17-m14*m16)*m8)
                    +(m12*(m16*m20-m17*m19)-m15*(m13*m20-m14*m19)
                                           +m17*(m13*m17-m14*m16))
                     *m7)
                -m5*((-m11*((-m15*(m19*m9-m10*m18))+m16*(m17*m9-m10*m16)
                                                   +(m16*m19-m17*m18)*m8))
                    +m12*((-m12*(m19*m9-m10*m18))+m16*(m14*m9-m10*m13)
                                                 +(m13*m19-m14*m18)*m8)
                    -m13*((-m12*(m17*m9-m10*m16))+m15*(m14*m9-m10*m13)
                                                 +(m13*m17-m14*m16)*m8)
                    +(m12*(m16*m19-m17*m18)-m15*(m13*m19-m14*m18)
                                           +m16*(m13*m17-m14*m16))
                     *m7)
                -m1*(m11*(m15*(m18*m20-m19^2)-m16*(m16*m20-m17*m19)
                                             +m17*(m16*m19-m17*m18))
                    -m12*(m12*(m18*m20-m19^2)-m16*(m13*m20-m14*m19)
                                             +m17*(m13*m19-m14*m18))
                    +m13*(m12*(m16*m20-m17*m19)-m15*(m13*m20-m14*m19)
                                               +m17*(m13*m17-m14*m16))
                    -m14*(m12*(m16*m19-m17*m18)-m15*(m13*m19-m14*m18)
                                               +m16*(m13*m17-m14*m16))),
               (-m2*((-m8*((-m16*(m20*m9-m10*m19))+m17*(m19*m9-m10*m18)
                                                  +(m18*m20-m19^2)*m8))
                    +m9*((-m15*(m20*m9-m10*m19))+m17*(m17*m9-m10*m16)
                                                +(m16*m20-m17*m19)*m8)
                    -m10*((-m15*(m19*m9-m10*m18))+m16*(m17*m9-m10*m16)
                                                 +(m16*m19-m17*m18)*m8)
                    +(m15*(m18*m20-m19^2)-m16*(m16*m20-m17*m19)
                                         +m17*(m16*m19-m17*m18))
                     *m6))
                +m3*((-m7*((-m16*(m20*m9-m10*m19))+m17*(m19*m9-m10*m18)
                                                  +(m18*m20-m19^2)*m8))
                    +m9*((-m12*(m20*m9-m10*m19))+m17*(m14*m9-m10*m13)
                                                +(m13*m20-m14*m19)*m8)
                    -m10*((-m12*(m19*m9-m10*m18))+m16*(m14*m9-m10*m13)
                                                 +(m13*m19-m14*m18)*m8)
                    +(m12*(m18*m20-m19^2)-m16*(m13*m20-m14*m19)
                                         +m17*(m13*m19-m14*m18))
                     *m6)
                -m4*((-m7*((-m15*(m20*m9-m10*m19))+m17*(m17*m9-m10*m16)
                                                  +(m16*m20-m17*m19)*m8))
                    +m8*((-m12*(m20*m9-m10*m19))+m17*(m14*m9-m10*m13)
                                                +(m13*m20-m14*m19)*m8)
                    -m10*((-m12*(m17*m9-m10*m16))+m15*(m14*m9-m10*m13)
                                                 +(m13*m17-m14*m16)*m8)
                    +(m12*(m16*m20-m17*m19)-m15*(m13*m20-m14*m19)
                                           +m17*(m13*m17-m14*m16))
                     *m6)
                +m5*((-m7*((-m15*(m19*m9-m10*m18))+m16*(m17*m9-m10*m16)
                                                  +(m16*m19-m17*m18)*m8))
                    +m8*((-m12*(m19*m9-m10*m18))+m16*(m14*m9-m10*m13)
                                                +(m13*m19-m14*m18)*m8)
                    -m9*((-m12*(m17*m9-m10*m16))+m15*(m14*m9-m10*m13)
                                                +(m13*m17-m14*m16)*m8)
                    +(m12*(m16*m19-m17*m18)-m15*(m13*m19-m14*m18)
                                           +m16*(m13*m17-m14*m16))
                     *m6)
                +m1*((m12*(m16*m20-m17*m19)-m15*(m13*m20-m14*m19)
                                           +m17*(m13*m17-m14*m16))
                    *m9
                    -(m12*(m18*m20-m19^2)-m16*(m13*m20-m14*m19)
                                         +m17*(m13*m19-m14*m18))
                     *m8
                    +(m15*(m18*m20-m19^2)-m16*(m16*m20-m17*m19)
                                         +m17*(m16*m19-m17*m18))
                     *m7
                    -m10*(m12*(m16*m19-m17*m18)-m15*(m13*m19-m14*m18)
                                               +m16*(m13*m17-m14*m16))),
               m2*((-m8*((-m13*(m20*m9-m10*m19))+m14*(m19*m9-m10*m18)
                                                +(m18*m20-m19^2)*m7))
                  +m9*((-m12*(m20*m9-m10*m19))+m14*(m17*m9-m10*m16)
                                              +(m16*m20-m17*m19)*m7)
                  -m10*((-m12*(m19*m9-m10*m18))+m13*(m17*m9-m10*m16)
                                               +(m16*m19-m17*m18)*m7)
                  +(m12*(m18*m20-m19^2)-m13*(m16*m20-m17*m19)
                                       +m14*(m16*m19-m17*m18))
                   *m6)
                -m3*((-m7*((-m13*(m20*m9-m10*m19))+m14*(m19*m9-m10*m18)
                                                  +(m18*m20-m19^2)*m7))
                    +m9*((-m11*(m20*m9-m10*m19))+m14*(m14*m9-m10*m13)
                                                +(m13*m20-m14*m19)*m7)
                    -m10*((-m11*(m19*m9-m10*m18))+m13*(m14*m9-m10*m13)
                                                 +(m13*m19-m14*m18)*m7)
                    +(m11*(m18*m20-m19^2)-m13*(m13*m20-m14*m19)
                                         +m14*(m13*m19-m14*m18))
                     *m6)
                +m4*((-m7*((-m12*(m20*m9-m10*m19))+m14*(m17*m9-m10*m16)
                                                  +(m16*m20-m17*m19)*m7))
                    +m8*((-m11*(m20*m9-m10*m19))+m14*(m14*m9-m10*m13)
                                                +(m13*m20-m14*m19)*m7)
                    -m10*((-m11*(m17*m9-m10*m16))+m12*(m14*m9-m10*m13)
                                                 +(m13*m17-m14*m16)*m7)
                    +(m11*(m16*m20-m17*m19)-m12*(m13*m20-m14*m19)
                                           +m14*(m13*m17-m14*m16))
                     *m6)
                -m5*((-m7*((-m12*(m19*m9-m10*m18))+m13*(m17*m9-m10*m16)
                                                  +(m16*m19-m17*m18)*m7))
                    +m8*((-m11*(m19*m9-m10*m18))+m13*(m14*m9-m10*m13)
                                                +(m13*m19-m14*m18)*m7)
                    -m9*((-m11*(m17*m9-m10*m16))+m12*(m14*m9-m10*m13)
                                                +(m13*m17-m14*m16)*m7)
                    +(m11*(m16*m19-m17*m18)-m12*(m13*m19-m14*m18)
                                           +m13*(m13*m17-m14*m16))
                     *m6)
                -m1*((m11*(m16*m20-m17*m19)-m12*(m13*m20-m14*m19)
                                           +m14*(m13*m17-m14*m16))
                    *m9
                    -(m11*(m18*m20-m19^2)-m13*(m13*m20-m14*m19)
                                         +m14*(m13*m19-m14*m18))
                     *m8
                    +(m12*(m18*m20-m19^2)-m13*(m16*m20-m17*m19)
                                         +m14*(m16*m19-m17*m18))
                     *m7
                    -m10*(m11*(m16*m19-m17*m18)-m12*(m13*m19-m14*m18)
                                               +m13*(m13*m17-m14*m16))),
               (-m2*(((-m12*(m20*m8-m10*m17))+m14*(m17*m8-m10*m15)
                                             +(m15*m20-m17^2)*m7)
                    *m9
                    -m8*((-m13*(m20*m8-m10*m17))+m14*(m19*m8-m10*m16)
                                                +(m16*m20-m17*m19)*m7)
                    -m10*((-m12*(m19*m8-m10*m16))+m13*(m17*m8-m10*m15)
                                                 +(m15*m19-m16*m17)*m7)
                    +(m12*(m16*m20-m17*m19)-m13*(m15*m20-m17^2)
                                           +m14*(m15*m19-m16*m17))
                     *m6))
                +m3*(((-m11*(m20*m8-m10*m17))+m14*(m14*m8-m10*m12)
                                             +(m12*m20-m14*m17)*m7)
                    *m9
                    -m7*((-m13*(m20*m8-m10*m17))+m14*(m19*m8-m10*m16)
                                                +(m16*m20-m17*m19)*m7)
                    -m10*((-m11*(m19*m8-m10*m16))+m13*(m14*m8-m10*m12)
                                                 +(m12*m19-m14*m16)*m7)
                    +(m11*(m16*m20-m17*m19)-m13*(m12*m20-m14*m17)
                                           +m14*(m12*m19-m14*m16))
                     *m6)
                +m5*((-((-m11*(m17*m8-m10*m15))+m12*(m14*m8-m10*m12)
                                               +(m12*m17-m14*m15)*m7)
                    *m9)
                    -m7*((-m12*(m19*m8-m10*m16))+m13*(m17*m8-m10*m15)
                                                +(m15*m19-m16*m17)*m7)
                    +m8*((-m11*(m19*m8-m10*m16))+m13*(m14*m8-m10*m12)
                                                +(m12*m19-m14*m16)*m7)
                    +(m11*(m15*m19-m16*m17)-m12*(m12*m19-m14*m16)
                                           +m13*(m12*m17-m14*m15))
                     *m6)
                +m1*((m11*(m15*m20-m17^2)-m12*(m12*m20-m14*m17)
                                         +m14*(m12*m17-m14*m15))
                    *m9
                    -(m11*(m16*m20-m17*m19)-m13*(m12*m20-m14*m17)
                                           +m14*(m12*m19-m14*m16))
                     *m8
                    +(m12*(m16*m20-m17*m19)-m13*(m15*m20-m17^2)
                                           +m14*(m15*m19-m16*m17))
                     *m7
                    -m10*(m11*(m15*m19-m16*m17)-m12*(m12*m19-m14*m16)
                                               +m13*(m12*m17-m14*m15)))
                -m4*((-m7*((-m12*(m20*m8-m10*m17))+m14*(m17*m8-m10*m15)
                                                  +(m15*m20-m17^2)*m7))
                    +m8*((-m11*(m20*m8-m10*m17))+m14*(m14*m8-m10*m12)
                                                +(m12*m20-m14*m17)*m7)
                    -m10*((-m11*(m17*m8-m10*m15))+m12*(m14*m8-m10*m12)
                                                 +(m12*m17-m14*m15)*m7)
                    +(m11*(m15*m20-m17^2)-m12*(m12*m20-m14*m17)
                                         +m14*(m12*m17-m14*m15))
                     *m6),
               m2*((-m8*((-m13*(m19*m8-m17*m9))+m14*(m18*m8-m16*m9)
                                               +(m16*m19-m17*m18)*m7))
                  +m9*((-m12*(m19*m8-m17*m9))+m14*(m16*m8-m15*m9)
                                             +(m15*m19-m16*m17)*m7)
                  -m10*((-m12*(m18*m8-m16*m9))+m13*(m16*m8-m15*m9)
                                              +(m15*m18-m16^2)*m7)
                  +(m12*(m16*m19-m17*m18)-m13*(m15*m19-m16*m17)
                                         +m14*(m15*m18-m16^2))
                   *m6)
                -m3*((-m7*((-m13*(m19*m8-m17*m9))+m14*(m18*m8-m16*m9)
                                                 +(m16*m19-m17*m18)*m7))
                    +m9*((-m11*(m19*m8-m17*m9))+m14*(m13*m8-m12*m9)
                                               +(m12*m19-m13*m17)*m7)
                    -m10*((-m11*(m18*m8-m16*m9))+m13*(m13*m8-m12*m9)
                                                +(m12*m18-m13*m16)*m7)
                    +(m11*(m16*m19-m17*m18)-m13*(m12*m19-m13*m17)
                                           +m14*(m12*m18-m13*m16))
                     *m6)
                +m4*((-m7*((-m12*(m19*m8-m17*m9))+m14*(m16*m8-m15*m9)
                                                 +(m15*m19-m16*m17)*m7))
                    +m8*((-m11*(m19*m8-m17*m9))+m14*(m13*m8-m12*m9)
                                               +(m12*m19-m13*m17)*m7)
                    -m10*((-m11*(m16*m8-m15*m9))+m12*(m13*m8-m12*m9)
                                                +(m12*m16-m13*m15)*m7)
                    +(m11*(m15*m19-m16*m17)-m12*(m12*m19-m13*m17)
                                           +m14*(m12*m16-m13*m15))
                     *m6)
                -m5*((-m7*((-m12*(m18*m8-m16*m9))+m13*(m16*m8-m15*m9)
                                                 +(m15*m18-m16^2)*m7))
                    +m8*((-m11*(m18*m8-m16*m9))+m13*(m13*m8-m12*m9)
                                               +(m12*m18-m13*m16)*m7)
                    -m9*((-m11*(m16*m8-m15*m9))+m12*(m13*m8-m12*m9)
                                               +(m12*m16-m13*m15)*m7)
                    +(m11*(m15*m18-m16^2)-m12*(m12*m18-m13*m16)
                                         +m13*(m12*m16-m13*m15))
                     *m6)
                -m1*((m11*(m15*m19-m16*m17)-m12*(m12*m19-m13*m17)
                                           +m14*(m12*m16-m13*m15))
                    *m9
                    -(m11*(m16*m19-m17*m18)-m13*(m12*m19-m13*m17)
                                           +m14*(m12*m18-m13*m16))
                     *m8
                    +(m12*(m16*m19-m17*m18)-m13*(m15*m19-m16*m17)
                                           +m14*(m15*m18-m16^2))
                     *m7
                    -m10*(m11*(m15*m18-m16^2)-m12*(m12*m18-m13*m16)
                                             +m13*(m12*m16-m13*m15)))],
              [((-m11*((-m15*(m20*m4-m19*m5))+m17*(m17*m4-m16*m5)
                                             +(m16*m20-m17*m19)*m3))
                +m12*((-m12*(m20*m4-m19*m5))+m17*(m14*m4-m13*m5)
                                            +(m13*m20-m14*m19)*m3)
                -m14*((-m12*(m17*m4-m16*m5))+m15*(m14*m4-m13*m5)
                                            +(m13*m17-m14*m16)*m3)
                +m2*(m12*(m16*m20-m17*m19)-m15*(m13*m20-m14*m19)
                                          +m17*(m13*m17-m14*m16)))
                *m9
                -((-m11*((-m16*(m20*m4-m19*m5))+m17*(m19*m4-m18*m5)
                                               +(m18*m20-m19^2)*m3))
                 +m13*((-m12*(m20*m4-m19*m5))+m17*(m14*m4-m13*m5)
                                             +(m13*m20-m14*m19)*m3)
                 -m14*((-m12*(m19*m4-m18*m5))+m16*(m14*m4-m13*m5)
                                             +(m13*m19-m14*m18)*m3)
                 +m2*(m12*(m18*m20-m19^2)-m16*(m13*m20-m14*m19)
                                         +m17*(m13*m19-m14*m18)))
                 *m8
                +((-m12*((-m16*(m20*m4-m19*m5))+m17*(m19*m4-m18*m5)
                                               +(m18*m20-m19^2)*m3))
                 +m13*((-m15*(m20*m4-m19*m5))+m17*(m17*m4-m16*m5)
                                             +(m16*m20-m17*m19)*m3)
                 -m14*((-m15*(m19*m4-m18*m5))+m16*(m17*m4-m16*m5)
                                             +(m16*m19-m17*m18)*m3)
                 +m2*(m15*(m18*m20-m19^2)-m16*(m16*m20-m17*m19)
                                         +m17*(m16*m19-m17*m18)))
                 *m7
                -m10*((-m11*((-m15*(m19*m4-m18*m5))+m16*(m17*m4-m16*m5)
                                                   +(m16*m19-m17*m18)*m3))
                     +m12*((-m12*(m19*m4-m18*m5))+m16*(m14*m4-m13*m5)
                                                 +(m13*m19-m14*m18)*m3)
                     -m13*((-m12*(m17*m4-m16*m5))+m15*(m14*m4-m13*m5)
                                                 +(m13*m17-m14*m16)*m3)
                     +(m12*(m16*m19-m17*m18)-m15*(m13*m19-m14*m18)
                                            +m16*(m13*m17-m14*m16))
                      *m2)
                -m1*(m11*(m15*(m18*m20-m19^2)-m16*(m16*m20-m17*m19)
                                             +m17*(m16*m19-m17*m18))
                    -m12*(m12*(m18*m20-m19^2)-m16*(m13*m20-m14*m19)
                                             +m17*(m13*m19-m14*m18))
                    +m13*(m12*(m16*m20-m17*m19)-m15*(m13*m20-m14*m19)
                                               +m17*(m13*m17-m14*m16))
                    -m14*(m12*(m16*m19-m17*m18)-m15*(m13*m19-m14*m18)
                                               +m16*(m13*m17-m14*m16))),
               (-m2*((-m12*((-m16*(m20*m4-m19*m5))+m17*(m19*m4-m18*m5)
                                                  +(m18*m20-m19^2)*m3))
                    +m13*((-m15*(m20*m4-m19*m5))+m17*(m17*m4-m16*m5)
                                                +(m16*m20-m17*m19)*m3)
                    -m14*((-m15*(m19*m4-m18*m5))+m16*(m17*m4-m16*m5)
                                                +(m16*m19-m17*m18)*m3)
                    +m2*(m15*(m18*m20-m19^2)-m16*(m16*m20-m17*m19)
                                            +m17*(m16*m19-m17*m18))))
                +m3*((-m11*((-m16*(m20*m4-m19*m5))+m17*(m19*m4-m18*m5)
                                                  +(m18*m20-m19^2)*m3))
                    +m13*((-m12*(m20*m4-m19*m5))+m17*(m14*m4-m13*m5)
                                                +(m13*m20-m14*m19)*m3)
                    -m14*((-m12*(m19*m4-m18*m5))+m16*(m14*m4-m13*m5)
                                                +(m13*m19-m14*m18)*m3)
                    +m2*(m12*(m18*m20-m19^2)-m16*(m13*m20-m14*m19)
                                            +m17*(m13*m19-m14*m18)))
                -m4*((-m11*((-m15*(m20*m4-m19*m5))+m17*(m17*m4-m16*m5)
                                                  +(m16*m20-m17*m19)*m3))
                    +m12*((-m12*(m20*m4-m19*m5))+m17*(m14*m4-m13*m5)
                                                +(m13*m20-m14*m19)*m3)
                    -m14*((-m12*(m17*m4-m16*m5))+m15*(m14*m4-m13*m5)
                                                +(m13*m17-m14*m16)*m3)
                    +m2*(m12*(m16*m20-m17*m19)-m15*(m13*m20-m14*m19)
                                              +m17*(m13*m17-m14*m16)))
                +m5*((-m11*((-m15*(m19*m4-m18*m5))+m16*(m17*m4-m16*m5)
                                                  +(m16*m19-m17*m18)*m3))
                    +m12*((-m12*(m19*m4-m18*m5))+m16*(m14*m4-m13*m5)
                                                +(m13*m19-m14*m18)*m3)
                    -m13*((-m12*(m17*m4-m16*m5))+m15*(m14*m4-m13*m5)
                                                +(m13*m17-m14*m16)*m3)
                    +(m12*(m16*m19-m17*m18)-m15*(m13*m19-m14*m18)
                                           +m16*(m13*m17-m14*m16))
                     *m2)
                +m0*(m11*(m15*(m18*m20-m19^2)-m16*(m16*m20-m17*m19)
                                             +m17*(m16*m19-m17*m18))
                    -m12*(m12*(m18*m20-m19^2)-m16*(m13*m20-m14*m19)
                                             +m17*(m13*m19-m14*m18))
                    +m13*(m12*(m16*m20-m17*m19)-m15*(m13*m20-m14*m19)
                                               +m17*(m13*m17-m14*m16))
                    -m14*(m12*(m16*m19-m17*m18)-m15*(m13*m19-m14*m18)
                                               +m16*(m13*m17-m14*m16))),
               m2*(((-m15*(m20*m4-m19*m5))+m17*(m17*m4-m16*m5)
                                          +(m16*m20-m17*m19)*m3)
                  *m9
                  -((-m16*(m20*m4-m19*m5))+m17*(m19*m4-m18*m5)
                                          +(m18*m20-m19^2)*m3)
                   *m8
                  -m10*((-m15*(m19*m4-m18*m5))+m16*(m17*m4-m16*m5)
                                              +(m16*m19-m17*m18)*m3)
                  +m1*(m15*(m18*m20-m19^2)-m16*(m16*m20-m17*m19)
                                          +m17*(m16*m19-m17*m18)))
                -m3*(((-m12*(m20*m4-m19*m5))+m17*(m14*m4-m13*m5)
                                            +(m13*m20-m14*m19)*m3)
                    *m9
                    -((-m16*(m20*m4-m19*m5))+m17*(m19*m4-m18*m5)
                                            +(m18*m20-m19^2)*m3)
                     *m7
                    -m10*((-m12*(m19*m4-m18*m5))+m16*(m14*m4-m13*m5)
                                                +(m13*m19-m14*m18)*m3)
                    +m1*(m12*(m18*m20-m19^2)-m16*(m13*m20-m14*m19)
                                            +m17*(m13*m19-m14*m18)))
                -m5*((-((-m12*(m17*m4-m16*m5))+m15*(m14*m4-m13*m5)
                                              +(m13*m17-m14*m16)*m3)
                    *m9)
                    +((-m12*(m19*m4-m18*m5))+m16*(m14*m4-m13*m5)
                                            +(m13*m19-m14*m18)*m3)
                     *m8
                    -((-m15*(m19*m4-m18*m5))+m16*(m17*m4-m16*m5)
                                            +(m16*m19-m17*m18)*m3)
                     *m7
                    +m1*(m12*(m16*m19-m17*m18)-m15*(m13*m19-m14*m18)
                                              +m16*(m13*m17-m14*m16)))
                -m0*((m12*(m16*m20-m17*m19)-m15*(m13*m20-m14*m19)
                                           +m17*(m13*m17-m14*m16))
                    *m9
                    -(m12*(m18*m20-m19^2)-m16*(m13*m20-m14*m19)
                                         +m17*(m13*m19-m14*m18))
                     *m8
                    +(m15*(m18*m20-m19^2)-m16*(m16*m20-m17*m19)
                                         +m17*(m16*m19-m17*m18))
                     *m7
                    -m10*(m12*(m16*m19-m17*m18)-m15*(m13*m19-m14*m18)
                                               +m16*(m13*m17-m14*m16)))
                +m4*(((-m12*(m20*m4-m19*m5))+m17*(m14*m4-m13*m5)
                                            +(m13*m20-m14*m19)*m3)
                    *m8
                    -((-m15*(m20*m4-m19*m5))+m17*(m17*m4-m16*m5)
                                            +(m16*m20-m17*m19)*m3)
                     *m7
                    -m10*((-m12*(m17*m4-m16*m5))+m15*(m14*m4-m13*m5)
                                                +(m13*m17-m14*m16)*m3)
                    +m1*(m12*(m16*m20-m17*m19)-m15*(m13*m20-m14*m19)
                                              +m17*(m13*m17-m14*m16))),
               (-m2*(((-m12*(m20*m4-m19*m5))+m14*(m17*m4-m16*m5)
                                            +m2*(m16*m20-m17*m19))
                    *m9
                    -((-m13*(m20*m4-m19*m5))+m14*(m19*m4-m18*m5)
                                            +m2*(m18*m20-m19^2))
                     *m8
                    -m10*((-m12*(m19*m4-m18*m5))+m13*(m17*m4-m16*m5)
                                                +(m16*m19-m17*m18)*m2)
                    +m1*(m12*(m18*m20-m19^2)-m13*(m16*m20-m17*m19)
                                            +m14*(m16*m19-m17*m18))))
                +m3*(((-m11*(m20*m4-m19*m5))+m14*(m14*m4-m13*m5)
                                            +m2*(m13*m20-m14*m19))
                    *m9
                    -((-m13*(m20*m4-m19*m5))+m14*(m19*m4-m18*m5)
                                            +m2*(m18*m20-m19^2))
                     *m7
                    -m10*((-m11*(m19*m4-m18*m5))+m13*(m14*m4-m13*m5)
                                                +(m13*m19-m14*m18)*m2)
                    +m1*(m11*(m18*m20-m19^2)-m13*(m13*m20-m14*m19)
                                            +m14*(m13*m19-m14*m18)))
                +m5*((-((-m11*(m17*m4-m16*m5))+m12*(m14*m4-m13*m5)
                                              +(m13*m17-m14*m16)*m2)
                    *m9)
                    +((-m11*(m19*m4-m18*m5))+m13*(m14*m4-m13*m5)
                                            +(m13*m19-m14*m18)*m2)
                     *m8
                    -((-m12*(m19*m4-m18*m5))+m13*(m17*m4-m16*m5)
                                            +(m16*m19-m17*m18)*m2)
                     *m7
                    +m1*(m11*(m16*m19-m17*m18)-m12*(m13*m19-m14*m18)
                                              +m13*(m13*m17-m14*m16)))
                +m0*((m11*(m16*m20-m17*m19)-m12*(m13*m20-m14*m19)
                                           +m14*(m13*m17-m14*m16))
                    *m9
                    -(m11*(m18*m20-m19^2)-m13*(m13*m20-m14*m19)
                                         +m14*(m13*m19-m14*m18))
                     *m8
                    +(m12*(m18*m20-m19^2)-m13*(m16*m20-m17*m19)
                                         +m14*(m16*m19-m17*m18))
                     *m7
                    -m10*(m11*(m16*m19-m17*m18)-m12*(m13*m19-m14*m18)
                                               +m13*(m13*m17-m14*m16)))
                -m4*(((-m11*(m20*m4-m19*m5))+m14*(m14*m4-m13*m5)
                                            +m2*(m13*m20-m14*m19))
                    *m8
                    -((-m12*(m20*m4-m19*m5))+m14*(m17*m4-m16*m5)
                                            +m2*(m16*m20-m17*m19))
                     *m7
                    -m10*((-m11*(m17*m4-m16*m5))+m12*(m14*m4-m13*m5)
                                                +(m13*m17-m14*m16)*m2)
                    +m1*(m11*(m16*m20-m17*m19)-m12*(m13*m20-m14*m19)
                                              +m14*(m13*m17-m14*m16))),
               m2*(((-m12*(m20*m3-m17*m5))+m14*(m17*m3-m15*m5)
                                          +m2*(m15*m20-m17^2))
                  *m9
                  -((-m13*(m20*m3-m17*m5))+m14*(m19*m3-m16*m5)
                                          +m2*(m16*m20-m17*m19))
                   *m8
                  -m10*((-m12*(m19*m3-m16*m5))+m13*(m17*m3-m15*m5)
                                              +(m15*m19-m16*m17)*m2)
                  +m1*(m12*(m16*m20-m17*m19)-m13*(m15*m20-m17^2)
                                            +m14*(m15*m19-m16*m17)))
                -m3*(((-m11*(m20*m3-m17*m5))+m14*(m14*m3-m12*m5)
                                            +m2*(m12*m20-m14*m17))
                    *m9
                    -((-m13*(m20*m3-m17*m5))+m14*(m19*m3-m16*m5)
                                            +m2*(m16*m20-m17*m19))
                     *m7
                    -m10*((-m11*(m19*m3-m16*m5))+m13*(m14*m3-m12*m5)
                                                +(m12*m19-m14*m16)*m2)
                    +m1*(m11*(m16*m20-m17*m19)-m13*(m12*m20-m14*m17)
                                              +m14*(m12*m19-m14*m16)))
                -m5*((-((-m11*(m17*m3-m15*m5))+m12*(m14*m3-m12*m5)
                                              +(m12*m17-m14*m15)*m2)
                    *m9)
                    +((-m11*(m19*m3-m16*m5))+m13*(m14*m3-m12*m5)
                                            +(m12*m19-m14*m16)*m2)
                     *m8
                    -((-m12*(m19*m3-m16*m5))+m13*(m17*m3-m15*m5)
                                            +(m15*m19-m16*m17)*m2)
                     *m7
                    +m1*(m11*(m15*m19-m16*m17)-m12*(m12*m19-m14*m16)
                                              +m13*(m12*m17-m14*m15)))
                -m0*((m11*(m15*m20-m17^2)-m12*(m12*m20-m14*m17)
                                         +m14*(m12*m17-m14*m15))
                    *m9
                    -(m11*(m16*m20-m17*m19)-m13*(m12*m20-m14*m17)
                                           +m14*(m12*m19-m14*m16))
                     *m8
                    +(m12*(m16*m20-m17*m19)-m13*(m15*m20-m17^2)
                                           +m14*(m15*m19-m16*m17))
                     *m7
                    -m10*(m11*(m15*m19-m16*m17)-m12*(m12*m19-m14*m16)
                                               +m13*(m12*m17-m14*m15)))
                +m4*(((-m11*(m20*m3-m17*m5))+m14*(m14*m3-m12*m5)
                                            +m2*(m12*m20-m14*m17))
                    *m8
                    -((-m12*(m20*m3-m17*m5))+m14*(m17*m3-m15*m5)
                                            +m2*(m15*m20-m17^2))
                     *m7
                    -m10*((-m11*(m17*m3-m15*m5))+m12*(m14*m3-m12*m5)
                                                +(m12*m17-m14*m15)*m2)
                    +m1*(m11*(m15*m20-m17^2)-m12*(m12*m20-m14*m17)
                                            +m14*(m12*m17-m14*m15))),
               (-m2*(((-m12*(m19*m3-m17*m4))+m14*(m16*m3-m15*m4)
                                            +(m15*m19-m16*m17)*m2)
                    *m9
                    -((-m13*(m19*m3-m17*m4))+m14*(m18*m3-m16*m4)
                                            +(m16*m19-m17*m18)*m2)
                     *m8
                    -m10*((-m12*(m18*m3-m16*m4))+m13*(m16*m3-m15*m4)
                                                +(m15*m18-m16^2)*m2)
                    +m1*(m12*(m16*m19-m17*m18)-m13*(m15*m19-m16*m17)
                                              +m14*(m15*m18-m16^2))))
                +m3*(((-m11*(m19*m3-m17*m4))+m14*(m13*m3-m12*m4)
                                            +(m12*m19-m13*m17)*m2)
                    *m9
                    -((-m13*(m19*m3-m17*m4))+m14*(m18*m3-m16*m4)
                                            +(m16*m19-m17*m18)*m2)
                     *m7
                    -m10*((-m11*(m18*m3-m16*m4))+m13*(m13*m3-m12*m4)
                                                +(m12*m18-m13*m16)*m2)
                    +m1*(m11*(m16*m19-m17*m18)-m13*(m12*m19-m13*m17)
                                              +m14*(m12*m18-m13*m16)))
                +m5*((-((-m11*(m16*m3-m15*m4))+m12*(m13*m3-m12*m4)
                                              +(m12*m16-m13*m15)*m2)
                    *m9)
                    +((-m11*(m18*m3-m16*m4))+m13*(m13*m3-m12*m4)
                                            +(m12*m18-m13*m16)*m2)
                     *m8
                    -((-m12*(m18*m3-m16*m4))+m13*(m16*m3-m15*m4)
                                            +(m15*m18-m16^2)*m2)
                     *m7
                    +m1*(m11*(m15*m18-m16^2)-m12*(m12*m18-m13*m16)
                                            +m13*(m12*m16-m13*m15)))
                +m0*((m11*(m15*m19-m16*m17)-m12*(m12*m19-m13*m17)
                                           +m14*(m12*m16-m13*m15))
                    *m9
                    -(m11*(m16*m19-m17*m18)-m13*(m12*m19-m13*m17)
                                           +m14*(m12*m18-m13*m16))
                     *m8
                    +(m12*(m16*m19-m17*m18)-m13*(m15*m19-m16*m17)
                                           +m14*(m15*m18-m16^2))
                     *m7
                    -m10*(m11*(m15*m18-m16^2)-m12*(m12*m18-m13*m16)
                                             +m13*(m12*m16-m13*m15)))
                -m4*(((-m11*(m19*m3-m17*m4))+m14*(m13*m3-m12*m4)
                                            +(m12*m19-m13*m17)*m2)
                    *m8
                    -((-m12*(m19*m3-m17*m4))+m14*(m16*m3-m15*m4)
                                            +(m15*m19-m16*m17)*m2)
                     *m7
                    -m10*((-m11*(m16*m3-m15*m4))+m12*(m13*m3-m12*m4)
                                                +(m12*m16-m13*m15)*m2)
                    +m1*(m11*(m15*m19-m16*m17)-m12*(m12*m19-m13*m17)
                                              +m14*(m12*m16-m13*m15)))],
              [m8*(m13*(m17*(m10*m4-m5*m9)+m3*(m20*m9-m10*m19)
                                          -(m20*m4-m19*m5)*m8)
                  -m14*(m16*(m10*m4-m5*m9)+m3*(m19*m9-m10*m18)
                                          -(m19*m4-m18*m5)*m8)
                  +m2*((-m16*(m20*m9-m10*m19))+m17*(m19*m9-m10*m18)
                                              +(m18*m20-m19^2)*m8)
                  -((-m16*(m20*m4-m19*m5))+m17*(m19*m4-m18*m5)
                                          +(m18*m20-m19^2)*m3)
                   *m7)
                -m9*(m12*(m17*(m10*m4-m5*m9)+m3*(m20*m9-m10*m19)
                                            -(m20*m4-m19*m5)*m8)
                    -m14*(m15*(m10*m4-m5*m9)+m3*(m17*m9-m10*m16)
                                            -(m17*m4-m16*m5)*m8)
                    +m2*((-m15*(m20*m9-m10*m19))+m17*(m17*m9-m10*m16)
                                                +(m16*m20-m17*m19)*m8)
                    -((-m15*(m20*m4-m19*m5))+m17*(m17*m4-m16*m5)
                                            +(m16*m20-m17*m19)*m3)
                     *m7)
                +m10*(m12*(m16*(m10*m4-m5*m9)+m3*(m19*m9-m10*m18)
                                             -(m19*m4-m18*m5)*m8)
                     -m13*(m15*(m10*m4-m5*m9)+m3*(m17*m9-m10*m16)
                                             -(m17*m4-m16*m5)*m8)
                     +m2*((-m15*(m19*m9-m10*m18))+m16*(m17*m9-m10*m16)
                                                 +(m16*m19-m17*m18)*m8)
                     -((-m15*(m19*m4-m18*m5))+m16*(m17*m4-m16*m5)
                                             +(m16*m19-m17*m18)*m3)
                      *m7)
                +m1*((-m12*((-m16*(m20*m9-m10*m19))+m17*(m19*m9-m10*m18)
                                                   +(m18*m20-m19^2)*m8))
                    +m13*((-m15*(m20*m9-m10*m19))+m17*(m17*m9-m10*m16)
                                                 +(m16*m20-m17*m19)*m8)
                    -m14*((-m15*(m19*m9-m10*m18))+m16*(m17*m9-m10*m16)
                                                 +(m16*m19-m17*m18)*m8)
                    +(m15*(m18*m20-m19^2)-m16*(m16*m20-m17*m19)
                                         +m17*(m16*m19-m17*m18))
                     *m7)
                -((-m12*((-m16*(m20*m4-m19*m5))+m17*(m19*m4-m18*m5)
                                               +(m18*m20-m19^2)*m3))
                 +m13*((-m15*(m20*m4-m19*m5))+m17*(m17*m4-m16*m5)
                                             +(m16*m20-m17*m19)*m3)
                 -m14*((-m15*(m19*m4-m18*m5))+m16*(m17*m4-m16*m5)
                                             +(m16*m19-m17*m18)*m3)
                 +m2*(m15*(m18*m20-m19^2)-m16*(m16*m20-m17*m19)
                                         +m17*(m16*m19-m17*m18)))
                 *m6,
               (-m3*(m13*(m17*(m10*m4-m5*m9)+m3*(m20*m9-m10*m19)
                                            -(m20*m4-m19*m5)*m8)
                    -m14*(m16*(m10*m4-m5*m9)+m3*(m19*m9-m10*m18)
                                            -(m19*m4-m18*m5)*m8)
                    +m2*((-m16*(m20*m9-m10*m19))+m17*(m19*m9-m10*m18)
                                                +(m18*m20-m19^2)*m8)
                    -((-m16*(m20*m4-m19*m5))+m17*(m19*m4-m18*m5)
                                            +(m18*m20-m19^2)*m3)
                     *m7))
                +m4*(m12*(m17*(m10*m4-m5*m9)+m3*(m20*m9-m10*m19)
                                            -(m20*m4-m19*m5)*m8)
                    -m14*(m15*(m10*m4-m5*m9)+m3*(m17*m9-m10*m16)
                                            -(m17*m4-m16*m5)*m8)
                    +m2*((-m15*(m20*m9-m10*m19))+m17*(m17*m9-m10*m16)
                                                +(m16*m20-m17*m19)*m8)
                    -((-m15*(m20*m4-m19*m5))+m17*(m17*m4-m16*m5)
                                            +(m16*m20-m17*m19)*m3)
                     *m7)
                -m5*(m12*(m16*(m10*m4-m5*m9)+m3*(m19*m9-m10*m18)
                                            -(m19*m4-m18*m5)*m8)
                    -m13*(m15*(m10*m4-m5*m9)+m3*(m17*m9-m10*m16)
                                            -(m17*m4-m16*m5)*m8)
                    +m2*((-m15*(m19*m9-m10*m18))+m16*(m17*m9-m10*m16)
                                                +(m16*m19-m17*m18)*m8)
                    -((-m15*(m19*m4-m18*m5))+m16*(m17*m4-m16*m5)
                                            +(m16*m19-m17*m18)*m3)
                     *m7)
                -m0*((-m12*((-m16*(m20*m9-m10*m19))+m17*(m19*m9-m10*m18)
                                                   +(m18*m20-m19^2)*m8))
                    +m13*((-m15*(m20*m9-m10*m19))+m17*(m17*m9-m10*m16)
                                                 +(m16*m20-m17*m19)*m8)
                    -m14*((-m15*(m19*m9-m10*m18))+m16*(m17*m9-m10*m16)
                                                 +(m16*m19-m17*m18)*m8)
                    +(m15*(m18*m20-m19^2)-m16*(m16*m20-m17*m19)
                                         +m17*(m16*m19-m17*m18))
                     *m7)
                +m1*((-m12*((-m16*(m20*m4-m19*m5))+m17*(m19*m4-m18*m5)
                                                  +(m18*m20-m19^2)*m3))
                    +m13*((-m15*(m20*m4-m19*m5))+m17*(m17*m4-m16*m5)
                                                +(m16*m20-m17*m19)*m3)
                    -m14*((-m15*(m19*m4-m18*m5))+m16*(m17*m4-m16*m5)
                                                +(m16*m19-m17*m18)*m3)
                    +m2*(m15*(m18*m20-m19^2)-m16*(m16*m20-m17*m19)
                                            +m17*(m16*m19-m17*m18))),
               m3*(m9*(m17*(m10*m4-m5*m9)+m3*(m20*m9-m10*m19)
                                         -(m20*m4-m19*m5)*m8)
                  -m10*(m16*(m10*m4-m5*m9)+m3*(m19*m9-m10*m18)
                                          -(m19*m4-m18*m5)*m8)
                  +m1*((-m16*(m20*m9-m10*m19))+m17*(m19*m9-m10*m18)
                                              +(m18*m20-m19^2)*m8)
                  -((-m16*(m20*m4-m19*m5))+m17*(m19*m4-m18*m5)
                                          +(m18*m20-m19^2)*m3)
                   *m6)
                -m4*(m8*(m17*(m10*m4-m5*m9)+m3*(m20*m9-m10*m19)
                                           -(m20*m4-m19*m5)*m8)
                    -m10*(m15*(m10*m4-m5*m9)+m3*(m17*m9-m10*m16)
                                            -(m17*m4-m16*m5)*m8)
                    +m1*((-m15*(m20*m9-m10*m19))+m17*(m17*m9-m10*m16)
                                                +(m16*m20-m17*m19)*m8)
                    -((-m15*(m20*m4-m19*m5))+m17*(m17*m4-m16*m5)
                                            +(m16*m20-m17*m19)*m3)
                     *m6)
                +m5*(m8*(m16*(m10*m4-m5*m9)+m3*(m19*m9-m10*m18)
                                           -(m19*m4-m18*m5)*m8)
                    -m9*(m15*(m10*m4-m5*m9)+m3*(m17*m9-m10*m16)
                                           -(m17*m4-m16*m5)*m8)
                    +m1*((-m15*(m19*m9-m10*m18))+m16*(m17*m9-m10*m16)
                                                +(m16*m19-m17*m18)*m8)
                    -((-m15*(m19*m4-m18*m5))+m16*(m17*m4-m16*m5)
                                            +(m16*m19-m17*m18)*m3)
                     *m6)
                +m0*((-m8*((-m16*(m20*m9-m10*m19))+m17*(m19*m9-m10*m18)
                                                  +(m18*m20-m19^2)*m8))
                    +m9*((-m15*(m20*m9-m10*m19))+m17*(m17*m9-m10*m16)
                                                +(m16*m20-m17*m19)*m8)
                    -m10*((-m15*(m19*m9-m10*m18))+m16*(m17*m9-m10*m16)
                                                 +(m16*m19-m17*m18)*m8)
                    +(m15*(m18*m20-m19^2)-m16*(m16*m20-m17*m19)
                                         +m17*(m16*m19-m17*m18))
                     *m6)
                -m1*(((-m15*(m20*m4-m19*m5))+m17*(m17*m4-m16*m5)
                                            +(m16*m20-m17*m19)*m3)
                    *m9
                    -((-m16*(m20*m4-m19*m5))+m17*(m19*m4-m18*m5)
                                            +(m18*m20-m19^2)*m3)
                     *m8
                    -m10*((-m15*(m19*m4-m18*m5))+m16*(m17*m4-m16*m5)
                                                +(m16*m19-m17*m18)*m3)
                    +m1*(m15*(m18*m20-m19^2)-m16*(m16*m20-m17*m19)
                                            +m17*(m16*m19-m17*m18))),
               (-m3*(m9*(m14*(m10*m4-m5*m9)+m2*(m20*m9-m10*m19)
                                           -(m20*m4-m19*m5)*m7)
                    -m10*(m13*(m10*m4-m5*m9)+m2*(m19*m9-m10*m18)
                                            -(m19*m4-m18*m5)*m7)
                    +m1*((-m13*(m20*m9-m10*m19))+m14*(m19*m9-m10*m18)
                                                +(m18*m20-m19^2)*m7)
                    -((-m13*(m20*m4-m19*m5))+m14*(m19*m4-m18*m5)
                                            +m2*(m18*m20-m19^2))
                     *m6))
                +m4*(m8*(m14*(m10*m4-m5*m9)+m2*(m20*m9-m10*m19)
                                           -(m20*m4-m19*m5)*m7)
                    -m10*(m12*(m10*m4-m5*m9)+m2*(m17*m9-m10*m16)
                                            -(m17*m4-m16*m5)*m7)
                    +m1*((-m12*(m20*m9-m10*m19))+m14*(m17*m9-m10*m16)
                                                +(m16*m20-m17*m19)*m7)
                    -((-m12*(m20*m4-m19*m5))+m14*(m17*m4-m16*m5)
                                            +m2*(m16*m20-m17*m19))
                     *m6)
                -m5*(m8*(m13*(m10*m4-m5*m9)+m2*(m19*m9-m10*m18)
                                           -(m19*m4-m18*m5)*m7)
                    -m9*(m12*(m10*m4-m5*m9)+m2*(m17*m9-m10*m16)
                                           -(m17*m4-m16*m5)*m7)
                    +m1*((-m12*(m19*m9-m10*m18))+m13*(m17*m9-m10*m16)
                                                +(m16*m19-m17*m18)*m7)
                    -((-m12*(m19*m4-m18*m5))+m13*(m17*m4-m16*m5)
                                            +(m16*m19-m17*m18)*m2)
                     *m6)
                -m0*((-m8*((-m13*(m20*m9-m10*m19))+m14*(m19*m9-m10*m18)
                                                  +(m18*m20-m19^2)*m7))
                    +m9*((-m12*(m20*m9-m10*m19))+m14*(m17*m9-m10*m16)
                                                +(m16*m20-m17*m19)*m7)
                    -m10*((-m12*(m19*m9-m10*m18))+m13*(m17*m9-m10*m16)
                                                 +(m16*m19-m17*m18)*m7)
                    +(m12*(m18*m20-m19^2)-m13*(m16*m20-m17*m19)
                                         +m14*(m16*m19-m17*m18))
                     *m6)
                +m1*(((-m12*(m20*m4-m19*m5))+m14*(m17*m4-m16*m5)
                                            +m2*(m16*m20-m17*m19))
                    *m9
                    -((-m13*(m20*m4-m19*m5))+m14*(m19*m4-m18*m5)
                                            +m2*(m18*m20-m19^2))
                     *m8
                    -m10*((-m12*(m19*m4-m18*m5))+m13*(m17*m4-m16*m5)
                                                +(m16*m19-m17*m18)*m2)
                    +m1*(m12*(m18*m20-m19^2)-m13*(m16*m20-m17*m19)
                                            +m14*(m16*m19-m17*m18))),
               m3*((m14*(m10*m3-m5*m8)+m2*(m20*m8-m10*m17)-(m20*m3-m17*m5)*m7)
                  *m9
                  -m10*(m13*(m10*m3-m5*m8)+m2*(m19*m8-m10*m16)
                                          -(m19*m3-m16*m5)*m7)
                  +m1*((-m13*(m20*m8-m10*m17))+m14*(m19*m8-m10*m16)
                                              +(m16*m20-m17*m19)*m7)
                  -((-m13*(m20*m3-m17*m5))+m14*(m19*m3-m16*m5)
                                          +m2*(m16*m20-m17*m19))
                   *m6)
                +m5*((-(m12*(m10*m3-m5*m8)+m2*(m17*m8-m10*m15)
                                          -(m17*m3-m15*m5)*m7)
                    *m9)
                    +m8*(m13*(m10*m3-m5*m8)+m2*(m19*m8-m10*m16)
                                           -(m19*m3-m16*m5)*m7)
                    +m1*((-m12*(m19*m8-m10*m16))+m13*(m17*m8-m10*m15)
                                                +(m15*m19-m16*m17)*m7)
                    -((-m12*(m19*m3-m16*m5))+m13*(m17*m3-m15*m5)
                                            +(m15*m19-m16*m17)*m2)
                     *m6)
                +m0*(((-m12*(m20*m8-m10*m17))+m14*(m17*m8-m10*m15)
                                             +(m15*m20-m17^2)*m7)
                    *m9
                    -m8*((-m13*(m20*m8-m10*m17))+m14*(m19*m8-m10*m16)
                                                +(m16*m20-m17*m19)*m7)
                    -m10*((-m12*(m19*m8-m10*m16))+m13*(m17*m8-m10*m15)
                                                 +(m15*m19-m16*m17)*m7)
                    +(m12*(m16*m20-m17*m19)-m13*(m15*m20-m17^2)
                                           +m14*(m15*m19-m16*m17))
                     *m6)
                -m1*(((-m12*(m20*m3-m17*m5))+m14*(m17*m3-m15*m5)
                                            +m2*(m15*m20-m17^2))
                    *m9
                    -((-m13*(m20*m3-m17*m5))+m14*(m19*m3-m16*m5)
                                            +m2*(m16*m20-m17*m19))
                     *m8
                    -m10*((-m12*(m19*m3-m16*m5))+m13*(m17*m3-m15*m5)
                                                +(m15*m19-m16*m17)*m2)
                    +m1*(m12*(m16*m20-m17*m19)-m13*(m15*m20-m17^2)
                                              +m14*(m15*m19-m16*m17)))
                -m4*(m8*(m14*(m10*m3-m5*m8)+m2*(m20*m8-m10*m17)
                                           -(m20*m3-m17*m5)*m7)
                    -m10*(m12*(m10*m3-m5*m8)+m2*(m17*m8-m10*m15)
                                            -(m17*m3-m15*m5)*m7)
                    +m1*((-m12*(m20*m8-m10*m17))+m14*(m17*m8-m10*m15)
                                                +(m15*m20-m17^2)*m7)
                    -((-m12*(m20*m3-m17*m5))+m14*(m17*m3-m15*m5)
                                            +m2*(m15*m20-m17^2))
                     *m6),
               (-m3*(m9*(m14*(m3*m9-m4*m8)+m2*(m19*m8-m17*m9)
                                          -(m19*m3-m17*m4)*m7)
                    -m10*(m13*(m3*m9-m4*m8)+m2*(m18*m8-m16*m9)
                                           -(m18*m3-m16*m4)*m7)
                    +m1*((-m13*(m19*m8-m17*m9))+m14*(m18*m8-m16*m9)
                                               +(m16*m19-m17*m18)*m7)
                    -((-m13*(m19*m3-m17*m4))+m14*(m18*m3-m16*m4)
                                            +(m16*m19-m17*m18)*m2)
                     *m6))
                +m4*(m8*(m14*(m3*m9-m4*m8)+m2*(m19*m8-m17*m9)
                                          -(m19*m3-m17*m4)*m7)
                    -m10*(m12*(m3*m9-m4*m8)+m2*(m16*m8-m15*m9)
                                           -(m16*m3-m15*m4)*m7)
                    +m1*((-m12*(m19*m8-m17*m9))+m14*(m16*m8-m15*m9)
                                               +(m15*m19-m16*m17)*m7)
                    -((-m12*(m19*m3-m17*m4))+m14*(m16*m3-m15*m4)
                                            +(m15*m19-m16*m17)*m2)
                     *m6)
                -m5*(m8*(m13*(m3*m9-m4*m8)+m2*(m18*m8-m16*m9)
                                          -(m18*m3-m16*m4)*m7)
                    -m9*(m12*(m3*m9-m4*m8)+m2*(m16*m8-m15*m9)
                                          -(m16*m3-m15*m4)*m7)
                    +m1*((-m12*(m18*m8-m16*m9))+m13*(m16*m8-m15*m9)
                                               +(m15*m18-m16^2)*m7)
                    -((-m12*(m18*m3-m16*m4))+m13*(m16*m3-m15*m4)
                                            +(m15*m18-m16^2)*m2)
                     *m6)
                -m0*((-m8*((-m13*(m19*m8-m17*m9))+m14*(m18*m8-m16*m9)
                                                 +(m16*m19-m17*m18)*m7))
                    +m9*((-m12*(m19*m8-m17*m9))+m14*(m16*m8-m15*m9)
                                               +(m15*m19-m16*m17)*m7)
                    -m10*((-m12*(m18*m8-m16*m9))+m13*(m16*m8-m15*m9)
                                                +(m15*m18-m16^2)*m7)
                    +(m12*(m16*m19-m17*m18)-m13*(m15*m19-m16*m17)
                                           +m14*(m15*m18-m16^2))
                     *m6)
                +m1*(((-m12*(m19*m3-m17*m4))+m14*(m16*m3-m15*m4)
                                            +(m15*m19-m16*m17)*m2)
                    *m9
                    -((-m13*(m19*m3-m17*m4))+m14*(m18*m3-m16*m4)
                                            +(m16*m19-m17*m18)*m2)
                     *m8
                    -m10*((-m12*(m18*m3-m16*m4))+m13*(m16*m3-m15*m4)
                                                +(m15*m18-m16^2)*m2)
                    +m1*(m12*(m16*m19-m17*m18)-m13*(m15*m19-m16*m17)
                                              +m14*(m15*m18-m16^2)))],
              [(-m7*(m13*(m17*(m10*m4-m5*m9)+m3*(m20*m9-m10*m19)
                                            -(m20*m4-m19*m5)*m8)
                    -m14*(m16*(m10*m4-m5*m9)+m3*(m19*m9-m10*m18)
                                            -(m19*m4-m18*m5)*m8)
                    +m2*((-m16*(m20*m9-m10*m19))+m17*(m19*m9-m10*m18)
                                                +(m18*m20-m19^2)*m8)
                    -((-m16*(m20*m4-m19*m5))+m17*(m19*m4-m18*m5)
                                            +(m18*m20-m19^2)*m3)
                     *m7))
                +m9*(m11*(m17*(m10*m4-m5*m9)+m3*(m20*m9-m10*m19)
                                            -(m20*m4-m19*m5)*m8)
                    -m14*(m12*(m10*m4-m5*m9)+m3*(m14*m9-m10*m13)
                                            -(m14*m4-m13*m5)*m8)
                    +m2*((-m12*(m20*m9-m10*m19))+m17*(m14*m9-m10*m13)
                                                +(m13*m20-m14*m19)*m8)
                    -((-m12*(m20*m4-m19*m5))+m17*(m14*m4-m13*m5)
                                            +(m13*m20-m14*m19)*m3)
                     *m7)
                -m10*(m11*(m16*(m10*m4-m5*m9)+m3*(m19*m9-m10*m18)
                                             -(m19*m4-m18*m5)*m8)
                     -m13*(m12*(m10*m4-m5*m9)+m3*(m14*m9-m10*m13)
                                             -(m14*m4-m13*m5)*m8)
                     +m2*((-m12*(m19*m9-m10*m18))+m16*(m14*m9-m10*m13)
                                                 +(m13*m19-m14*m18)*m8)
                     -((-m12*(m19*m4-m18*m5))+m16*(m14*m4-m13*m5)
                                             +(m13*m19-m14*m18)*m3)
                      *m7)
                -m1*((-m11*((-m16*(m20*m9-m10*m19))+m17*(m19*m9-m10*m18)
                                                   +(m18*m20-m19^2)*m8))
                    +m13*((-m12*(m20*m9-m10*m19))+m17*(m14*m9-m10*m13)
                                                 +(m13*m20-m14*m19)*m8)
                    -m14*((-m12*(m19*m9-m10*m18))+m16*(m14*m9-m10*m13)
                                                 +(m13*m19-m14*m18)*m8)
                    +(m12*(m18*m20-m19^2)-m16*(m13*m20-m14*m19)
                                         +m17*(m13*m19-m14*m18))
                     *m7)
                +((-m11*((-m16*(m20*m4-m19*m5))+m17*(m19*m4-m18*m5)
                                               +(m18*m20-m19^2)*m3))
                 +m13*((-m12*(m20*m4-m19*m5))+m17*(m14*m4-m13*m5)
                                             +(m13*m20-m14*m19)*m3)
                 -m14*((-m12*(m19*m4-m18*m5))+m16*(m14*m4-m13*m5)
                                             +(m13*m19-m14*m18)*m3)
                 +m2*(m12*(m18*m20-m19^2)-m16*(m13*m20-m14*m19)
                                         +m17*(m13*m19-m14*m18)))
                 *m6,
               m2*(m13*(m17*(m10*m4-m5*m9)+m3*(m20*m9-m10*m19)
                                          -(m20*m4-m19*m5)*m8)
                  -m14*(m16*(m10*m4-m5*m9)+m3*(m19*m9-m10*m18)
                                          -(m19*m4-m18*m5)*m8)
                  +m2*((-m16*(m20*m9-m10*m19))+m17*(m19*m9-m10*m18)
                                              +(m18*m20-m19^2)*m8)
                  -((-m16*(m20*m4-m19*m5))+m17*(m19*m4-m18*m5)
                                          +(m18*m20-m19^2)*m3)
                   *m7)
                -m4*(m11*(m17*(m10*m4-m5*m9)+m3*(m20*m9-m10*m19)
                                            -(m20*m4-m19*m5)*m8)
                    -m14*(m12*(m10*m4-m5*m9)+m3*(m14*m9-m10*m13)
                                            -(m14*m4-m13*m5)*m8)
                    +m2*((-m12*(m20*m9-m10*m19))+m17*(m14*m9-m10*m13)
                                                +(m13*m20-m14*m19)*m8)
                    -((-m12*(m20*m4-m19*m5))+m17*(m14*m4-m13*m5)
                                            +(m13*m20-m14*m19)*m3)
                     *m7)
                +m5*(m11*(m16*(m10*m4-m5*m9)+m3*(m19*m9-m10*m18)
                                            -(m19*m4-m18*m5)*m8)
                    -m13*(m12*(m10*m4-m5*m9)+m3*(m14*m9-m10*m13)
                                            -(m14*m4-m13*m5)*m8)
                    +m2*((-m12*(m19*m9-m10*m18))+m16*(m14*m9-m10*m13)
                                                +(m13*m19-m14*m18)*m8)
                    -((-m12*(m19*m4-m18*m5))+m16*(m14*m4-m13*m5)
                                            +(m13*m19-m14*m18)*m3)
                     *m7)
                +m0*((-m11*((-m16*(m20*m9-m10*m19))+m17*(m19*m9-m10*m18)
                                                   +(m18*m20-m19^2)*m8))
                    +m13*((-m12*(m20*m9-m10*m19))+m17*(m14*m9-m10*m13)
                                                 +(m13*m20-m14*m19)*m8)
                    -m14*((-m12*(m19*m9-m10*m18))+m16*(m14*m9-m10*m13)
                                                 +(m13*m19-m14*m18)*m8)
                    +(m12*(m18*m20-m19^2)-m16*(m13*m20-m14*m19)
                                         +m17*(m13*m19-m14*m18))
                     *m7)
                -m1*((-m11*((-m16*(m20*m4-m19*m5))+m17*(m19*m4-m18*m5)
                                                  +(m18*m20-m19^2)*m3))
                    +m13*((-m12*(m20*m4-m19*m5))+m17*(m14*m4-m13*m5)
                                                +(m13*m20-m14*m19)*m3)
                    -m14*((-m12*(m19*m4-m18*m5))+m16*(m14*m4-m13*m5)
                                                +(m13*m19-m14*m18)*m3)
                    +m2*(m12*(m18*m20-m19^2)-m16*(m13*m20-m14*m19)
                                            +m17*(m13*m19-m14*m18))),
               (-m2*(m9*(m17*(m10*m4-m5*m9)+m3*(m20*m9-m10*m19)
                                           -(m20*m4-m19*m5)*m8)
                    -m10*(m16*(m10*m4-m5*m9)+m3*(m19*m9-m10*m18)
                                            -(m19*m4-m18*m5)*m8)
                    +m1*((-m16*(m20*m9-m10*m19))+m17*(m19*m9-m10*m18)
                                                +(m18*m20-m19^2)*m8)
                    -((-m16*(m20*m4-m19*m5))+m17*(m19*m4-m18*m5)
                                            +(m18*m20-m19^2)*m3)
                     *m6))
                +m4*(m7*(m17*(m10*m4-m5*m9)+m3*(m20*m9-m10*m19)
                                           -(m20*m4-m19*m5)*m8)
                    -m10*(m12*(m10*m4-m5*m9)+m3*(m14*m9-m10*m13)
                                            -(m14*m4-m13*m5)*m8)
                    +m1*((-m12*(m20*m9-m10*m19))+m17*(m14*m9-m10*m13)
                                                +(m13*m20-m14*m19)*m8)
                    -((-m12*(m20*m4-m19*m5))+m17*(m14*m4-m13*m5)
                                            +(m13*m20-m14*m19)*m3)
                     *m6)
                -m5*(m7*(m16*(m10*m4-m5*m9)+m3*(m19*m9-m10*m18)
                                           -(m19*m4-m18*m5)*m8)
                    -m9*(m12*(m10*m4-m5*m9)+m3*(m14*m9-m10*m13)
                                           -(m14*m4-m13*m5)*m8)
                    +m1*((-m12*(m19*m9-m10*m18))+m16*(m14*m9-m10*m13)
                                                +(m13*m19-m14*m18)*m8)
                    -((-m12*(m19*m4-m18*m5))+m16*(m14*m4-m13*m5)
                                            +(m13*m19-m14*m18)*m3)
                     *m6)
                -m0*((-m7*((-m16*(m20*m9-m10*m19))+m17*(m19*m9-m10*m18)
                                                  +(m18*m20-m19^2)*m8))
                    +m9*((-m12*(m20*m9-m10*m19))+m17*(m14*m9-m10*m13)
                                                +(m13*m20-m14*m19)*m8)
                    -m10*((-m12*(m19*m9-m10*m18))+m16*(m14*m9-m10*m13)
                                                 +(m13*m19-m14*m18)*m8)
                    +(m12*(m18*m20-m19^2)-m16*(m13*m20-m14*m19)
                                         +m17*(m13*m19-m14*m18))
                     *m6)
                +m1*(((-m12*(m20*m4-m19*m5))+m17*(m14*m4-m13*m5)
                                            +(m13*m20-m14*m19)*m3)
                    *m9
                    -((-m16*(m20*m4-m19*m5))+m17*(m19*m4-m18*m5)
                                            +(m18*m20-m19^2)*m3)
                     *m7
                    -m10*((-m12*(m19*m4-m18*m5))+m16*(m14*m4-m13*m5)
                                                +(m13*m19-m14*m18)*m3)
                    +m1*(m12*(m18*m20-m19^2)-m16*(m13*m20-m14*m19)
                                            +m17*(m13*m19-m14*m18))),
               m2*(m9*(m14*(m10*m4-m5*m9)+m2*(m20*m9-m10*m19)
                                         -(m20*m4-m19*m5)*m7)
                  -m10*(m13*(m10*m4-m5*m9)+m2*(m19*m9-m10*m18)
                                          -(m19*m4-m18*m5)*m7)
                  +m1*((-m13*(m20*m9-m10*m19))+m14*(m19*m9-m10*m18)
                                              +(m18*m20-m19^2)*m7)
                  -((-m13*(m20*m4-m19*m5))+m14*(m19*m4-m18*m5)
                                          +m2*(m18*m20-m19^2))
                   *m6)
                -m4*(m7*(m14*(m10*m4-m5*m9)+m2*(m20*m9-m10*m19)
                                           -(m20*m4-m19*m5)*m7)
                    -m10*(m11*(m10*m4-m5*m9)+m2*(m14*m9-m10*m13)
                                            -(m14*m4-m13*m5)*m7)
                    +m1*((-m11*(m20*m9-m10*m19))+m14*(m14*m9-m10*m13)
                                                +(m13*m20-m14*m19)*m7)
                    -((-m11*(m20*m4-m19*m5))+m14*(m14*m4-m13*m5)
                                            +m2*(m13*m20-m14*m19))
                     *m6)
                +m5*(m7*(m13*(m10*m4-m5*m9)+m2*(m19*m9-m10*m18)
                                           -(m19*m4-m18*m5)*m7)
                    -m9*(m11*(m10*m4-m5*m9)+m2*(m14*m9-m10*m13)
                                           -(m14*m4-m13*m5)*m7)
                    +m1*((-m11*(m19*m9-m10*m18))+m13*(m14*m9-m10*m13)
                                                +(m13*m19-m14*m18)*m7)
                    -((-m11*(m19*m4-m18*m5))+m13*(m14*m4-m13*m5)
                                            +(m13*m19-m14*m18)*m2)
                     *m6)
                +m0*((-m7*((-m13*(m20*m9-m10*m19))+m14*(m19*m9-m10*m18)
                                                  +(m18*m20-m19^2)*m7))
                    +m9*((-m11*(m20*m9-m10*m19))+m14*(m14*m9-m10*m13)
                                                +(m13*m20-m14*m19)*m7)
                    -m10*((-m11*(m19*m9-m10*m18))+m13*(m14*m9-m10*m13)
                                                 +(m13*m19-m14*m18)*m7)
                    +(m11*(m18*m20-m19^2)-m13*(m13*m20-m14*m19)
                                         +m14*(m13*m19-m14*m18))
                     *m6)
                -m1*(((-m11*(m20*m4-m19*m5))+m14*(m14*m4-m13*m5)
                                            +m2*(m13*m20-m14*m19))
                    *m9
                    -((-m13*(m20*m4-m19*m5))+m14*(m19*m4-m18*m5)
                                            +m2*(m18*m20-m19^2))
                     *m7
                    -m10*((-m11*(m19*m4-m18*m5))+m13*(m14*m4-m13*m5)
                                                +(m13*m19-m14*m18)*m2)
                    +m1*(m11*(m18*m20-m19^2)-m13*(m13*m20-m14*m19)
                                            +m14*(m13*m19-m14*m18))),
               (-m2*((m14*(m10*m3-m5*m8)+m2*(m20*m8-m10*m17)
                                        -(m20*m3-m17*m5)*m7)
                    *m9
                    -m10*(m13*(m10*m3-m5*m8)+m2*(m19*m8-m10*m16)
                                            -(m19*m3-m16*m5)*m7)
                    +m1*((-m13*(m20*m8-m10*m17))+m14*(m19*m8-m10*m16)
                                                +(m16*m20-m17*m19)*m7)
                    -((-m13*(m20*m3-m17*m5))+m14*(m19*m3-m16*m5)
                                            +m2*(m16*m20-m17*m19))
                     *m6))
                -m5*((-(m11*(m10*m3-m5*m8)+m2*(m14*m8-m10*m12)
                                          -(m14*m3-m12*m5)*m7)
                    *m9)
                    +m7*(m13*(m10*m3-m5*m8)+m2*(m19*m8-m10*m16)
                                           -(m19*m3-m16*m5)*m7)
                    +m1*((-m11*(m19*m8-m10*m16))+m13*(m14*m8-m10*m12)
                                                +(m12*m19-m14*m16)*m7)
                    -((-m11*(m19*m3-m16*m5))+m13*(m14*m3-m12*m5)
                                            +(m12*m19-m14*m16)*m2)
                     *m6)
                -m0*(((-m11*(m20*m8-m10*m17))+m14*(m14*m8-m10*m12)
                                             +(m12*m20-m14*m17)*m7)
                    *m9
                    -m7*((-m13*(m20*m8-m10*m17))+m14*(m19*m8-m10*m16)
                                                +(m16*m20-m17*m19)*m7)
                    -m10*((-m11*(m19*m8-m10*m16))+m13*(m14*m8-m10*m12)
                                                 +(m12*m19-m14*m16)*m7)
                    +(m11*(m16*m20-m17*m19)-m13*(m12*m20-m14*m17)
                                           +m14*(m12*m19-m14*m16))
                     *m6)
                +m1*(((-m11*(m20*m3-m17*m5))+m14*(m14*m3-m12*m5)
                                            +m2*(m12*m20-m14*m17))
                    *m9
                    -((-m13*(m20*m3-m17*m5))+m14*(m19*m3-m16*m5)
                                            +m2*(m16*m20-m17*m19))
                     *m7
                    -m10*((-m11*(m19*m3-m16*m5))+m13*(m14*m3-m12*m5)
                                                +(m12*m19-m14*m16)*m2)
                    +m1*(m11*(m16*m20-m17*m19)-m13*(m12*m20-m14*m17)
                                              +m14*(m12*m19-m14*m16)))
                +m4*(m7*(m14*(m10*m3-m5*m8)+m2*(m20*m8-m10*m17)
                                           -(m20*m3-m17*m5)*m7)
                    -m10*(m11*(m10*m3-m5*m8)+m2*(m14*m8-m10*m12)
                                            -(m14*m3-m12*m5)*m7)
                    +m1*((-m11*(m20*m8-m10*m17))+m14*(m14*m8-m10*m12)
                                                +(m12*m20-m14*m17)*m7)
                    -((-m11*(m20*m3-m17*m5))+m14*(m14*m3-m12*m5)
                                            +m2*(m12*m20-m14*m17))
                     *m6),
               m2*(m9*(m14*(m3*m9-m4*m8)+m2*(m19*m8-m17*m9)
                                        -(m19*m3-m17*m4)*m7)
                  -m10*(m13*(m3*m9-m4*m8)+m2*(m18*m8-m16*m9)
                                         -(m18*m3-m16*m4)*m7)
                  +m1*((-m13*(m19*m8-m17*m9))+m14*(m18*m8-m16*m9)
                                             +(m16*m19-m17*m18)*m7)
                  -((-m13*(m19*m3-m17*m4))+m14*(m18*m3-m16*m4)
                                          +(m16*m19-m17*m18)*m2)
                   *m6)
                -m4*(m7*(m14*(m3*m9-m4*m8)+m2*(m19*m8-m17*m9)
                                          -(m19*m3-m17*m4)*m7)
                    -m10*(m11*(m3*m9-m4*m8)+m2*(m13*m8-m12*m9)
                                           -(m13*m3-m12*m4)*m7)
                    +m1*((-m11*(m19*m8-m17*m9))+m14*(m13*m8-m12*m9)
                                               +(m12*m19-m13*m17)*m7)
                    -((-m11*(m19*m3-m17*m4))+m14*(m13*m3-m12*m4)
                                            +(m12*m19-m13*m17)*m2)
                     *m6)
                +m5*(m7*(m13*(m3*m9-m4*m8)+m2*(m18*m8-m16*m9)
                                          -(m18*m3-m16*m4)*m7)
                    -m9*(m11*(m3*m9-m4*m8)+m2*(m13*m8-m12*m9)
                                          -(m13*m3-m12*m4)*m7)
                    +m1*((-m11*(m18*m8-m16*m9))+m13*(m13*m8-m12*m9)
                                               +(m12*m18-m13*m16)*m7)
                    -((-m11*(m18*m3-m16*m4))+m13*(m13*m3-m12*m4)
                                            +(m12*m18-m13*m16)*m2)
                     *m6)
                +m0*((-m7*((-m13*(m19*m8-m17*m9))+m14*(m18*m8-m16*m9)
                                                 +(m16*m19-m17*m18)*m7))
                    +m9*((-m11*(m19*m8-m17*m9))+m14*(m13*m8-m12*m9)
                                               +(m12*m19-m13*m17)*m7)
                    -m10*((-m11*(m18*m8-m16*m9))+m13*(m13*m8-m12*m9)
                                                +(m12*m18-m13*m16)*m7)
                    +(m11*(m16*m19-m17*m18)-m13*(m12*m19-m13*m17)
                                           +m14*(m12*m18-m13*m16))
                     *m6)
                -m1*(((-m11*(m19*m3-m17*m4))+m14*(m13*m3-m12*m4)
                                            +(m12*m19-m13*m17)*m2)
                    *m9
                    -((-m13*(m19*m3-m17*m4))+m14*(m18*m3-m16*m4)
                                            +(m16*m19-m17*m18)*m2)
                     *m7
                    -m10*((-m11*(m18*m3-m16*m4))+m13*(m13*m3-m12*m4)
                                                +(m12*m18-m13*m16)*m2)
                    +m1*(m11*(m16*m19-m17*m18)-m13*(m12*m19-m13*m17)
                                              +m14*(m12*m18-m13*m16)))],
              [m7*(m12*(m17*(m10*m4-m5*m9)+m3*(m20*m9-m10*m19)
                                          -(m20*m4-m19*m5)*m8)
                  -m14*(m15*(m10*m4-m5*m9)+m3*(m17*m9-m10*m16)
                                          -(m17*m4-m16*m5)*m8)
                  +m2*((-m15*(m20*m9-m10*m19))+m17*(m17*m9-m10*m16)
                                              +(m16*m20-m17*m19)*m8)
                  -((-m15*(m20*m4-m19*m5))+m17*(m17*m4-m16*m5)
                                          +(m16*m20-m17*m19)*m3)
                   *m7)
                -m8*(m11*(m17*(m10*m4-m5*m9)+m3*(m20*m9-m10*m19)
                                            -(m20*m4-m19*m5)*m8)
                    -m14*(m12*(m10*m4-m5*m9)+m3*(m14*m9-m10*m13)
                                            -(m14*m4-m13*m5)*m8)
                    +m2*((-m12*(m20*m9-m10*m19))+m17*(m14*m9-m10*m13)
                                                +(m13*m20-m14*m19)*m8)
                    -((-m12*(m20*m4-m19*m5))+m17*(m14*m4-m13*m5)
                                            +(m13*m20-m14*m19)*m3)
                     *m7)
                +m10*(m11*(m15*(m10*m4-m5*m9)+m3*(m17*m9-m10*m16)
                                             -(m17*m4-m16*m5)*m8)
                     -m12*(m12*(m10*m4-m5*m9)+m3*(m14*m9-m10*m13)
                                             -(m14*m4-m13*m5)*m8)
                     +m2*((-m12*(m17*m9-m10*m16))+m15*(m14*m9-m10*m13)
                                                 +(m13*m17-m14*m16)*m8)
                     -((-m12*(m17*m4-m16*m5))+m15*(m14*m4-m13*m5)
                                             +(m13*m17-m14*m16)*m3)
                      *m7)
                +m1*((-m11*((-m15*(m20*m9-m10*m19))+m17*(m17*m9-m10*m16)
                                                   +(m16*m20-m17*m19)*m8))
                    +m12*((-m12*(m20*m9-m10*m19))+m17*(m14*m9-m10*m13)
                                                 +(m13*m20-m14*m19)*m8)
                    -m14*((-m12*(m17*m9-m10*m16))+m15*(m14*m9-m10*m13)
                                                 +(m13*m17-m14*m16)*m8)
                    +(m12*(m16*m20-m17*m19)-m15*(m13*m20-m14*m19)
                                           +m17*(m13*m17-m14*m16))
                     *m7)
                -((-m11*((-m15*(m20*m4-m19*m5))+m17*(m17*m4-m16*m5)
                                               +(m16*m20-m17*m19)*m3))
                 +m12*((-m12*(m20*m4-m19*m5))+m17*(m14*m4-m13*m5)
                                             +(m13*m20-m14*m19)*m3)
                 -m14*((-m12*(m17*m4-m16*m5))+m15*(m14*m4-m13*m5)
                                             +(m13*m17-m14*m16)*m3)
                 +m2*(m12*(m16*m20-m17*m19)-m15*(m13*m20-m14*m19)
                                           +m17*(m13*m17-m14*m16)))
                 *m6,
               (-m2*(m12*(m17*(m10*m4-m5*m9)+m3*(m20*m9-m10*m19)
                                            -(m20*m4-m19*m5)*m8)
                    -m14*(m15*(m10*m4-m5*m9)+m3*(m17*m9-m10*m16)
                                            -(m17*m4-m16*m5)*m8)
                    +m2*((-m15*(m20*m9-m10*m19))+m17*(m17*m9-m10*m16)
                                                +(m16*m20-m17*m19)*m8)
                    -((-m15*(m20*m4-m19*m5))+m17*(m17*m4-m16*m5)
                                            +(m16*m20-m17*m19)*m3)
                     *m7))
                +m3*(m11*(m17*(m10*m4-m5*m9)+m3*(m20*m9-m10*m19)
                                            -(m20*m4-m19*m5)*m8)
                    -m14*(m12*(m10*m4-m5*m9)+m3*(m14*m9-m10*m13)
                                            -(m14*m4-m13*m5)*m8)
                    +m2*((-m12*(m20*m9-m10*m19))+m17*(m14*m9-m10*m13)
                                                +(m13*m20-m14*m19)*m8)
                    -((-m12*(m20*m4-m19*m5))+m17*(m14*m4-m13*m5)
                                            +(m13*m20-m14*m19)*m3)
                     *m7)
                -m5*(m11*(m15*(m10*m4-m5*m9)+m3*(m17*m9-m10*m16)
                                            -(m17*m4-m16*m5)*m8)
                    -m12*(m12*(m10*m4-m5*m9)+m3*(m14*m9-m10*m13)
                                            -(m14*m4-m13*m5)*m8)
                    +m2*((-m12*(m17*m9-m10*m16))+m15*(m14*m9-m10*m13)
                                                +(m13*m17-m14*m16)*m8)
                    -((-m12*(m17*m4-m16*m5))+m15*(m14*m4-m13*m5)
                                            +(m13*m17-m14*m16)*m3)
                     *m7)
                -m0*((-m11*((-m15*(m20*m9-m10*m19))+m17*(m17*m9-m10*m16)
                                                   +(m16*m20-m17*m19)*m8))
                    +m12*((-m12*(m20*m9-m10*m19))+m17*(m14*m9-m10*m13)
                                                 +(m13*m20-m14*m19)*m8)
                    -m14*((-m12*(m17*m9-m10*m16))+m15*(m14*m9-m10*m13)
                                                 +(m13*m17-m14*m16)*m8)
                    +(m12*(m16*m20-m17*m19)-m15*(m13*m20-m14*m19)
                                           +m17*(m13*m17-m14*m16))
                     *m7)
                +m1*((-m11*((-m15*(m20*m4-m19*m5))+m17*(m17*m4-m16*m5)
                                                  +(m16*m20-m17*m19)*m3))
                    +m12*((-m12*(m20*m4-m19*m5))+m17*(m14*m4-m13*m5)
                                                +(m13*m20-m14*m19)*m3)
                    -m14*((-m12*(m17*m4-m16*m5))+m15*(m14*m4-m13*m5)
                                                +(m13*m17-m14*m16)*m3)
                    +m2*(m12*(m16*m20-m17*m19)-m15*(m13*m20-m14*m19)
                                              +m17*(m13*m17-m14*m16))),
               m2*(m8*(m17*(m10*m4-m5*m9)+m3*(m20*m9-m10*m19)
                                         -(m20*m4-m19*m5)*m8)
                  -m10*(m15*(m10*m4-m5*m9)+m3*(m17*m9-m10*m16)
                                          -(m17*m4-m16*m5)*m8)
                  +m1*((-m15*(m20*m9-m10*m19))+m17*(m17*m9-m10*m16)
                                              +(m16*m20-m17*m19)*m8)
                  -((-m15*(m20*m4-m19*m5))+m17*(m17*m4-m16*m5)
                                          +(m16*m20-m17*m19)*m3)
                   *m6)
                -m3*(m7*(m17*(m10*m4-m5*m9)+m3*(m20*m9-m10*m19)
                                           -(m20*m4-m19*m5)*m8)
                    -m10*(m12*(m10*m4-m5*m9)+m3*(m14*m9-m10*m13)
                                            -(m14*m4-m13*m5)*m8)
                    +m1*((-m12*(m20*m9-m10*m19))+m17*(m14*m9-m10*m13)
                                                +(m13*m20-m14*m19)*m8)
                    -((-m12*(m20*m4-m19*m5))+m17*(m14*m4-m13*m5)
                                            +(m13*m20-m14*m19)*m3)
                     *m6)
                +m5*(m7*(m15*(m10*m4-m5*m9)+m3*(m17*m9-m10*m16)
                                           -(m17*m4-m16*m5)*m8)
                    -m8*(m12*(m10*m4-m5*m9)+m3*(m14*m9-m10*m13)
                                           -(m14*m4-m13*m5)*m8)
                    +m1*((-m12*(m17*m9-m10*m16))+m15*(m14*m9-m10*m13)
                                                +(m13*m17-m14*m16)*m8)
                    -((-m12*(m17*m4-m16*m5))+m15*(m14*m4-m13*m5)
                                            +(m13*m17-m14*m16)*m3)
                     *m6)
                +m0*((-m7*((-m15*(m20*m9-m10*m19))+m17*(m17*m9-m10*m16)
                                                  +(m16*m20-m17*m19)*m8))
                    +m8*((-m12*(m20*m9-m10*m19))+m17*(m14*m9-m10*m13)
                                                +(m13*m20-m14*m19)*m8)
                    -m10*((-m12*(m17*m9-m10*m16))+m15*(m14*m9-m10*m13)
                                                 +(m13*m17-m14*m16)*m8)
                    +(m12*(m16*m20-m17*m19)-m15*(m13*m20-m14*m19)
                                           +m17*(m13*m17-m14*m16))
                     *m6)
                -m1*(((-m12*(m20*m4-m19*m5))+m17*(m14*m4-m13*m5)
                                            +(m13*m20-m14*m19)*m3)
                    *m8
                    -((-m15*(m20*m4-m19*m5))+m17*(m17*m4-m16*m5)
                                            +(m16*m20-m17*m19)*m3)
                     *m7
                    -m10*((-m12*(m17*m4-m16*m5))+m15*(m14*m4-m13*m5)
                                                +(m13*m17-m14*m16)*m3)
                    +m1*(m12*(m16*m20-m17*m19)-m15*(m13*m20-m14*m19)
                                              +m17*(m13*m17-m14*m16))),
               (-m2*(m8*(m14*(m10*m4-m5*m9)+m2*(m20*m9-m10*m19)
                                           -(m20*m4-m19*m5)*m7)
                    -m10*(m12*(m10*m4-m5*m9)+m2*(m17*m9-m10*m16)
                                            -(m17*m4-m16*m5)*m7)
                    +m1*((-m12*(m20*m9-m10*m19))+m14*(m17*m9-m10*m16)
                                                +(m16*m20-m17*m19)*m7)
                    -((-m12*(m20*m4-m19*m5))+m14*(m17*m4-m16*m5)
                                            +m2*(m16*m20-m17*m19))
                     *m6))
                +m3*(m7*(m14*(m10*m4-m5*m9)+m2*(m20*m9-m10*m19)
                                           -(m20*m4-m19*m5)*m7)
                    -m10*(m11*(m10*m4-m5*m9)+m2*(m14*m9-m10*m13)
                                            -(m14*m4-m13*m5)*m7)
                    +m1*((-m11*(m20*m9-m10*m19))+m14*(m14*m9-m10*m13)
                                                +(m13*m20-m14*m19)*m7)
                    -((-m11*(m20*m4-m19*m5))+m14*(m14*m4-m13*m5)
                                            +m2*(m13*m20-m14*m19))
                     *m6)
                -m5*(m7*(m12*(m10*m4-m5*m9)+m2*(m17*m9-m10*m16)
                                           -(m17*m4-m16*m5)*m7)
                    -m8*(m11*(m10*m4-m5*m9)+m2*(m14*m9-m10*m13)
                                           -(m14*m4-m13*m5)*m7)
                    +m1*((-m11*(m17*m9-m10*m16))+m12*(m14*m9-m10*m13)
                                                +(m13*m17-m14*m16)*m7)
                    -((-m11*(m17*m4-m16*m5))+m12*(m14*m4-m13*m5)
                                            +(m13*m17-m14*m16)*m2)
                     *m6)
                -m0*((-m7*((-m12*(m20*m9-m10*m19))+m14*(m17*m9-m10*m16)
                                                  +(m16*m20-m17*m19)*m7))
                    +m8*((-m11*(m20*m9-m10*m19))+m14*(m14*m9-m10*m13)
                                                +(m13*m20-m14*m19)*m7)
                    -m10*((-m11*(m17*m9-m10*m16))+m12*(m14*m9-m10*m13)
                                                 +(m13*m17-m14*m16)*m7)
                    +(m11*(m16*m20-m17*m19)-m12*(m13*m20-m14*m19)
                                           +m14*(m13*m17-m14*m16))
                     *m6)
                +m1*(((-m11*(m20*m4-m19*m5))+m14*(m14*m4-m13*m5)
                                            +m2*(m13*m20-m14*m19))
                    *m8
                    -((-m12*(m20*m4-m19*m5))+m14*(m17*m4-m16*m5)
                                            +m2*(m16*m20-m17*m19))
                     *m7
                    -m10*((-m11*(m17*m4-m16*m5))+m12*(m14*m4-m13*m5)
                                                +(m13*m17-m14*m16)*m2)
                    +m1*(m11*(m16*m20-m17*m19)-m12*(m13*m20-m14*m19)
                                              +m14*(m13*m17-m14*m16))),
               m2*(m8*(m14*(m10*m3-m5*m8)+m2*(m20*m8-m10*m17)
                                         -(m20*m3-m17*m5)*m7)
                  -m10*(m12*(m10*m3-m5*m8)+m2*(m17*m8-m10*m15)
                                          -(m17*m3-m15*m5)*m7)
                  +m1*((-m12*(m20*m8-m10*m17))+m14*(m17*m8-m10*m15)
                                              +(m15*m20-m17^2)*m7)
                  -((-m12*(m20*m3-m17*m5))+m14*(m17*m3-m15*m5)
                                          +m2*(m15*m20-m17^2))
                   *m6)
                -m3*(m7*(m14*(m10*m3-m5*m8)+m2*(m20*m8-m10*m17)
                                           -(m20*m3-m17*m5)*m7)
                    -m10*(m11*(m10*m3-m5*m8)+m2*(m14*m8-m10*m12)
                                            -(m14*m3-m12*m5)*m7)
                    +m1*((-m11*(m20*m8-m10*m17))+m14*(m14*m8-m10*m12)
                                                +(m12*m20-m14*m17)*m7)
                    -((-m11*(m20*m3-m17*m5))+m14*(m14*m3-m12*m5)
                                            +m2*(m12*m20-m14*m17))
                     *m6)
                +m5*(m7*(m12*(m10*m3-m5*m8)+m2*(m17*m8-m10*m15)
                                           -(m17*m3-m15*m5)*m7)
                    -m8*(m11*(m10*m3-m5*m8)+m2*(m14*m8-m10*m12)
                                           -(m14*m3-m12*m5)*m7)
                    +m1*((-m11*(m17*m8-m10*m15))+m12*(m14*m8-m10*m12)
                                                +(m12*m17-m14*m15)*m7)
                    -((-m11*(m17*m3-m15*m5))+m12*(m14*m3-m12*m5)
                                            +(m12*m17-m14*m15)*m2)
                     *m6)
                +m0*((-m7*((-m12*(m20*m8-m10*m17))+m14*(m17*m8-m10*m15)
                                                  +(m15*m20-m17^2)*m7))
                    +m8*((-m11*(m20*m8-m10*m17))+m14*(m14*m8-m10*m12)
                                                +(m12*m20-m14*m17)*m7)
                    -m10*((-m11*(m17*m8-m10*m15))+m12*(m14*m8-m10*m12)
                                                 +(m12*m17-m14*m15)*m7)
                    +(m11*(m15*m20-m17^2)-m12*(m12*m20-m14*m17)
                                         +m14*(m12*m17-m14*m15))
                     *m6)
                -m1*(((-m11*(m20*m3-m17*m5))+m14*(m14*m3-m12*m5)
                                            +m2*(m12*m20-m14*m17))
                    *m8
                    -((-m12*(m20*m3-m17*m5))+m14*(m17*m3-m15*m5)
                                            +m2*(m15*m20-m17^2))
                     *m7
                    -m10*((-m11*(m17*m3-m15*m5))+m12*(m14*m3-m12*m5)
                                                +(m12*m17-m14*m15)*m2)
                    +m1*(m11*(m15*m20-m17^2)-m12*(m12*m20-m14*m17)
                                            +m14*(m12*m17-m14*m15))),
               (-m2*(m8*(m14*(m3*m9-m4*m8)+m2*(m19*m8-m17*m9)
                                          -(m19*m3-m17*m4)*m7)
                    -m10*(m12*(m3*m9-m4*m8)+m2*(m16*m8-m15*m9)
                                           -(m16*m3-m15*m4)*m7)
                    +m1*((-m12*(m19*m8-m17*m9))+m14*(m16*m8-m15*m9)
                                               +(m15*m19-m16*m17)*m7)
                    -((-m12*(m19*m3-m17*m4))+m14*(m16*m3-m15*m4)
                                            +(m15*m19-m16*m17)*m2)
                     *m6))
                +m3*(m7*(m14*(m3*m9-m4*m8)+m2*(m19*m8-m17*m9)
                                          -(m19*m3-m17*m4)*m7)
                    -m10*(m11*(m3*m9-m4*m8)+m2*(m13*m8-m12*m9)
                                           -(m13*m3-m12*m4)*m7)
                    +m1*((-m11*(m19*m8-m17*m9))+m14*(m13*m8-m12*m9)
                                               +(m12*m19-m13*m17)*m7)
                    -((-m11*(m19*m3-m17*m4))+m14*(m13*m3-m12*m4)
                                            +(m12*m19-m13*m17)*m2)
                     *m6)
                -m5*(m7*(m12*(m3*m9-m4*m8)+m2*(m16*m8-m15*m9)
                                          -(m16*m3-m15*m4)*m7)
                    -m8*(m11*(m3*m9-m4*m8)+m2*(m13*m8-m12*m9)
                                          -(m13*m3-m12*m4)*m7)
                    +m1*((-m11*(m16*m8-m15*m9))+m12*(m13*m8-m12*m9)
                                               +(m12*m16-m13*m15)*m7)
                    -((-m11*(m16*m3-m15*m4))+m12*(m13*m3-m12*m4)
                                            +(m12*m16-m13*m15)*m2)
                     *m6)
                -m0*((-m7*((-m12*(m19*m8-m17*m9))+m14*(m16*m8-m15*m9)
                                                 +(m15*m19-m16*m17)*m7))
                    +m8*((-m11*(m19*m8-m17*m9))+m14*(m13*m8-m12*m9)
                                               +(m12*m19-m13*m17)*m7)
                    -m10*((-m11*(m16*m8-m15*m9))+m12*(m13*m8-m12*m9)
                                                +(m12*m16-m13*m15)*m7)
                    +(m11*(m15*m19-m16*m17)-m12*(m12*m19-m13*m17)
                                           +m14*(m12*m16-m13*m15))
                     *m6)
                +m1*(((-m11*(m19*m3-m17*m4))+m14*(m13*m3-m12*m4)
                                            +(m12*m19-m13*m17)*m2)
                    *m8
                    -((-m12*(m19*m3-m17*m4))+m14*(m16*m3-m15*m4)
                                            +(m15*m19-m16*m17)*m2)
                     *m7
                    -m10*((-m11*(m16*m3-m15*m4))+m12*(m13*m3-m12*m4)
                                                +(m12*m16-m13*m15)*m2)
                    +m1*(m11*(m15*m19-m16*m17)-m12*(m12*m19-m13*m17)
                                              +m14*(m12*m16-m13*m15)))],
              [(-m7*(m12*(m16*(m10*m4-m5*m9)+m3*(m19*m9-m10*m18)
                                            -(m19*m4-m18*m5)*m8)
                    -m13*(m15*(m10*m4-m5*m9)+m3*(m17*m9-m10*m16)
                                            -(m17*m4-m16*m5)*m8)
                    +m2*((-m15*(m19*m9-m10*m18))+m16*(m17*m9-m10*m16)
                                                +(m16*m19-m17*m18)*m8)
                    -((-m15*(m19*m4-m18*m5))+m16*(m17*m4-m16*m5)
                                            +(m16*m19-m17*m18)*m3)
                     *m7))
                +m8*(m11*(m16*(m10*m4-m5*m9)+m3*(m19*m9-m10*m18)
                                            -(m19*m4-m18*m5)*m8)
                    -m13*(m12*(m10*m4-m5*m9)+m3*(m14*m9-m10*m13)
                                            -(m14*m4-m13*m5)*m8)
                    +m2*((-m12*(m19*m9-m10*m18))+m16*(m14*m9-m10*m13)
                                                +(m13*m19-m14*m18)*m8)
                    -((-m12*(m19*m4-m18*m5))+m16*(m14*m4-m13*m5)
                                            +(m13*m19-m14*m18)*m3)
                     *m7)
                -m9*(m11*(m15*(m10*m4-m5*m9)+m3*(m17*m9-m10*m16)
                                            -(m17*m4-m16*m5)*m8)
                    -m12*(m12*(m10*m4-m5*m9)+m3*(m14*m9-m10*m13)
                                            -(m14*m4-m13*m5)*m8)
                    +m2*((-m12*(m17*m9-m10*m16))+m15*(m14*m9-m10*m13)
                                                +(m13*m17-m14*m16)*m8)
                    -((-m12*(m17*m4-m16*m5))+m15*(m14*m4-m13*m5)
                                            +(m13*m17-m14*m16)*m3)
                     *m7)
                -m1*((-m11*((-m15*(m19*m9-m10*m18))+m16*(m17*m9-m10*m16)
                                                   +(m16*m19-m17*m18)*m8))
                    +m12*((-m12*(m19*m9-m10*m18))+m16*(m14*m9-m10*m13)
                                                 +(m13*m19-m14*m18)*m8)
                    -m13*((-m12*(m17*m9-m10*m16))+m15*(m14*m9-m10*m13)
                                                 +(m13*m17-m14*m16)*m8)
                    +(m12*(m16*m19-m17*m18)-m15*(m13*m19-m14*m18)
                                           +m16*(m13*m17-m14*m16))
                     *m7)
                +((-m11*((-m15*(m19*m4-m18*m5))+m16*(m17*m4-m16*m5)
                                               +(m16*m19-m17*m18)*m3))
                 +m12*((-m12*(m19*m4-m18*m5))+m16*(m14*m4-m13*m5)
                                             +(m13*m19-m14*m18)*m3)
                 -m13*((-m12*(m17*m4-m16*m5))+m15*(m14*m4-m13*m5)
                                             +(m13*m17-m14*m16)*m3)
                 +(m12*(m16*m19-m17*m18)-m15*(m13*m19-m14*m18)
                                        +m16*(m13*m17-m14*m16))
                  *m2)
                 *m6,
               m2*(m12*(m16*(m10*m4-m5*m9)+m3*(m19*m9-m10*m18)
                                          -(m19*m4-m18*m5)*m8)
                  -m13*(m15*(m10*m4-m5*m9)+m3*(m17*m9-m10*m16)
                                          -(m17*m4-m16*m5)*m8)
                  +m2*((-m15*(m19*m9-m10*m18))+m16*(m17*m9-m10*m16)
                                              +(m16*m19-m17*m18)*m8)
                  -((-m15*(m19*m4-m18*m5))+m16*(m17*m4-m16*m5)
                                          +(m16*m19-m17*m18)*m3)
                   *m7)
                -m3*(m11*(m16*(m10*m4-m5*m9)+m3*(m19*m9-m10*m18)
                                            -(m19*m4-m18*m5)*m8)
                    -m13*(m12*(m10*m4-m5*m9)+m3*(m14*m9-m10*m13)
                                            -(m14*m4-m13*m5)*m8)
                    +m2*((-m12*(m19*m9-m10*m18))+m16*(m14*m9-m10*m13)
                                                +(m13*m19-m14*m18)*m8)
                    -((-m12*(m19*m4-m18*m5))+m16*(m14*m4-m13*m5)
                                            +(m13*m19-m14*m18)*m3)
                     *m7)
                +m4*(m11*(m15*(m10*m4-m5*m9)+m3*(m17*m9-m10*m16)
                                            -(m17*m4-m16*m5)*m8)
                    -m12*(m12*(m10*m4-m5*m9)+m3*(m14*m9-m10*m13)
                                            -(m14*m4-m13*m5)*m8)
                    +m2*((-m12*(m17*m9-m10*m16))+m15*(m14*m9-m10*m13)
                                                +(m13*m17-m14*m16)*m8)
                    -((-m12*(m17*m4-m16*m5))+m15*(m14*m4-m13*m5)
                                            +(m13*m17-m14*m16)*m3)
                     *m7)
                +m0*((-m11*((-m15*(m19*m9-m10*m18))+m16*(m17*m9-m10*m16)
                                                   +(m16*m19-m17*m18)*m8))
                    +m12*((-m12*(m19*m9-m10*m18))+m16*(m14*m9-m10*m13)
                                                 +(m13*m19-m14*m18)*m8)
                    -m13*((-m12*(m17*m9-m10*m16))+m15*(m14*m9-m10*m13)
                                                 +(m13*m17-m14*m16)*m8)
                    +(m12*(m16*m19-m17*m18)-m15*(m13*m19-m14*m18)
                                           +m16*(m13*m17-m14*m16))
                     *m7)
                -m1*((-m11*((-m15*(m19*m4-m18*m5))+m16*(m17*m4-m16*m5)
                                                  +(m16*m19-m17*m18)*m3))
                    +m12*((-m12*(m19*m4-m18*m5))+m16*(m14*m4-m13*m5)
                                                +(m13*m19-m14*m18)*m3)
                    -m13*((-m12*(m17*m4-m16*m5))+m15*(m14*m4-m13*m5)
                                                +(m13*m17-m14*m16)*m3)
                    +(m12*(m16*m19-m17*m18)-m15*(m13*m19-m14*m18)
                                           +m16*(m13*m17-m14*m16))
                     *m2),
               (-m2*(m8*(m16*(m10*m4-m5*m9)+m3*(m19*m9-m10*m18)
                                           -(m19*m4-m18*m5)*m8)
                    -m9*(m15*(m10*m4-m5*m9)+m3*(m17*m9-m10*m16)
                                           -(m17*m4-m16*m5)*m8)
                    +m1*((-m15*(m19*m9-m10*m18))+m16*(m17*m9-m10*m16)
                                                +(m16*m19-m17*m18)*m8)
                    -((-m15*(m19*m4-m18*m5))+m16*(m17*m4-m16*m5)
                                            +(m16*m19-m17*m18)*m3)
                     *m6))
                +m3*(m7*(m16*(m10*m4-m5*m9)+m3*(m19*m9-m10*m18)
                                           -(m19*m4-m18*m5)*m8)
                    -m9*(m12*(m10*m4-m5*m9)+m3*(m14*m9-m10*m13)
                                           -(m14*m4-m13*m5)*m8)
                    +m1*((-m12*(m19*m9-m10*m18))+m16*(m14*m9-m10*m13)
                                                +(m13*m19-m14*m18)*m8)
                    -((-m12*(m19*m4-m18*m5))+m16*(m14*m4-m13*m5)
                                            +(m13*m19-m14*m18)*m3)
                     *m6)
                -m4*(m7*(m15*(m10*m4-m5*m9)+m3*(m17*m9-m10*m16)
                                           -(m17*m4-m16*m5)*m8)
                    -m8*(m12*(m10*m4-m5*m9)+m3*(m14*m9-m10*m13)
                                           -(m14*m4-m13*m5)*m8)
                    +m1*((-m12*(m17*m9-m10*m16))+m15*(m14*m9-m10*m13)
                                                +(m13*m17-m14*m16)*m8)
                    -((-m12*(m17*m4-m16*m5))+m15*(m14*m4-m13*m5)
                                            +(m13*m17-m14*m16)*m3)
                     *m6)
                -m0*((-m7*((-m15*(m19*m9-m10*m18))+m16*(m17*m9-m10*m16)
                                                  +(m16*m19-m17*m18)*m8))
                    +m8*((-m12*(m19*m9-m10*m18))+m16*(m14*m9-m10*m13)
                                                +(m13*m19-m14*m18)*m8)
                    -m9*((-m12*(m17*m9-m10*m16))+m15*(m14*m9-m10*m13)
                                                +(m13*m17-m14*m16)*m8)
                    +(m12*(m16*m19-m17*m18)-m15*(m13*m19-m14*m18)
                                           +m16*(m13*m17-m14*m16))
                     *m6)
                +m1*((-((-m12*(m17*m4-m16*m5))+m15*(m14*m4-m13*m5)
                                              +(m13*m17-m14*m16)*m3)
                    *m9)
                    +((-m12*(m19*m4-m18*m5))+m16*(m14*m4-m13*m5)
                                            +(m13*m19-m14*m18)*m3)
                     *m8
                    -((-m15*(m19*m4-m18*m5))+m16*(m17*m4-m16*m5)
                                            +(m16*m19-m17*m18)*m3)
                     *m7
                    +m1*(m12*(m16*m19-m17*m18)-m15*(m13*m19-m14*m18)
                                              +m16*(m13*m17-m14*m16))),
               m2*(m8*(m13*(m10*m4-m5*m9)+m2*(m19*m9-m10*m18)
                                         -(m19*m4-m18*m5)*m7)
                  -m9*(m12*(m10*m4-m5*m9)+m2*(m17*m9-m10*m16)
                                         -(m17*m4-m16*m5)*m7)
                  +m1*((-m12*(m19*m9-m10*m18))+m13*(m17*m9-m10*m16)
                                              +(m16*m19-m17*m18)*m7)
                  -((-m12*(m19*m4-m18*m5))+m13*(m17*m4-m16*m5)
                                          +(m16*m19-m17*m18)*m2)
                   *m6)
                -m3*(m7*(m13*(m10*m4-m5*m9)+m2*(m19*m9-m10*m18)
                                           -(m19*m4-m18*m5)*m7)
                    -m9*(m11*(m10*m4-m5*m9)+m2*(m14*m9-m10*m13)
                                           -(m14*m4-m13*m5)*m7)
                    +m1*((-m11*(m19*m9-m10*m18))+m13*(m14*m9-m10*m13)
                                                +(m13*m19-m14*m18)*m7)
                    -((-m11*(m19*m4-m18*m5))+m13*(m14*m4-m13*m5)
                                            +(m13*m19-m14*m18)*m2)
                     *m6)
                +m4*(m7*(m12*(m10*m4-m5*m9)+m2*(m17*m9-m10*m16)
                                           -(m17*m4-m16*m5)*m7)
                    -m8*(m11*(m10*m4-m5*m9)+m2*(m14*m9-m10*m13)
                                           -(m14*m4-m13*m5)*m7)
                    +m1*((-m11*(m17*m9-m10*m16))+m12*(m14*m9-m10*m13)
                                                +(m13*m17-m14*m16)*m7)
                    -((-m11*(m17*m4-m16*m5))+m12*(m14*m4-m13*m5)
                                            +(m13*m17-m14*m16)*m2)
                     *m6)
                +m0*((-m7*((-m12*(m19*m9-m10*m18))+m13*(m17*m9-m10*m16)
                                                  +(m16*m19-m17*m18)*m7))
                    +m8*((-m11*(m19*m9-m10*m18))+m13*(m14*m9-m10*m13)
                                                +(m13*m19-m14*m18)*m7)
                    -m9*((-m11*(m17*m9-m10*m16))+m12*(m14*m9-m10*m13)
                                                +(m13*m17-m14*m16)*m7)
                    +(m11*(m16*m19-m17*m18)-m12*(m13*m19-m14*m18)
                                           +m13*(m13*m17-m14*m16))
                     *m6)
                -m1*((-((-m11*(m17*m4-m16*m5))+m12*(m14*m4-m13*m5)
                                              +(m13*m17-m14*m16)*m2)
                    *m9)
                    +((-m11*(m19*m4-m18*m5))+m13*(m14*m4-m13*m5)
                                            +(m13*m19-m14*m18)*m2)
                     *m8
                    -((-m12*(m19*m4-m18*m5))+m13*(m17*m4-m16*m5)
                                            +(m16*m19-m17*m18)*m2)
                     *m7
                    +m1*(m11*(m16*m19-m17*m18)-m12*(m13*m19-m14*m18)
                                              +m13*(m13*m17-m14*m16))),
               (-m2*((-(m12*(m10*m3-m5*m8)+m2*(m17*m8-m10*m15)
                                          -(m17*m3-m15*m5)*m7)
                    *m9)
                    +m8*(m13*(m10*m3-m5*m8)+m2*(m19*m8-m10*m16)
                                           -(m19*m3-m16*m5)*m7)
                    +m1*((-m12*(m19*m8-m10*m16))+m13*(m17*m8-m10*m15)
                                                +(m15*m19-m16*m17)*m7)
                    -((-m12*(m19*m3-m16*m5))+m13*(m17*m3-m15*m5)
                                            +(m15*m19-m16*m17)*m2)
                     *m6))
                +m3*((-(m11*(m10*m3-m5*m8)+m2*(m14*m8-m10*m12)
                                          -(m14*m3-m12*m5)*m7)
                    *m9)
                    +m7*(m13*(m10*m3-m5*m8)+m2*(m19*m8-m10*m16)
                                           -(m19*m3-m16*m5)*m7)
                    +m1*((-m11*(m19*m8-m10*m16))+m13*(m14*m8-m10*m12)
                                                +(m12*m19-m14*m16)*m7)
                    -((-m11*(m19*m3-m16*m5))+m13*(m14*m3-m12*m5)
                                            +(m12*m19-m14*m16)*m2)
                     *m6)
                -m0*((-((-m11*(m17*m8-m10*m15))+m12*(m14*m8-m10*m12)
                                               +(m12*m17-m14*m15)*m7)
                    *m9)
                    -m7*((-m12*(m19*m8-m10*m16))+m13*(m17*m8-m10*m15)
                                                +(m15*m19-m16*m17)*m7)
                    +m8*((-m11*(m19*m8-m10*m16))+m13*(m14*m8-m10*m12)
                                                +(m12*m19-m14*m16)*m7)
                    +(m11*(m15*m19-m16*m17)-m12*(m12*m19-m14*m16)
                                           +m13*(m12*m17-m14*m15))
                     *m6)
                +m1*((-((-m11*(m17*m3-m15*m5))+m12*(m14*m3-m12*m5)
                                              +(m12*m17-m14*m15)*m2)
                    *m9)
                    +((-m11*(m19*m3-m16*m5))+m13*(m14*m3-m12*m5)
                                            +(m12*m19-m14*m16)*m2)
                     *m8
                    -((-m12*(m19*m3-m16*m5))+m13*(m17*m3-m15*m5)
                                            +(m15*m19-m16*m17)*m2)
                     *m7
                    +m1*(m11*(m15*m19-m16*m17)-m12*(m12*m19-m14*m16)
                                              +m13*(m12*m17-m14*m15)))
                -m4*(m7*(m12*(m10*m3-m5*m8)+m2*(m17*m8-m10*m15)
                                           -(m17*m3-m15*m5)*m7)
                    -m8*(m11*(m10*m3-m5*m8)+m2*(m14*m8-m10*m12)
                                           -(m14*m3-m12*m5)*m7)
                    +m1*((-m11*(m17*m8-m10*m15))+m12*(m14*m8-m10*m12)
                                                +(m12*m17-m14*m15)*m7)
                    -((-m11*(m17*m3-m15*m5))+m12*(m14*m3-m12*m5)
                                            +(m12*m17-m14*m15)*m2)
                     *m6),
               m2*(m8*(m13*(m3*m9-m4*m8)+m2*(m18*m8-m16*m9)
                                        -(m18*m3-m16*m4)*m7)
                  -m9*(m12*(m3*m9-m4*m8)+m2*(m16*m8-m15*m9)
                                        -(m16*m3-m15*m4)*m7)
                  +m1*((-m12*(m18*m8-m16*m9))+m13*(m16*m8-m15*m9)
                                             +(m15*m18-m16^2)*m7)
                  -((-m12*(m18*m3-m16*m4))+m13*(m16*m3-m15*m4)
                                          +(m15*m18-m16^2)*m2)
                   *m6)
                -m3*(m7*(m13*(m3*m9-m4*m8)+m2*(m18*m8-m16*m9)
                                          -(m18*m3-m16*m4)*m7)
                    -m9*(m11*(m3*m9-m4*m8)+m2*(m13*m8-m12*m9)
                                          -(m13*m3-m12*m4)*m7)
                    +m1*((-m11*(m18*m8-m16*m9))+m13*(m13*m8-m12*m9)
                                               +(m12*m18-m13*m16)*m7)
                    -((-m11*(m18*m3-m16*m4))+m13*(m13*m3-m12*m4)
                                            +(m12*m18-m13*m16)*m2)
                     *m6)
                +m4*(m7*(m12*(m3*m9-m4*m8)+m2*(m16*m8-m15*m9)
                                          -(m16*m3-m15*m4)*m7)
                    -m8*(m11*(m3*m9-m4*m8)+m2*(m13*m8-m12*m9)
                                          -(m13*m3-m12*m4)*m7)
                    +m1*((-m11*(m16*m8-m15*m9))+m12*(m13*m8-m12*m9)
                                               +(m12*m16-m13*m15)*m7)
                    -((-m11*(m16*m3-m15*m4))+m12*(m13*m3-m12*m4)
                                            +(m12*m16-m13*m15)*m2)
                     *m6)
                +m0*((-m7*((-m12*(m18*m8-m16*m9))+m13*(m16*m8-m15*m9)
                                                 +(m15*m18-m16^2)*m7))
                    +m8*((-m11*(m18*m8-m16*m9))+m13*(m13*m8-m12*m9)
                                               +(m12*m18-m13*m16)*m7)
                    -m9*((-m11*(m16*m8-m15*m9))+m12*(m13*m8-m12*m9)
                                               +(m12*m16-m13*m15)*m7)
                    +(m11*(m15*m18-m16^2)-m12*(m12*m18-m13*m16)
                                         +m13*(m12*m16-m13*m15))
                     *m6)
                -m1*((-((-m11*(m16*m3-m15*m4))+m12*(m13*m3-m12*m4)
                                              +(m12*m16-m13*m15)*m2)
                    *m9)
                    +((-m11*(m18*m3-m16*m4))+m13*(m13*m3-m12*m4)
                                            +(m12*m18-m13*m16)*m2)
                     *m8
                    -((-m12*(m18*m3-m16*m4))+m13*(m16*m3-m15*m4)
                                            +(m15*m18-m16^2)*m2)
                     *m7
                    +m1*(m11*(m15*m18-m16^2)-m12*(m12*m18-m13*m16)
                                            +m13*(m12*m16-m13*m15)))])

Test program:

    void main(void)
    {
        const double a[] =
            { 1.9826929 , 0.49539104, 1.21536243, 0.98610923, 1.68623959, 1.0331091 ,
                          1.96643809, 1.42962549, 0.9336305 , 1.96542156, 0.6086516 ,
                                      0.81542249, 0.74012536, 0.83940333, 1.58604071,
                                                  0.1338364 , 1.03314221, 0.44817192,
                                                              1.97146512, 0.27591278,
                                                                          1.51474051 };

        double c[21];

        int N=6;
        double det = cofactors_sym6(a, c);

        printf("det = %f\n", det);
        printf("\n\n");

        double p[36] = {};

        for(int iout=0; iout<N; iout++)
        {
            for(int jout=0; jout<N; jout++)
            {
                for(int k=0; k<N; k++)
                {
                    p[iout*N + jout] +=
                        a[index_sym66(iout, k)] *
                        c[index_sym66(k, jout)];
                }
                p[iout*N + jout] /= det;

                printf("%.5f ", p[iout*N + jout]);
            }
            printf("\n");
        }
    }

     */

    c[ 0] = (-m[ 7]*((-m[12]*((-m[16]*(m[20]*m[ 9]-m[10]*m[19]))+m[17]*(m[19]*m[ 9]-m[10]*m[18])+(m[18]*m[20]-m[19]*m[19])*m[ 8]))+m[13]*((-m[15]*(m[20]*m[ 9]-m[10]*m[19]))+m[17]*(m[17]*m[ 9]-m[10]*m[16])+(m[16]*m[20]-m[17]*m[19])*m[ 8])-m[14]*((-m[15]*(m[19]*m[ 9]-m[10]*m[18]))+m[16]*(m[17]*m[ 9]-m[10]*m[16])+(m[16]*m[19]-m[17]*m[18])*m[ 8])+(m[15]*(m[18]*m[20]-m[19]*m[19])-m[16]*(m[16]*m[20]-m[17]*m[19])+m[17]*(m[16]*m[19]-m[17]*m[18]))*m[ 7]))+m[ 8]*((-m[11]*((-m[16]*(m[20]*m[ 9]-m[10]*m[19]))+m[17]*(m[19]*m[ 9]-m[10]*m[18])+(m[18]*m[20]-m[19]*m[19])*m[ 8]))+m[13]*((-m[12]*(m[20]*m[ 9]-m[10]*m[19]))+m[17]*(m[14]*m[ 9]-m[10]*m[13])+(m[13]*m[20]-m[14]*m[19])*m[ 8])-m[14]*((-m[12]*(m[19]*m[ 9]-m[10]*m[18]))+m[16]*(m[14]*m[ 9]-m[10]*m[13])+(m[13]*m[19]-m[14]*m[18])*m[ 8])+(m[12]*(m[18]*m[20]-m[19]*m[19])-m[16]*(m[13]*m[20]-m[14]*m[19])+m[17]*(m[13]*m[19]-m[14]*m[18]))*m[ 7])-m[ 9]*((-m[11]*((-m[15]*(m[20]*m[ 9]-m[10]*m[19]))+m[17]*(m[17]*m[ 9]-m[10]*m[16])+(m[16]*m[20]-m[17]*m[19])*m[ 8]))+m[12]*((-m[12]*(m[20]*m[ 9]-m[10]*m[19]))+m[17]*(m[14]*m[ 9]-m[10]*m[13])+(m[13]*m[20]-m[14]*m[19])*m[ 8])-m[14]*((-m[12]*(m[17]*m[ 9]-m[10]*m[16]))+m[15]*(m[14]*m[ 9]-m[10]*m[13])+(m[13]*m[17]-m[14]*m[16])*m[ 8])+(m[12]*(m[16]*m[20]-m[17]*m[19])-m[15]*(m[13]*m[20]-m[14]*m[19])+m[17]*(m[13]*m[17]-m[14]*m[16]))*m[ 7])+m[10]*((-m[11]*((-m[15]*(m[19]*m[ 9]-m[10]*m[18]))+m[16]*(m[17]*m[ 9]-m[10]*m[16])+(m[16]*m[19]-m[17]*m[18])*m[ 8]))+m[12]*((-m[12]*(m[19]*m[ 9]-m[10]*m[18]))+m[16]*(m[14]*m[ 9]-m[10]*m[13])+(m[13]*m[19]-m[14]*m[18])*m[ 8])-m[13]*((-m[12]*(m[17]*m[ 9]-m[10]*m[16]))+m[15]*(m[14]*m[ 9]-m[10]*m[13])+(m[13]*m[17]-m[14]*m[16])*m[ 8])+(m[12]*(m[16]*m[19]-m[17]*m[18])-m[15]*(m[13]*m[19]-m[14]*m[18])+m[16]*(m[13]*m[17]-m[14]*m[16]))*m[ 7])+(m[11]*(m[15]*(m[18]*m[20]-m[19]*m[19])-m[16]*(m[16]*m[20]-m[17]*m[19])+m[17]*(m[16]*m[19]-m[17]*m[18]))-m[12]*(m[12]*(m[18]*m[20]-m[19]*m[19])-m[16]*(m[13]*m[20]-m[14]*m[19])+m[17]*(m[13]*m[19]-m[14]*m[18]))+m[13]*(m[12]*(m[16]*m[20]-m[17]*m[19])-m[15]*(m[13]*m[20]-m[14]*m[19])+m[17]*(m[13]*m[17]-m[14]*m[16]))-m[14]*(m[12]*(m[16]*m[19]-m[17]*m[18])-m[15]*(m[13]*m[19]-m[14]*m[18])+m[16]*(m[13]*m[17]-m[14]*m[16])))*m[ 6];
    c[ 1] = m[ 2]*((-m[12]*((-m[16]*(m[20]*m[ 9]-m[10]*m[19]))+m[17]*(m[19]*m[ 9]-m[10]*m[18])+(m[18]*m[20]-m[19]*m[19])*m[ 8]))+m[13]*((-m[15]*(m[20]*m[ 9]-m[10]*m[19]))+m[17]*(m[17]*m[ 9]-m[10]*m[16])+(m[16]*m[20]-m[17]*m[19])*m[ 8])-m[14]*((-m[15]*(m[19]*m[ 9]-m[10]*m[18]))+m[16]*(m[17]*m[ 9]-m[10]*m[16])+(m[16]*m[19]-m[17]*m[18])*m[ 8])+(m[15]*(m[18]*m[20]-m[19]*m[19])-m[16]*(m[16]*m[20]-m[17]*m[19])+m[17]*(m[16]*m[19]-m[17]*m[18]))*m[ 7])-m[ 3]*((-m[11]*((-m[16]*(m[20]*m[ 9]-m[10]*m[19]))+m[17]*(m[19]*m[ 9]-m[10]*m[18])+(m[18]*m[20]-m[19]*m[19])*m[ 8]))+m[13]*((-m[12]*(m[20]*m[ 9]-m[10]*m[19]))+m[17]*(m[14]*m[ 9]-m[10]*m[13])+(m[13]*m[20]-m[14]*m[19])*m[ 8])-m[14]*((-m[12]*(m[19]*m[ 9]-m[10]*m[18]))+m[16]*(m[14]*m[ 9]-m[10]*m[13])+(m[13]*m[19]-m[14]*m[18])*m[ 8])+(m[12]*(m[18]*m[20]-m[19]*m[19])-m[16]*(m[13]*m[20]-m[14]*m[19])+m[17]*(m[13]*m[19]-m[14]*m[18]))*m[ 7])+m[ 4]*((-m[11]*((-m[15]*(m[20]*m[ 9]-m[10]*m[19]))+m[17]*(m[17]*m[ 9]-m[10]*m[16])+(m[16]*m[20]-m[17]*m[19])*m[ 8]))+m[12]*((-m[12]*(m[20]*m[ 9]-m[10]*m[19]))+m[17]*(m[14]*m[ 9]-m[10]*m[13])+(m[13]*m[20]-m[14]*m[19])*m[ 8])-m[14]*((-m[12]*(m[17]*m[ 9]-m[10]*m[16]))+m[15]*(m[14]*m[ 9]-m[10]*m[13])+(m[13]*m[17]-m[14]*m[16])*m[ 8])+(m[12]*(m[16]*m[20]-m[17]*m[19])-m[15]*(m[13]*m[20]-m[14]*m[19])+m[17]*(m[13]*m[17]-m[14]*m[16]))*m[ 7])-m[ 5]*((-m[11]*((-m[15]*(m[19]*m[ 9]-m[10]*m[18]))+m[16]*(m[17]*m[ 9]-m[10]*m[16])+(m[16]*m[19]-m[17]*m[18])*m[ 8]))+m[12]*((-m[12]*(m[19]*m[ 9]-m[10]*m[18]))+m[16]*(m[14]*m[ 9]-m[10]*m[13])+(m[13]*m[19]-m[14]*m[18])*m[ 8])-m[13]*((-m[12]*(m[17]*m[ 9]-m[10]*m[16]))+m[15]*(m[14]*m[ 9]-m[10]*m[13])+(m[13]*m[17]-m[14]*m[16])*m[ 8])+(m[12]*(m[16]*m[19]-m[17]*m[18])-m[15]*(m[13]*m[19]-m[14]*m[18])+m[16]*(m[13]*m[17]-m[14]*m[16]))*m[ 7])-m[ 1]*(m[11]*(m[15]*(m[18]*m[20]-m[19]*m[19])-m[16]*(m[16]*m[20]-m[17]*m[19])+m[17]*(m[16]*m[19]-m[17]*m[18]))-m[12]*(m[12]*(m[18]*m[20]-m[19]*m[19])-m[16]*(m[13]*m[20]-m[14]*m[19])+m[17]*(m[13]*m[19]-m[14]*m[18]))+m[13]*(m[12]*(m[16]*m[20]-m[17]*m[19])-m[15]*(m[13]*m[20]-m[14]*m[19])+m[17]*(m[13]*m[17]-m[14]*m[16]))-m[14]*(m[12]*(m[16]*m[19]-m[17]*m[18])-m[15]*(m[13]*m[19]-m[14]*m[18])+m[16]*(m[13]*m[17]-m[14]*m[16])));
    c[ 2] = (-m[ 2]*((-m[ 8]*((-m[16]*(m[20]*m[ 9]-m[10]*m[19]))+m[17]*(m[19]*m[ 9]-m[10]*m[18])+(m[18]*m[20]-m[19]*m[19])*m[ 8]))+m[ 9]*((-m[15]*(m[20]*m[ 9]-m[10]*m[19]))+m[17]*(m[17]*m[ 9]-m[10]*m[16])+(m[16]*m[20]-m[17]*m[19])*m[ 8])-m[10]*((-m[15]*(m[19]*m[ 9]-m[10]*m[18]))+m[16]*(m[17]*m[ 9]-m[10]*m[16])+(m[16]*m[19]-m[17]*m[18])*m[ 8])+(m[15]*(m[18]*m[20]-m[19]*m[19])-m[16]*(m[16]*m[20]-m[17]*m[19])+m[17]*(m[16]*m[19]-m[17]*m[18]))*m[ 6]))+m[ 3]*((-m[ 7]*((-m[16]*(m[20]*m[ 9]-m[10]*m[19]))+m[17]*(m[19]*m[ 9]-m[10]*m[18])+(m[18]*m[20]-m[19]*m[19])*m[ 8]))+m[ 9]*((-m[12]*(m[20]*m[ 9]-m[10]*m[19]))+m[17]*(m[14]*m[ 9]-m[10]*m[13])+(m[13]*m[20]-m[14]*m[19])*m[ 8])-m[10]*((-m[12]*(m[19]*m[ 9]-m[10]*m[18]))+m[16]*(m[14]*m[ 9]-m[10]*m[13])+(m[13]*m[19]-m[14]*m[18])*m[ 8])+(m[12]*(m[18]*m[20]-m[19]*m[19])-m[16]*(m[13]*m[20]-m[14]*m[19])+m[17]*(m[13]*m[19]-m[14]*m[18]))*m[ 6])-m[ 4]*((-m[ 7]*((-m[15]*(m[20]*m[ 9]-m[10]*m[19]))+m[17]*(m[17]*m[ 9]-m[10]*m[16])+(m[16]*m[20]-m[17]*m[19])*m[ 8]))+m[ 8]*((-m[12]*(m[20]*m[ 9]-m[10]*m[19]))+m[17]*(m[14]*m[ 9]-m[10]*m[13])+(m[13]*m[20]-m[14]*m[19])*m[ 8])-m[10]*((-m[12]*(m[17]*m[ 9]-m[10]*m[16]))+m[15]*(m[14]*m[ 9]-m[10]*m[13])+(m[13]*m[17]-m[14]*m[16])*m[ 8])+(m[12]*(m[16]*m[20]-m[17]*m[19])-m[15]*(m[13]*m[20]-m[14]*m[19])+m[17]*(m[13]*m[17]-m[14]*m[16]))*m[ 6])+m[ 5]*((-m[ 7]*((-m[15]*(m[19]*m[ 9]-m[10]*m[18]))+m[16]*(m[17]*m[ 9]-m[10]*m[16])+(m[16]*m[19]-m[17]*m[18])*m[ 8]))+m[ 8]*((-m[12]*(m[19]*m[ 9]-m[10]*m[18]))+m[16]*(m[14]*m[ 9]-m[10]*m[13])+(m[13]*m[19]-m[14]*m[18])*m[ 8])-m[ 9]*((-m[12]*(m[17]*m[ 9]-m[10]*m[16]))+m[15]*(m[14]*m[ 9]-m[10]*m[13])+(m[13]*m[17]-m[14]*m[16])*m[ 8])+(m[12]*(m[16]*m[19]-m[17]*m[18])-m[15]*(m[13]*m[19]-m[14]*m[18])+m[16]*(m[13]*m[17]-m[14]*m[16]))*m[ 6])+m[ 1]*((m[12]*(m[16]*m[20]-m[17]*m[19])-m[15]*(m[13]*m[20]-m[14]*m[19])+m[17]*(m[13]*m[17]-m[14]*m[16]))*m[ 9]-(m[12]*(m[18]*m[20]-m[19]*m[19])-m[16]*(m[13]*m[20]-m[14]*m[19])+m[17]*(m[13]*m[19]-m[14]*m[18]))*m[ 8]+(m[15]*(m[18]*m[20]-m[19]*m[19])-m[16]*(m[16]*m[20]-m[17]*m[19])+m[17]*(m[16]*m[19]-m[17]*m[18]))*m[ 7]-m[10]*(m[12]*(m[16]*m[19]-m[17]*m[18])-m[15]*(m[13]*m[19]-m[14]*m[18])+m[16]*(m[13]*m[17]-m[14]*m[16])));
    c[ 3] = m[ 2]*((-m[ 8]*((-m[13]*(m[20]*m[ 9]-m[10]*m[19]))+m[14]*(m[19]*m[ 9]-m[10]*m[18])+(m[18]*m[20]-m[19]*m[19])*m[ 7]))+m[ 9]*((-m[12]*(m[20]*m[ 9]-m[10]*m[19]))+m[14]*(m[17]*m[ 9]-m[10]*m[16])+(m[16]*m[20]-m[17]*m[19])*m[ 7])-m[10]*((-m[12]*(m[19]*m[ 9]-m[10]*m[18]))+m[13]*(m[17]*m[ 9]-m[10]*m[16])+(m[16]*m[19]-m[17]*m[18])*m[ 7])+(m[12]*(m[18]*m[20]-m[19]*m[19])-m[13]*(m[16]*m[20]-m[17]*m[19])+m[14]*(m[16]*m[19]-m[17]*m[18]))*m[ 6])-m[ 3]*((-m[ 7]*((-m[13]*(m[20]*m[ 9]-m[10]*m[19]))+m[14]*(m[19]*m[ 9]-m[10]*m[18])+(m[18]*m[20]-m[19]*m[19])*m[ 7]))+m[ 9]*((-m[11]*(m[20]*m[ 9]-m[10]*m[19]))+m[14]*(m[14]*m[ 9]-m[10]*m[13])+(m[13]*m[20]-m[14]*m[19])*m[ 7])-m[10]*((-m[11]*(m[19]*m[ 9]-m[10]*m[18]))+m[13]*(m[14]*m[ 9]-m[10]*m[13])+(m[13]*m[19]-m[14]*m[18])*m[ 7])+(m[11]*(m[18]*m[20]-m[19]*m[19])-m[13]*(m[13]*m[20]-m[14]*m[19])+m[14]*(m[13]*m[19]-m[14]*m[18]))*m[ 6])+m[ 4]*((-m[ 7]*((-m[12]*(m[20]*m[ 9]-m[10]*m[19]))+m[14]*(m[17]*m[ 9]-m[10]*m[16])+(m[16]*m[20]-m[17]*m[19])*m[ 7]))+m[ 8]*((-m[11]*(m[20]*m[ 9]-m[10]*m[19]))+m[14]*(m[14]*m[ 9]-m[10]*m[13])+(m[13]*m[20]-m[14]*m[19])*m[ 7])-m[10]*((-m[11]*(m[17]*m[ 9]-m[10]*m[16]))+m[12]*(m[14]*m[ 9]-m[10]*m[13])+(m[13]*m[17]-m[14]*m[16])*m[ 7])+(m[11]*(m[16]*m[20]-m[17]*m[19])-m[12]*(m[13]*m[20]-m[14]*m[19])+m[14]*(m[13]*m[17]-m[14]*m[16]))*m[ 6])-m[ 5]*((-m[ 7]*((-m[12]*(m[19]*m[ 9]-m[10]*m[18]))+m[13]*(m[17]*m[ 9]-m[10]*m[16])+(m[16]*m[19]-m[17]*m[18])*m[ 7]))+m[ 8]*((-m[11]*(m[19]*m[ 9]-m[10]*m[18]))+m[13]*(m[14]*m[ 9]-m[10]*m[13])+(m[13]*m[19]-m[14]*m[18])*m[ 7])-m[ 9]*((-m[11]*(m[17]*m[ 9]-m[10]*m[16]))+m[12]*(m[14]*m[ 9]-m[10]*m[13])+(m[13]*m[17]-m[14]*m[16])*m[ 7])+(m[11]*(m[16]*m[19]-m[17]*m[18])-m[12]*(m[13]*m[19]-m[14]*m[18])+m[13]*(m[13]*m[17]-m[14]*m[16]))*m[ 6])-m[ 1]*((m[11]*(m[16]*m[20]-m[17]*m[19])-m[12]*(m[13]*m[20]-m[14]*m[19])+m[14]*(m[13]*m[17]-m[14]*m[16]))*m[ 9]-(m[11]*(m[18]*m[20]-m[19]*m[19])-m[13]*(m[13]*m[20]-m[14]*m[19])+m[14]*(m[13]*m[19]-m[14]*m[18]))*m[ 8]+(m[12]*(m[18]*m[20]-m[19]*m[19])-m[13]*(m[16]*m[20]-m[17]*m[19])+m[14]*(m[16]*m[19]-m[17]*m[18]))*m[ 7]-m[10]*(m[11]*(m[16]*m[19]-m[17]*m[18])-m[12]*(m[13]*m[19]-m[14]*m[18])+m[13]*(m[13]*m[17]-m[14]*m[16])));
    c[ 4] = (-m[ 2]*(((-m[12]*(m[20]*m[ 8]-m[10]*m[17]))+m[14]*(m[17]*m[ 8]-m[10]*m[15])+(m[15]*m[20]-m[17]*m[17])*m[ 7])*m[ 9]-m[ 8]*((-m[13]*(m[20]*m[ 8]-m[10]*m[17]))+m[14]*(m[19]*m[ 8]-m[10]*m[16])+(m[16]*m[20]-m[17]*m[19])*m[ 7])-m[10]*((-m[12]*(m[19]*m[ 8]-m[10]*m[16]))+m[13]*(m[17]*m[ 8]-m[10]*m[15])+(m[15]*m[19]-m[16]*m[17])*m[ 7])+(m[12]*(m[16]*m[20]-m[17]*m[19])-m[13]*(m[15]*m[20]-m[17]*m[17])+m[14]*(m[15]*m[19]-m[16]*m[17]))*m[ 6]))+m[ 3]*(((-m[11]*(m[20]*m[ 8]-m[10]*m[17]))+m[14]*(m[14]*m[ 8]-m[10]*m[12])+(m[12]*m[20]-m[14]*m[17])*m[ 7])*m[ 9]-m[ 7]*((-m[13]*(m[20]*m[ 8]-m[10]*m[17]))+m[14]*(m[19]*m[ 8]-m[10]*m[16])+(m[16]*m[20]-m[17]*m[19])*m[ 7])-m[10]*((-m[11]*(m[19]*m[ 8]-m[10]*m[16]))+m[13]*(m[14]*m[ 8]-m[10]*m[12])+(m[12]*m[19]-m[14]*m[16])*m[ 7])+(m[11]*(m[16]*m[20]-m[17]*m[19])-m[13]*(m[12]*m[20]-m[14]*m[17])+m[14]*(m[12]*m[19]-m[14]*m[16]))*m[ 6])+m[ 5]*((-((-m[11]*(m[17]*m[ 8]-m[10]*m[15]))+m[12]*(m[14]*m[ 8]-m[10]*m[12])+(m[12]*m[17]-m[14]*m[15])*m[ 7])*m[ 9])-m[ 7]*((-m[12]*(m[19]*m[ 8]-m[10]*m[16]))+m[13]*(m[17]*m[ 8]-m[10]*m[15])+(m[15]*m[19]-m[16]*m[17])*m[ 7])+m[ 8]*((-m[11]*(m[19]*m[ 8]-m[10]*m[16]))+m[13]*(m[14]*m[ 8]-m[10]*m[12])+(m[12]*m[19]-m[14]*m[16])*m[ 7])+(m[11]*(m[15]*m[19]-m[16]*m[17])-m[12]*(m[12]*m[19]-m[14]*m[16])+m[13]*(m[12]*m[17]-m[14]*m[15]))*m[ 6])+m[ 1]*((m[11]*(m[15]*m[20]-m[17]*m[17])-m[12]*(m[12]*m[20]-m[14]*m[17])+m[14]*(m[12]*m[17]-m[14]*m[15]))*m[ 9]-(m[11]*(m[16]*m[20]-m[17]*m[19])-m[13]*(m[12]*m[20]-m[14]*m[17])+m[14]*(m[12]*m[19]-m[14]*m[16]))*m[ 8]+(m[12]*(m[16]*m[20]-m[17]*m[19])-m[13]*(m[15]*m[20]-m[17]*m[17])+m[14]*(m[15]*m[19]-m[16]*m[17]))*m[ 7]-m[10]*(m[11]*(m[15]*m[19]-m[16]*m[17])-m[12]*(m[12]*m[19]-m[14]*m[16])+m[13]*(m[12]*m[17]-m[14]*m[15])))-m[ 4]*((-m[ 7]*((-m[12]*(m[20]*m[ 8]-m[10]*m[17]))+m[14]*(m[17]*m[ 8]-m[10]*m[15])+(m[15]*m[20]-m[17]*m[17])*m[ 7]))+m[ 8]*((-m[11]*(m[20]*m[ 8]-m[10]*m[17]))+m[14]*(m[14]*m[ 8]-m[10]*m[12])+(m[12]*m[20]-m[14]*m[17])*m[ 7])-m[10]*((-m[11]*(m[17]*m[ 8]-m[10]*m[15]))+m[12]*(m[14]*m[ 8]-m[10]*m[12])+(m[12]*m[17]-m[14]*m[15])*m[ 7])+(m[11]*(m[15]*m[20]-m[17]*m[17])-m[12]*(m[12]*m[20]-m[14]*m[17])+m[14]*(m[12]*m[17]-m[14]*m[15]))*m[ 6]);
    c[ 5] = m[ 2]*((-m[ 8]*((-m[13]*(m[19]*m[ 8]-m[17]*m[ 9]))+m[14]*(m[18]*m[ 8]-m[16]*m[ 9])+(m[16]*m[19]-m[17]*m[18])*m[ 7]))+m[ 9]*((-m[12]*(m[19]*m[ 8]-m[17]*m[ 9]))+m[14]*(m[16]*m[ 8]-m[15]*m[ 9])+(m[15]*m[19]-m[16]*m[17])*m[ 7])-m[10]*((-m[12]*(m[18]*m[ 8]-m[16]*m[ 9]))+m[13]*(m[16]*m[ 8]-m[15]*m[ 9])+(m[15]*m[18]-m[16]*m[16])*m[ 7])+(m[12]*(m[16]*m[19]-m[17]*m[18])-m[13]*(m[15]*m[19]-m[16]*m[17])+m[14]*(m[15]*m[18]-m[16]*m[16]))*m[ 6])-m[ 3]*((-m[ 7]*((-m[13]*(m[19]*m[ 8]-m[17]*m[ 9]))+m[14]*(m[18]*m[ 8]-m[16]*m[ 9])+(m[16]*m[19]-m[17]*m[18])*m[ 7]))+m[ 9]*((-m[11]*(m[19]*m[ 8]-m[17]*m[ 9]))+m[14]*(m[13]*m[ 8]-m[12]*m[ 9])+(m[12]*m[19]-m[13]*m[17])*m[ 7])-m[10]*((-m[11]*(m[18]*m[ 8]-m[16]*m[ 9]))+m[13]*(m[13]*m[ 8]-m[12]*m[ 9])+(m[12]*m[18]-m[13]*m[16])*m[ 7])+(m[11]*(m[16]*m[19]-m[17]*m[18])-m[13]*(m[12]*m[19]-m[13]*m[17])+m[14]*(m[12]*m[18]-m[13]*m[16]))*m[ 6])+m[ 4]*((-m[ 7]*((-m[12]*(m[19]*m[ 8]-m[17]*m[ 9]))+m[14]*(m[16]*m[ 8]-m[15]*m[ 9])+(m[15]*m[19]-m[16]*m[17])*m[ 7]))+m[ 8]*((-m[11]*(m[19]*m[ 8]-m[17]*m[ 9]))+m[14]*(m[13]*m[ 8]-m[12]*m[ 9])+(m[12]*m[19]-m[13]*m[17])*m[ 7])-m[10]*((-m[11]*(m[16]*m[ 8]-m[15]*m[ 9]))+m[12]*(m[13]*m[ 8]-m[12]*m[ 9])+(m[12]*m[16]-m[13]*m[15])*m[ 7])+(m[11]*(m[15]*m[19]-m[16]*m[17])-m[12]*(m[12]*m[19]-m[13]*m[17])+m[14]*(m[12]*m[16]-m[13]*m[15]))*m[ 6])-m[ 5]*((-m[ 7]*((-m[12]*(m[18]*m[ 8]-m[16]*m[ 9]))+m[13]*(m[16]*m[ 8]-m[15]*m[ 9])+(m[15]*m[18]-m[16]*m[16])*m[ 7]))+m[ 8]*((-m[11]*(m[18]*m[ 8]-m[16]*m[ 9]))+m[13]*(m[13]*m[ 8]-m[12]*m[ 9])+(m[12]*m[18]-m[13]*m[16])*m[ 7])-m[ 9]*((-m[11]*(m[16]*m[ 8]-m[15]*m[ 9]))+m[12]*(m[13]*m[ 8]-m[12]*m[ 9])+(m[12]*m[16]-m[13]*m[15])*m[ 7])+(m[11]*(m[15]*m[18]-m[16]*m[16])-m[12]*(m[12]*m[18]-m[13]*m[16])+m[13]*(m[12]*m[16]-m[13]*m[15]))*m[ 6])-m[ 1]*((m[11]*(m[15]*m[19]-m[16]*m[17])-m[12]*(m[12]*m[19]-m[13]*m[17])+m[14]*(m[12]*m[16]-m[13]*m[15]))*m[ 9]-(m[11]*(m[16]*m[19]-m[17]*m[18])-m[13]*(m[12]*m[19]-m[13]*m[17])+m[14]*(m[12]*m[18]-m[13]*m[16]))*m[ 8]+(m[12]*(m[16]*m[19]-m[17]*m[18])-m[13]*(m[15]*m[19]-m[16]*m[17])+m[14]*(m[15]*m[18]-m[16]*m[16]))*m[ 7]-m[10]*(m[11]*(m[15]*m[18]-m[16]*m[16])-m[12]*(m[12]*m[18]-m[13]*m[16])+m[13]*(m[12]*m[16]-m[13]*m[15])));
    c[ 6] = (-m[ 2]*((-m[12]*((-m[16]*(m[20]*m[ 4]-m[19]*m[ 5]))+m[17]*(m[19]*m[ 4]-m[18]*m[ 5])+(m[18]*m[20]-m[19]*m[19])*m[ 3]))+m[13]*((-m[15]*(m[20]*m[ 4]-m[19]*m[ 5]))+m[17]*(m[17]*m[ 4]-m[16]*m[ 5])+(m[16]*m[20]-m[17]*m[19])*m[ 3])-m[14]*((-m[15]*(m[19]*m[ 4]-m[18]*m[ 5]))+m[16]*(m[17]*m[ 4]-m[16]*m[ 5])+(m[16]*m[19]-m[17]*m[18])*m[ 3])+m[ 2]*(m[15]*(m[18]*m[20]-m[19]*m[19])-m[16]*(m[16]*m[20]-m[17]*m[19])+m[17]*(m[16]*m[19]-m[17]*m[18]))))+m[ 3]*((-m[11]*((-m[16]*(m[20]*m[ 4]-m[19]*m[ 5]))+m[17]*(m[19]*m[ 4]-m[18]*m[ 5])+(m[18]*m[20]-m[19]*m[19])*m[ 3]))+m[13]*((-m[12]*(m[20]*m[ 4]-m[19]*m[ 5]))+m[17]*(m[14]*m[ 4]-m[13]*m[ 5])+(m[13]*m[20]-m[14]*m[19])*m[ 3])-m[14]*((-m[12]*(m[19]*m[ 4]-m[18]*m[ 5]))+m[16]*(m[14]*m[ 4]-m[13]*m[ 5])+(m[13]*m[19]-m[14]*m[18])*m[ 3])+m[ 2]*(m[12]*(m[18]*m[20]-m[19]*m[19])-m[16]*(m[13]*m[20]-m[14]*m[19])+m[17]*(m[13]*m[19]-m[14]*m[18])))-m[ 4]*((-m[11]*((-m[15]*(m[20]*m[ 4]-m[19]*m[ 5]))+m[17]*(m[17]*m[ 4]-m[16]*m[ 5])+(m[16]*m[20]-m[17]*m[19])*m[ 3]))+m[12]*((-m[12]*(m[20]*m[ 4]-m[19]*m[ 5]))+m[17]*(m[14]*m[ 4]-m[13]*m[ 5])+(m[13]*m[20]-m[14]*m[19])*m[ 3])-m[14]*((-m[12]*(m[17]*m[ 4]-m[16]*m[ 5]))+m[15]*(m[14]*m[ 4]-m[13]*m[ 5])+(m[13]*m[17]-m[14]*m[16])*m[ 3])+m[ 2]*(m[12]*(m[16]*m[20]-m[17]*m[19])-m[15]*(m[13]*m[20]-m[14]*m[19])+m[17]*(m[13]*m[17]-m[14]*m[16])))+m[ 5]*((-m[11]*((-m[15]*(m[19]*m[ 4]-m[18]*m[ 5]))+m[16]*(m[17]*m[ 4]-m[16]*m[ 5])+(m[16]*m[19]-m[17]*m[18])*m[ 3]))+m[12]*((-m[12]*(m[19]*m[ 4]-m[18]*m[ 5]))+m[16]*(m[14]*m[ 4]-m[13]*m[ 5])+(m[13]*m[19]-m[14]*m[18])*m[ 3])-m[13]*((-m[12]*(m[17]*m[ 4]-m[16]*m[ 5]))+m[15]*(m[14]*m[ 4]-m[13]*m[ 5])+(m[13]*m[17]-m[14]*m[16])*m[ 3])+(m[12]*(m[16]*m[19]-m[17]*m[18])-m[15]*(m[13]*m[19]-m[14]*m[18])+m[16]*(m[13]*m[17]-m[14]*m[16]))*m[ 2])+m[ 0]*(m[11]*(m[15]*(m[18]*m[20]-m[19]*m[19])-m[16]*(m[16]*m[20]-m[17]*m[19])+m[17]*(m[16]*m[19]-m[17]*m[18]))-m[12]*(m[12]*(m[18]*m[20]-m[19]*m[19])-m[16]*(m[13]*m[20]-m[14]*m[19])+m[17]*(m[13]*m[19]-m[14]*m[18]))+m[13]*(m[12]*(m[16]*m[20]-m[17]*m[19])-m[15]*(m[13]*m[20]-m[14]*m[19])+m[17]*(m[13]*m[17]-m[14]*m[16]))-m[14]*(m[12]*(m[16]*m[19]-m[17]*m[18])-m[15]*(m[13]*m[19]-m[14]*m[18])+m[16]*(m[13]*m[17]-m[14]*m[16])));
    c[ 7] = m[ 2]*(((-m[15]*(m[20]*m[ 4]-m[19]*m[ 5]))+m[17]*(m[17]*m[ 4]-m[16]*m[ 5])+(m[16]*m[20]-m[17]*m[19])*m[ 3])*m[ 9]-((-m[16]*(m[20]*m[ 4]-m[19]*m[ 5]))+m[17]*(m[19]*m[ 4]-m[18]*m[ 5])+(m[18]*m[20]-m[19]*m[19])*m[ 3])*m[ 8]-m[10]*((-m[15]*(m[19]*m[ 4]-m[18]*m[ 5]))+m[16]*(m[17]*m[ 4]-m[16]*m[ 5])+(m[16]*m[19]-m[17]*m[18])*m[ 3])+m[ 1]*(m[15]*(m[18]*m[20]-m[19]*m[19])-m[16]*(m[16]*m[20]-m[17]*m[19])+m[17]*(m[16]*m[19]-m[17]*m[18])))-m[ 3]*(((-m[12]*(m[20]*m[ 4]-m[19]*m[ 5]))+m[17]*(m[14]*m[ 4]-m[13]*m[ 5])+(m[13]*m[20]-m[14]*m[19])*m[ 3])*m[ 9]-((-m[16]*(m[20]*m[ 4]-m[19]*m[ 5]))+m[17]*(m[19]*m[ 4]-m[18]*m[ 5])+(m[18]*m[20]-m[19]*m[19])*m[ 3])*m[ 7]-m[10]*((-m[12]*(m[19]*m[ 4]-m[18]*m[ 5]))+m[16]*(m[14]*m[ 4]-m[13]*m[ 5])+(m[13]*m[19]-m[14]*m[18])*m[ 3])+m[ 1]*(m[12]*(m[18]*m[20]-m[19]*m[19])-m[16]*(m[13]*m[20]-m[14]*m[19])+m[17]*(m[13]*m[19]-m[14]*m[18])))-m[ 5]*((-((-m[12]*(m[17]*m[ 4]-m[16]*m[ 5]))+m[15]*(m[14]*m[ 4]-m[13]*m[ 5])+(m[13]*m[17]-m[14]*m[16])*m[ 3])*m[ 9])+((-m[12]*(m[19]*m[ 4]-m[18]*m[ 5]))+m[16]*(m[14]*m[ 4]-m[13]*m[ 5])+(m[13]*m[19]-m[14]*m[18])*m[ 3])*m[ 8]-((-m[15]*(m[19]*m[ 4]-m[18]*m[ 5]))+m[16]*(m[17]*m[ 4]-m[16]*m[ 5])+(m[16]*m[19]-m[17]*m[18])*m[ 3])*m[ 7]+m[ 1]*(m[12]*(m[16]*m[19]-m[17]*m[18])-m[15]*(m[13]*m[19]-m[14]*m[18])+m[16]*(m[13]*m[17]-m[14]*m[16])))-m[ 0]*((m[12]*(m[16]*m[20]-m[17]*m[19])-m[15]*(m[13]*m[20]-m[14]*m[19])+m[17]*(m[13]*m[17]-m[14]*m[16]))*m[ 9]-(m[12]*(m[18]*m[20]-m[19]*m[19])-m[16]*(m[13]*m[20]-m[14]*m[19])+m[17]*(m[13]*m[19]-m[14]*m[18]))*m[ 8]+(m[15]*(m[18]*m[20]-m[19]*m[19])-m[16]*(m[16]*m[20]-m[17]*m[19])+m[17]*(m[16]*m[19]-m[17]*m[18]))*m[ 7]-m[10]*(m[12]*(m[16]*m[19]-m[17]*m[18])-m[15]*(m[13]*m[19]-m[14]*m[18])+m[16]*(m[13]*m[17]-m[14]*m[16])))+m[ 4]*(((-m[12]*(m[20]*m[ 4]-m[19]*m[ 5]))+m[17]*(m[14]*m[ 4]-m[13]*m[ 5])+(m[13]*m[20]-m[14]*m[19])*m[ 3])*m[ 8]-((-m[15]*(m[20]*m[ 4]-m[19]*m[ 5]))+m[17]*(m[17]*m[ 4]-m[16]*m[ 5])+(m[16]*m[20]-m[17]*m[19])*m[ 3])*m[ 7]-m[10]*((-m[12]*(m[17]*m[ 4]-m[16]*m[ 5]))+m[15]*(m[14]*m[ 4]-m[13]*m[ 5])+(m[13]*m[17]-m[14]*m[16])*m[ 3])+m[ 1]*(m[12]*(m[16]*m[20]-m[17]*m[19])-m[15]*(m[13]*m[20]-m[14]*m[19])+m[17]*(m[13]*m[17]-m[14]*m[16])));
    c[ 8] = (-m[ 2]*(((-m[12]*(m[20]*m[ 4]-m[19]*m[ 5]))+m[14]*(m[17]*m[ 4]-m[16]*m[ 5])+m[ 2]*(m[16]*m[20]-m[17]*m[19]))*m[ 9]-((-m[13]*(m[20]*m[ 4]-m[19]*m[ 5]))+m[14]*(m[19]*m[ 4]-m[18]*m[ 5])+m[ 2]*(m[18]*m[20]-m[19]*m[19]))*m[ 8]-m[10]*((-m[12]*(m[19]*m[ 4]-m[18]*m[ 5]))+m[13]*(m[17]*m[ 4]-m[16]*m[ 5])+(m[16]*m[19]-m[17]*m[18])*m[ 2])+m[ 1]*(m[12]*(m[18]*m[20]-m[19]*m[19])-m[13]*(m[16]*m[20]-m[17]*m[19])+m[14]*(m[16]*m[19]-m[17]*m[18]))))+m[ 3]*(((-m[11]*(m[20]*m[ 4]-m[19]*m[ 5]))+m[14]*(m[14]*m[ 4]-m[13]*m[ 5])+m[ 2]*(m[13]*m[20]-m[14]*m[19]))*m[ 9]-((-m[13]*(m[20]*m[ 4]-m[19]*m[ 5]))+m[14]*(m[19]*m[ 4]-m[18]*m[ 5])+m[ 2]*(m[18]*m[20]-m[19]*m[19]))*m[ 7]-m[10]*((-m[11]*(m[19]*m[ 4]-m[18]*m[ 5]))+m[13]*(m[14]*m[ 4]-m[13]*m[ 5])+(m[13]*m[19]-m[14]*m[18])*m[ 2])+m[ 1]*(m[11]*(m[18]*m[20]-m[19]*m[19])-m[13]*(m[13]*m[20]-m[14]*m[19])+m[14]*(m[13]*m[19]-m[14]*m[18])))+m[ 5]*((-((-m[11]*(m[17]*m[ 4]-m[16]*m[ 5]))+m[12]*(m[14]*m[ 4]-m[13]*m[ 5])+(m[13]*m[17]-m[14]*m[16])*m[ 2])*m[ 9])+((-m[11]*(m[19]*m[ 4]-m[18]*m[ 5]))+m[13]*(m[14]*m[ 4]-m[13]*m[ 5])+(m[13]*m[19]-m[14]*m[18])*m[ 2])*m[ 8]-((-m[12]*(m[19]*m[ 4]-m[18]*m[ 5]))+m[13]*(m[17]*m[ 4]-m[16]*m[ 5])+(m[16]*m[19]-m[17]*m[18])*m[ 2])*m[ 7]+m[ 1]*(m[11]*(m[16]*m[19]-m[17]*m[18])-m[12]*(m[13]*m[19]-m[14]*m[18])+m[13]*(m[13]*m[17]-m[14]*m[16])))+m[ 0]*((m[11]*(m[16]*m[20]-m[17]*m[19])-m[12]*(m[13]*m[20]-m[14]*m[19])+m[14]*(m[13]*m[17]-m[14]*m[16]))*m[ 9]-(m[11]*(m[18]*m[20]-m[19]*m[19])-m[13]*(m[13]*m[20]-m[14]*m[19])+m[14]*(m[13]*m[19]-m[14]*m[18]))*m[ 8]+(m[12]*(m[18]*m[20]-m[19]*m[19])-m[13]*(m[16]*m[20]-m[17]*m[19])+m[14]*(m[16]*m[19]-m[17]*m[18]))*m[ 7]-m[10]*(m[11]*(m[16]*m[19]-m[17]*m[18])-m[12]*(m[13]*m[19]-m[14]*m[18])+m[13]*(m[13]*m[17]-m[14]*m[16])))-m[ 4]*(((-m[11]*(m[20]*m[ 4]-m[19]*m[ 5]))+m[14]*(m[14]*m[ 4]-m[13]*m[ 5])+m[ 2]*(m[13]*m[20]-m[14]*m[19]))*m[ 8]-((-m[12]*(m[20]*m[ 4]-m[19]*m[ 5]))+m[14]*(m[17]*m[ 4]-m[16]*m[ 5])+m[ 2]*(m[16]*m[20]-m[17]*m[19]))*m[ 7]-m[10]*((-m[11]*(m[17]*m[ 4]-m[16]*m[ 5]))+m[12]*(m[14]*m[ 4]-m[13]*m[ 5])+(m[13]*m[17]-m[14]*m[16])*m[ 2])+m[ 1]*(m[11]*(m[16]*m[20]-m[17]*m[19])-m[12]*(m[13]*m[20]-m[14]*m[19])+m[14]*(m[13]*m[17]-m[14]*m[16])));
    c[ 9] = m[ 2]*(((-m[12]*(m[20]*m[ 3]-m[17]*m[ 5]))+m[14]*(m[17]*m[ 3]-m[15]*m[ 5])+m[ 2]*(m[15]*m[20]-m[17]*m[17]))*m[ 9]-((-m[13]*(m[20]*m[ 3]-m[17]*m[ 5]))+m[14]*(m[19]*m[ 3]-m[16]*m[ 5])+m[ 2]*(m[16]*m[20]-m[17]*m[19]))*m[ 8]-m[10]*((-m[12]*(m[19]*m[ 3]-m[16]*m[ 5]))+m[13]*(m[17]*m[ 3]-m[15]*m[ 5])+(m[15]*m[19]-m[16]*m[17])*m[ 2])+m[ 1]*(m[12]*(m[16]*m[20]-m[17]*m[19])-m[13]*(m[15]*m[20]-m[17]*m[17])+m[14]*(m[15]*m[19]-m[16]*m[17])))-m[ 3]*(((-m[11]*(m[20]*m[ 3]-m[17]*m[ 5]))+m[14]*(m[14]*m[ 3]-m[12]*m[ 5])+m[ 2]*(m[12]*m[20]-m[14]*m[17]))*m[ 9]-((-m[13]*(m[20]*m[ 3]-m[17]*m[ 5]))+m[14]*(m[19]*m[ 3]-m[16]*m[ 5])+m[ 2]*(m[16]*m[20]-m[17]*m[19]))*m[ 7]-m[10]*((-m[11]*(m[19]*m[ 3]-m[16]*m[ 5]))+m[13]*(m[14]*m[ 3]-m[12]*m[ 5])+(m[12]*m[19]-m[14]*m[16])*m[ 2])+m[ 1]*(m[11]*(m[16]*m[20]-m[17]*m[19])-m[13]*(m[12]*m[20]-m[14]*m[17])+m[14]*(m[12]*m[19]-m[14]*m[16])))-m[ 5]*((-((-m[11]*(m[17]*m[ 3]-m[15]*m[ 5]))+m[12]*(m[14]*m[ 3]-m[12]*m[ 5])+(m[12]*m[17]-m[14]*m[15])*m[ 2])*m[ 9])+((-m[11]*(m[19]*m[ 3]-m[16]*m[ 5]))+m[13]*(m[14]*m[ 3]-m[12]*m[ 5])+(m[12]*m[19]-m[14]*m[16])*m[ 2])*m[ 8]-((-m[12]*(m[19]*m[ 3]-m[16]*m[ 5]))+m[13]*(m[17]*m[ 3]-m[15]*m[ 5])+(m[15]*m[19]-m[16]*m[17])*m[ 2])*m[ 7]+m[ 1]*(m[11]*(m[15]*m[19]-m[16]*m[17])-m[12]*(m[12]*m[19]-m[14]*m[16])+m[13]*(m[12]*m[17]-m[14]*m[15])))-m[ 0]*((m[11]*(m[15]*m[20]-m[17]*m[17])-m[12]*(m[12]*m[20]-m[14]*m[17])+m[14]*(m[12]*m[17]-m[14]*m[15]))*m[ 9]-(m[11]*(m[16]*m[20]-m[17]*m[19])-m[13]*(m[12]*m[20]-m[14]*m[17])+m[14]*(m[12]*m[19]-m[14]*m[16]))*m[ 8]+(m[12]*(m[16]*m[20]-m[17]*m[19])-m[13]*(m[15]*m[20]-m[17]*m[17])+m[14]*(m[15]*m[19]-m[16]*m[17]))*m[ 7]-m[10]*(m[11]*(m[15]*m[19]-m[16]*m[17])-m[12]*(m[12]*m[19]-m[14]*m[16])+m[13]*(m[12]*m[17]-m[14]*m[15])))+m[ 4]*(((-m[11]*(m[20]*m[ 3]-m[17]*m[ 5]))+m[14]*(m[14]*m[ 3]-m[12]*m[ 5])+m[ 2]*(m[12]*m[20]-m[14]*m[17]))*m[ 8]-((-m[12]*(m[20]*m[ 3]-m[17]*m[ 5]))+m[14]*(m[17]*m[ 3]-m[15]*m[ 5])+m[ 2]*(m[15]*m[20]-m[17]*m[17]))*m[ 7]-m[10]*((-m[11]*(m[17]*m[ 3]-m[15]*m[ 5]))+m[12]*(m[14]*m[ 3]-m[12]*m[ 5])+(m[12]*m[17]-m[14]*m[15])*m[ 2])+m[ 1]*(m[11]*(m[15]*m[20]-m[17]*m[17])-m[12]*(m[12]*m[20]-m[14]*m[17])+m[14]*(m[12]*m[17]-m[14]*m[15])));
    c[10] = (-m[ 2]*(((-m[12]*(m[19]*m[ 3]-m[17]*m[ 4]))+m[14]*(m[16]*m[ 3]-m[15]*m[ 4])+(m[15]*m[19]-m[16]*m[17])*m[ 2])*m[ 9]-((-m[13]*(m[19]*m[ 3]-m[17]*m[ 4]))+m[14]*(m[18]*m[ 3]-m[16]*m[ 4])+(m[16]*m[19]-m[17]*m[18])*m[ 2])*m[ 8]-m[10]*((-m[12]*(m[18]*m[ 3]-m[16]*m[ 4]))+m[13]*(m[16]*m[ 3]-m[15]*m[ 4])+(m[15]*m[18]-m[16]*m[16])*m[ 2])+m[ 1]*(m[12]*(m[16]*m[19]-m[17]*m[18])-m[13]*(m[15]*m[19]-m[16]*m[17])+m[14]*(m[15]*m[18]-m[16]*m[16]))))+m[ 3]*(((-m[11]*(m[19]*m[ 3]-m[17]*m[ 4]))+m[14]*(m[13]*m[ 3]-m[12]*m[ 4])+(m[12]*m[19]-m[13]*m[17])*m[ 2])*m[ 9]-((-m[13]*(m[19]*m[ 3]-m[17]*m[ 4]))+m[14]*(m[18]*m[ 3]-m[16]*m[ 4])+(m[16]*m[19]-m[17]*m[18])*m[ 2])*m[ 7]-m[10]*((-m[11]*(m[18]*m[ 3]-m[16]*m[ 4]))+m[13]*(m[13]*m[ 3]-m[12]*m[ 4])+(m[12]*m[18]-m[13]*m[16])*m[ 2])+m[ 1]*(m[11]*(m[16]*m[19]-m[17]*m[18])-m[13]*(m[12]*m[19]-m[13]*m[17])+m[14]*(m[12]*m[18]-m[13]*m[16])))+m[ 5]*((-((-m[11]*(m[16]*m[ 3]-m[15]*m[ 4]))+m[12]*(m[13]*m[ 3]-m[12]*m[ 4])+(m[12]*m[16]-m[13]*m[15])*m[ 2])*m[ 9])+((-m[11]*(m[18]*m[ 3]-m[16]*m[ 4]))+m[13]*(m[13]*m[ 3]-m[12]*m[ 4])+(m[12]*m[18]-m[13]*m[16])*m[ 2])*m[ 8]-((-m[12]*(m[18]*m[ 3]-m[16]*m[ 4]))+m[13]*(m[16]*m[ 3]-m[15]*m[ 4])+(m[15]*m[18]-m[16]*m[16])*m[ 2])*m[ 7]+m[ 1]*(m[11]*(m[15]*m[18]-m[16]*m[16])-m[12]*(m[12]*m[18]-m[13]*m[16])+m[13]*(m[12]*m[16]-m[13]*m[15])))+m[ 0]*((m[11]*(m[15]*m[19]-m[16]*m[17])-m[12]*(m[12]*m[19]-m[13]*m[17])+m[14]*(m[12]*m[16]-m[13]*m[15]))*m[ 9]-(m[11]*(m[16]*m[19]-m[17]*m[18])-m[13]*(m[12]*m[19]-m[13]*m[17])+m[14]*(m[12]*m[18]-m[13]*m[16]))*m[ 8]+(m[12]*(m[16]*m[19]-m[17]*m[18])-m[13]*(m[15]*m[19]-m[16]*m[17])+m[14]*(m[15]*m[18]-m[16]*m[16]))*m[ 7]-m[10]*(m[11]*(m[15]*m[18]-m[16]*m[16])-m[12]*(m[12]*m[18]-m[13]*m[16])+m[13]*(m[12]*m[16]-m[13]*m[15])))-m[ 4]*(((-m[11]*(m[19]*m[ 3]-m[17]*m[ 4]))+m[14]*(m[13]*m[ 3]-m[12]*m[ 4])+(m[12]*m[19]-m[13]*m[17])*m[ 2])*m[ 8]-((-m[12]*(m[19]*m[ 3]-m[17]*m[ 4]))+m[14]*(m[16]*m[ 3]-m[15]*m[ 4])+(m[15]*m[19]-m[16]*m[17])*m[ 2])*m[ 7]-m[10]*((-m[11]*(m[16]*m[ 3]-m[15]*m[ 4]))+m[12]*(m[13]*m[ 3]-m[12]*m[ 4])+(m[12]*m[16]-m[13]*m[15])*m[ 2])+m[ 1]*(m[11]*(m[15]*m[19]-m[16]*m[17])-m[12]*(m[12]*m[19]-m[13]*m[17])+m[14]*(m[12]*m[16]-m[13]*m[15])));
    c[11] = m[ 3]*(m[ 9]*(m[17]*(m[10]*m[ 4]-m[ 5]*m[ 9])+m[ 3]*(m[20]*m[ 9]-m[10]*m[19])-(m[20]*m[ 4]-m[19]*m[ 5])*m[ 8])-m[10]*(m[16]*(m[10]*m[ 4]-m[ 5]*m[ 9])+m[ 3]*(m[19]*m[ 9]-m[10]*m[18])-(m[19]*m[ 4]-m[18]*m[ 5])*m[ 8])+m[ 1]*((-m[16]*(m[20]*m[ 9]-m[10]*m[19]))+m[17]*(m[19]*m[ 9]-m[10]*m[18])+(m[18]*m[20]-m[19]*m[19])*m[ 8])-((-m[16]*(m[20]*m[ 4]-m[19]*m[ 5]))+m[17]*(m[19]*m[ 4]-m[18]*m[ 5])+(m[18]*m[20]-m[19]*m[19])*m[ 3])*m[ 6])-m[ 4]*(m[ 8]*(m[17]*(m[10]*m[ 4]-m[ 5]*m[ 9])+m[ 3]*(m[20]*m[ 9]-m[10]*m[19])-(m[20]*m[ 4]-m[19]*m[ 5])*m[ 8])-m[10]*(m[15]*(m[10]*m[ 4]-m[ 5]*m[ 9])+m[ 3]*(m[17]*m[ 9]-m[10]*m[16])-(m[17]*m[ 4]-m[16]*m[ 5])*m[ 8])+m[ 1]*((-m[15]*(m[20]*m[ 9]-m[10]*m[19]))+m[17]*(m[17]*m[ 9]-m[10]*m[16])+(m[16]*m[20]-m[17]*m[19])*m[ 8])-((-m[15]*(m[20]*m[ 4]-m[19]*m[ 5]))+m[17]*(m[17]*m[ 4]-m[16]*m[ 5])+(m[16]*m[20]-m[17]*m[19])*m[ 3])*m[ 6])+m[ 5]*(m[ 8]*(m[16]*(m[10]*m[ 4]-m[ 5]*m[ 9])+m[ 3]*(m[19]*m[ 9]-m[10]*m[18])-(m[19]*m[ 4]-m[18]*m[ 5])*m[ 8])-m[ 9]*(m[15]*(m[10]*m[ 4]-m[ 5]*m[ 9])+m[ 3]*(m[17]*m[ 9]-m[10]*m[16])-(m[17]*m[ 4]-m[16]*m[ 5])*m[ 8])+m[ 1]*((-m[15]*(m[19]*m[ 9]-m[10]*m[18]))+m[16]*(m[17]*m[ 9]-m[10]*m[16])+(m[16]*m[19]-m[17]*m[18])*m[ 8])-((-m[15]*(m[19]*m[ 4]-m[18]*m[ 5]))+m[16]*(m[17]*m[ 4]-m[16]*m[ 5])+(m[16]*m[19]-m[17]*m[18])*m[ 3])*m[ 6])+m[ 0]*((-m[ 8]*((-m[16]*(m[20]*m[ 9]-m[10]*m[19]))+m[17]*(m[19]*m[ 9]-m[10]*m[18])+(m[18]*m[20]-m[19]*m[19])*m[ 8]))+m[ 9]*((-m[15]*(m[20]*m[ 9]-m[10]*m[19]))+m[17]*(m[17]*m[ 9]-m[10]*m[16])+(m[16]*m[20]-m[17]*m[19])*m[ 8])-m[10]*((-m[15]*(m[19]*m[ 9]-m[10]*m[18]))+m[16]*(m[17]*m[ 9]-m[10]*m[16])+(m[16]*m[19]-m[17]*m[18])*m[ 8])+(m[15]*(m[18]*m[20]-m[19]*m[19])-m[16]*(m[16]*m[20]-m[17]*m[19])+m[17]*(m[16]*m[19]-m[17]*m[18]))*m[ 6])-m[ 1]*(((-m[15]*(m[20]*m[ 4]-m[19]*m[ 5]))+m[17]*(m[17]*m[ 4]-m[16]*m[ 5])+(m[16]*m[20]-m[17]*m[19])*m[ 3])*m[ 9]-((-m[16]*(m[20]*m[ 4]-m[19]*m[ 5]))+m[17]*(m[19]*m[ 4]-m[18]*m[ 5])+(m[18]*m[20]-m[19]*m[19])*m[ 3])*m[ 8]-m[10]*((-m[15]*(m[19]*m[ 4]-m[18]*m[ 5]))+m[16]*(m[17]*m[ 4]-m[16]*m[ 5])+(m[16]*m[19]-m[17]*m[18])*m[ 3])+m[ 1]*(m[15]*(m[18]*m[20]-m[19]*m[19])-m[16]*(m[16]*m[20]-m[17]*m[19])+m[17]*(m[16]*m[19]-m[17]*m[18])));
    c[12] = (-m[ 3]*(m[ 9]*(m[14]*(m[10]*m[ 4]-m[ 5]*m[ 9])+m[ 2]*(m[20]*m[ 9]-m[10]*m[19])-(m[20]*m[ 4]-m[19]*m[ 5])*m[ 7])-m[10]*(m[13]*(m[10]*m[ 4]-m[ 5]*m[ 9])+m[ 2]*(m[19]*m[ 9]-m[10]*m[18])-(m[19]*m[ 4]-m[18]*m[ 5])*m[ 7])+m[ 1]*((-m[13]*(m[20]*m[ 9]-m[10]*m[19]))+m[14]*(m[19]*m[ 9]-m[10]*m[18])+(m[18]*m[20]-m[19]*m[19])*m[ 7])-((-m[13]*(m[20]*m[ 4]-m[19]*m[ 5]))+m[14]*(m[19]*m[ 4]-m[18]*m[ 5])+m[ 2]*(m[18]*m[20]-m[19]*m[19]))*m[ 6]))+m[ 4]*(m[ 8]*(m[14]*(m[10]*m[ 4]-m[ 5]*m[ 9])+m[ 2]*(m[20]*m[ 9]-m[10]*m[19])-(m[20]*m[ 4]-m[19]*m[ 5])*m[ 7])-m[10]*(m[12]*(m[10]*m[ 4]-m[ 5]*m[ 9])+m[ 2]*(m[17]*m[ 9]-m[10]*m[16])-(m[17]*m[ 4]-m[16]*m[ 5])*m[ 7])+m[ 1]*((-m[12]*(m[20]*m[ 9]-m[10]*m[19]))+m[14]*(m[17]*m[ 9]-m[10]*m[16])+(m[16]*m[20]-m[17]*m[19])*m[ 7])-((-m[12]*(m[20]*m[ 4]-m[19]*m[ 5]))+m[14]*(m[17]*m[ 4]-m[16]*m[ 5])+m[ 2]*(m[16]*m[20]-m[17]*m[19]))*m[ 6])-m[ 5]*(m[ 8]*(m[13]*(m[10]*m[ 4]-m[ 5]*m[ 9])+m[ 2]*(m[19]*m[ 9]-m[10]*m[18])-(m[19]*m[ 4]-m[18]*m[ 5])*m[ 7])-m[ 9]*(m[12]*(m[10]*m[ 4]-m[ 5]*m[ 9])+m[ 2]*(m[17]*m[ 9]-m[10]*m[16])-(m[17]*m[ 4]-m[16]*m[ 5])*m[ 7])+m[ 1]*((-m[12]*(m[19]*m[ 9]-m[10]*m[18]))+m[13]*(m[17]*m[ 9]-m[10]*m[16])+(m[16]*m[19]-m[17]*m[18])*m[ 7])-((-m[12]*(m[19]*m[ 4]-m[18]*m[ 5]))+m[13]*(m[17]*m[ 4]-m[16]*m[ 5])+(m[16]*m[19]-m[17]*m[18])*m[ 2])*m[ 6])-m[ 0]*((-m[ 8]*((-m[13]*(m[20]*m[ 9]-m[10]*m[19]))+m[14]*(m[19]*m[ 9]-m[10]*m[18])+(m[18]*m[20]-m[19]*m[19])*m[ 7]))+m[ 9]*((-m[12]*(m[20]*m[ 9]-m[10]*m[19]))+m[14]*(m[17]*m[ 9]-m[10]*m[16])+(m[16]*m[20]-m[17]*m[19])*m[ 7])-m[10]*((-m[12]*(m[19]*m[ 9]-m[10]*m[18]))+m[13]*(m[17]*m[ 9]-m[10]*m[16])+(m[16]*m[19]-m[17]*m[18])*m[ 7])+(m[12]*(m[18]*m[20]-m[19]*m[19])-m[13]*(m[16]*m[20]-m[17]*m[19])+m[14]*(m[16]*m[19]-m[17]*m[18]))*m[ 6])+m[ 1]*(((-m[12]*(m[20]*m[ 4]-m[19]*m[ 5]))+m[14]*(m[17]*m[ 4]-m[16]*m[ 5])+m[ 2]*(m[16]*m[20]-m[17]*m[19]))*m[ 9]-((-m[13]*(m[20]*m[ 4]-m[19]*m[ 5]))+m[14]*(m[19]*m[ 4]-m[18]*m[ 5])+m[ 2]*(m[18]*m[20]-m[19]*m[19]))*m[ 8]-m[10]*((-m[12]*(m[19]*m[ 4]-m[18]*m[ 5]))+m[13]*(m[17]*m[ 4]-m[16]*m[ 5])+(m[16]*m[19]-m[17]*m[18])*m[ 2])+m[ 1]*(m[12]*(m[18]*m[20]-m[19]*m[19])-m[13]*(m[16]*m[20]-m[17]*m[19])+m[14]*(m[16]*m[19]-m[17]*m[18])));
    c[13] = m[ 3]*((m[14]*(m[10]*m[ 3]-m[ 5]*m[ 8])+m[ 2]*(m[20]*m[ 8]-m[10]*m[17])-(m[20]*m[ 3]-m[17]*m[ 5])*m[ 7])*m[ 9]-m[10]*(m[13]*(m[10]*m[ 3]-m[ 5]*m[ 8])+m[ 2]*(m[19]*m[ 8]-m[10]*m[16])-(m[19]*m[ 3]-m[16]*m[ 5])*m[ 7])+m[ 1]*((-m[13]*(m[20]*m[ 8]-m[10]*m[17]))+m[14]*(m[19]*m[ 8]-m[10]*m[16])+(m[16]*m[20]-m[17]*m[19])*m[ 7])-((-m[13]*(m[20]*m[ 3]-m[17]*m[ 5]))+m[14]*(m[19]*m[ 3]-m[16]*m[ 5])+m[ 2]*(m[16]*m[20]-m[17]*m[19]))*m[ 6])+m[ 5]*((-(m[12]*(m[10]*m[ 3]-m[ 5]*m[ 8])+m[ 2]*(m[17]*m[ 8]-m[10]*m[15])-(m[17]*m[ 3]-m[15]*m[ 5])*m[ 7])*m[ 9])+m[ 8]*(m[13]*(m[10]*m[ 3]-m[ 5]*m[ 8])+m[ 2]*(m[19]*m[ 8]-m[10]*m[16])-(m[19]*m[ 3]-m[16]*m[ 5])*m[ 7])+m[ 1]*((-m[12]*(m[19]*m[ 8]-m[10]*m[16]))+m[13]*(m[17]*m[ 8]-m[10]*m[15])+(m[15]*m[19]-m[16]*m[17])*m[ 7])-((-m[12]*(m[19]*m[ 3]-m[16]*m[ 5]))+m[13]*(m[17]*m[ 3]-m[15]*m[ 5])+(m[15]*m[19]-m[16]*m[17])*m[ 2])*m[ 6])+m[ 0]*(((-m[12]*(m[20]*m[ 8]-m[10]*m[17]))+m[14]*(m[17]*m[ 8]-m[10]*m[15])+(m[15]*m[20]-m[17]*m[17])*m[ 7])*m[ 9]-m[ 8]*((-m[13]*(m[20]*m[ 8]-m[10]*m[17]))+m[14]*(m[19]*m[ 8]-m[10]*m[16])+(m[16]*m[20]-m[17]*m[19])*m[ 7])-m[10]*((-m[12]*(m[19]*m[ 8]-m[10]*m[16]))+m[13]*(m[17]*m[ 8]-m[10]*m[15])+(m[15]*m[19]-m[16]*m[17])*m[ 7])+(m[12]*(m[16]*m[20]-m[17]*m[19])-m[13]*(m[15]*m[20]-m[17]*m[17])+m[14]*(m[15]*m[19]-m[16]*m[17]))*m[ 6])-m[ 1]*(((-m[12]*(m[20]*m[ 3]-m[17]*m[ 5]))+m[14]*(m[17]*m[ 3]-m[15]*m[ 5])+m[ 2]*(m[15]*m[20]-m[17]*m[17]))*m[ 9]-((-m[13]*(m[20]*m[ 3]-m[17]*m[ 5]))+m[14]*(m[19]*m[ 3]-m[16]*m[ 5])+m[ 2]*(m[16]*m[20]-m[17]*m[19]))*m[ 8]-m[10]*((-m[12]*(m[19]*m[ 3]-m[16]*m[ 5]))+m[13]*(m[17]*m[ 3]-m[15]*m[ 5])+(m[15]*m[19]-m[16]*m[17])*m[ 2])+m[ 1]*(m[12]*(m[16]*m[20]-m[17]*m[19])-m[13]*(m[15]*m[20]-m[17]*m[17])+m[14]*(m[15]*m[19]-m[16]*m[17])))-m[ 4]*(m[ 8]*(m[14]*(m[10]*m[ 3]-m[ 5]*m[ 8])+m[ 2]*(m[20]*m[ 8]-m[10]*m[17])-(m[20]*m[ 3]-m[17]*m[ 5])*m[ 7])-m[10]*(m[12]*(m[10]*m[ 3]-m[ 5]*m[ 8])+m[ 2]*(m[17]*m[ 8]-m[10]*m[15])-(m[17]*m[ 3]-m[15]*m[ 5])*m[ 7])+m[ 1]*((-m[12]*(m[20]*m[ 8]-m[10]*m[17]))+m[14]*(m[17]*m[ 8]-m[10]*m[15])+(m[15]*m[20]-m[17]*m[17])*m[ 7])-((-m[12]*(m[20]*m[ 3]-m[17]*m[ 5]))+m[14]*(m[17]*m[ 3]-m[15]*m[ 5])+m[ 2]*(m[15]*m[20]-m[17]*m[17]))*m[ 6]);
    c[14] = (-m[ 3]*(m[ 9]*(m[14]*(m[ 3]*m[ 9]-m[ 4]*m[ 8])+m[ 2]*(m[19]*m[ 8]-m[17]*m[ 9])-(m[19]*m[ 3]-m[17]*m[ 4])*m[ 7])-m[10]*(m[13]*(m[ 3]*m[ 9]-m[ 4]*m[ 8])+m[ 2]*(m[18]*m[ 8]-m[16]*m[ 9])-(m[18]*m[ 3]-m[16]*m[ 4])*m[ 7])+m[ 1]*((-m[13]*(m[19]*m[ 8]-m[17]*m[ 9]))+m[14]*(m[18]*m[ 8]-m[16]*m[ 9])+(m[16]*m[19]-m[17]*m[18])*m[ 7])-((-m[13]*(m[19]*m[ 3]-m[17]*m[ 4]))+m[14]*(m[18]*m[ 3]-m[16]*m[ 4])+(m[16]*m[19]-m[17]*m[18])*m[ 2])*m[ 6]))+m[ 4]*(m[ 8]*(m[14]*(m[ 3]*m[ 9]-m[ 4]*m[ 8])+m[ 2]*(m[19]*m[ 8]-m[17]*m[ 9])-(m[19]*m[ 3]-m[17]*m[ 4])*m[ 7])-m[10]*(m[12]*(m[ 3]*m[ 9]-m[ 4]*m[ 8])+m[ 2]*(m[16]*m[ 8]-m[15]*m[ 9])-(m[16]*m[ 3]-m[15]*m[ 4])*m[ 7])+m[ 1]*((-m[12]*(m[19]*m[ 8]-m[17]*m[ 9]))+m[14]*(m[16]*m[ 8]-m[15]*m[ 9])+(m[15]*m[19]-m[16]*m[17])*m[ 7])-((-m[12]*(m[19]*m[ 3]-m[17]*m[ 4]))+m[14]*(m[16]*m[ 3]-m[15]*m[ 4])+(m[15]*m[19]-m[16]*m[17])*m[ 2])*m[ 6])-m[ 5]*(m[ 8]*(m[13]*(m[ 3]*m[ 9]-m[ 4]*m[ 8])+m[ 2]*(m[18]*m[ 8]-m[16]*m[ 9])-(m[18]*m[ 3]-m[16]*m[ 4])*m[ 7])-m[ 9]*(m[12]*(m[ 3]*m[ 9]-m[ 4]*m[ 8])+m[ 2]*(m[16]*m[ 8]-m[15]*m[ 9])-(m[16]*m[ 3]-m[15]*m[ 4])*m[ 7])+m[ 1]*((-m[12]*(m[18]*m[ 8]-m[16]*m[ 9]))+m[13]*(m[16]*m[ 8]-m[15]*m[ 9])+(m[15]*m[18]-m[16]*m[16])*m[ 7])-((-m[12]*(m[18]*m[ 3]-m[16]*m[ 4]))+m[13]*(m[16]*m[ 3]-m[15]*m[ 4])+(m[15]*m[18]-m[16]*m[16])*m[ 2])*m[ 6])-m[ 0]*((-m[ 8]*((-m[13]*(m[19]*m[ 8]-m[17]*m[ 9]))+m[14]*(m[18]*m[ 8]-m[16]*m[ 9])+(m[16]*m[19]-m[17]*m[18])*m[ 7]))+m[ 9]*((-m[12]*(m[19]*m[ 8]-m[17]*m[ 9]))+m[14]*(m[16]*m[ 8]-m[15]*m[ 9])+(m[15]*m[19]-m[16]*m[17])*m[ 7])-m[10]*((-m[12]*(m[18]*m[ 8]-m[16]*m[ 9]))+m[13]*(m[16]*m[ 8]-m[15]*m[ 9])+(m[15]*m[18]-m[16]*m[16])*m[ 7])+(m[12]*(m[16]*m[19]-m[17]*m[18])-m[13]*(m[15]*m[19]-m[16]*m[17])+m[14]*(m[15]*m[18]-m[16]*m[16]))*m[ 6])+m[ 1]*(((-m[12]*(m[19]*m[ 3]-m[17]*m[ 4]))+m[14]*(m[16]*m[ 3]-m[15]*m[ 4])+(m[15]*m[19]-m[16]*m[17])*m[ 2])*m[ 9]-((-m[13]*(m[19]*m[ 3]-m[17]*m[ 4]))+m[14]*(m[18]*m[ 3]-m[16]*m[ 4])+(m[16]*m[19]-m[17]*m[18])*m[ 2])*m[ 8]-m[10]*((-m[12]*(m[18]*m[ 3]-m[16]*m[ 4]))+m[13]*(m[16]*m[ 3]-m[15]*m[ 4])+(m[15]*m[18]-m[16]*m[16])*m[ 2])+m[ 1]*(m[12]*(m[16]*m[19]-m[17]*m[18])-m[13]*(m[15]*m[19]-m[16]*m[17])+m[14]*(m[15]*m[18]-m[16]*m[16])));
    c[15] = m[ 2]*(m[ 9]*(m[14]*(m[10]*m[ 4]-m[ 5]*m[ 9])+m[ 2]*(m[20]*m[ 9]-m[10]*m[19])-(m[20]*m[ 4]-m[19]*m[ 5])*m[ 7])-m[10]*(m[13]*(m[10]*m[ 4]-m[ 5]*m[ 9])+m[ 2]*(m[19]*m[ 9]-m[10]*m[18])-(m[19]*m[ 4]-m[18]*m[ 5])*m[ 7])+m[ 1]*((-m[13]*(m[20]*m[ 9]-m[10]*m[19]))+m[14]*(m[19]*m[ 9]-m[10]*m[18])+(m[18]*m[20]-m[19]*m[19])*m[ 7])-((-m[13]*(m[20]*m[ 4]-m[19]*m[ 5]))+m[14]*(m[19]*m[ 4]-m[18]*m[ 5])+m[ 2]*(m[18]*m[20]-m[19]*m[19]))*m[ 6])-m[ 4]*(m[ 7]*(m[14]*(m[10]*m[ 4]-m[ 5]*m[ 9])+m[ 2]*(m[20]*m[ 9]-m[10]*m[19])-(m[20]*m[ 4]-m[19]*m[ 5])*m[ 7])-m[10]*(m[11]*(m[10]*m[ 4]-m[ 5]*m[ 9])+m[ 2]*(m[14]*m[ 9]-m[10]*m[13])-(m[14]*m[ 4]-m[13]*m[ 5])*m[ 7])+m[ 1]*((-m[11]*(m[20]*m[ 9]-m[10]*m[19]))+m[14]*(m[14]*m[ 9]-m[10]*m[13])+(m[13]*m[20]-m[14]*m[19])*m[ 7])-((-m[11]*(m[20]*m[ 4]-m[19]*m[ 5]))+m[14]*(m[14]*m[ 4]-m[13]*m[ 5])+m[ 2]*(m[13]*m[20]-m[14]*m[19]))*m[ 6])+m[ 5]*(m[ 7]*(m[13]*(m[10]*m[ 4]-m[ 5]*m[ 9])+m[ 2]*(m[19]*m[ 9]-m[10]*m[18])-(m[19]*m[ 4]-m[18]*m[ 5])*m[ 7])-m[ 9]*(m[11]*(m[10]*m[ 4]-m[ 5]*m[ 9])+m[ 2]*(m[14]*m[ 9]-m[10]*m[13])-(m[14]*m[ 4]-m[13]*m[ 5])*m[ 7])+m[ 1]*((-m[11]*(m[19]*m[ 9]-m[10]*m[18]))+m[13]*(m[14]*m[ 9]-m[10]*m[13])+(m[13]*m[19]-m[14]*m[18])*m[ 7])-((-m[11]*(m[19]*m[ 4]-m[18]*m[ 5]))+m[13]*(m[14]*m[ 4]-m[13]*m[ 5])+(m[13]*m[19]-m[14]*m[18])*m[ 2])*m[ 6])+m[ 0]*((-m[ 7]*((-m[13]*(m[20]*m[ 9]-m[10]*m[19]))+m[14]*(m[19]*m[ 9]-m[10]*m[18])+(m[18]*m[20]-m[19]*m[19])*m[ 7]))+m[ 9]*((-m[11]*(m[20]*m[ 9]-m[10]*m[19]))+m[14]*(m[14]*m[ 9]-m[10]*m[13])+(m[13]*m[20]-m[14]*m[19])*m[ 7])-m[10]*((-m[11]*(m[19]*m[ 9]-m[10]*m[18]))+m[13]*(m[14]*m[ 9]-m[10]*m[13])+(m[13]*m[19]-m[14]*m[18])*m[ 7])+(m[11]*(m[18]*m[20]-m[19]*m[19])-m[13]*(m[13]*m[20]-m[14]*m[19])+m[14]*(m[13]*m[19]-m[14]*m[18]))*m[ 6])-m[ 1]*(((-m[11]*(m[20]*m[ 4]-m[19]*m[ 5]))+m[14]*(m[14]*m[ 4]-m[13]*m[ 5])+m[ 2]*(m[13]*m[20]-m[14]*m[19]))*m[ 9]-((-m[13]*(m[20]*m[ 4]-m[19]*m[ 5]))+m[14]*(m[19]*m[ 4]-m[18]*m[ 5])+m[ 2]*(m[18]*m[20]-m[19]*m[19]))*m[ 7]-m[10]*((-m[11]*(m[19]*m[ 4]-m[18]*m[ 5]))+m[13]*(m[14]*m[ 4]-m[13]*m[ 5])+(m[13]*m[19]-m[14]*m[18])*m[ 2])+m[ 1]*(m[11]*(m[18]*m[20]-m[19]*m[19])-m[13]*(m[13]*m[20]-m[14]*m[19])+m[14]*(m[13]*m[19]-m[14]*m[18])));
    c[16] = (-m[ 2]*((m[14]*(m[10]*m[ 3]-m[ 5]*m[ 8])+m[ 2]*(m[20]*m[ 8]-m[10]*m[17])-(m[20]*m[ 3]-m[17]*m[ 5])*m[ 7])*m[ 9]-m[10]*(m[13]*(m[10]*m[ 3]-m[ 5]*m[ 8])+m[ 2]*(m[19]*m[ 8]-m[10]*m[16])-(m[19]*m[ 3]-m[16]*m[ 5])*m[ 7])+m[ 1]*((-m[13]*(m[20]*m[ 8]-m[10]*m[17]))+m[14]*(m[19]*m[ 8]-m[10]*m[16])+(m[16]*m[20]-m[17]*m[19])*m[ 7])-((-m[13]*(m[20]*m[ 3]-m[17]*m[ 5]))+m[14]*(m[19]*m[ 3]-m[16]*m[ 5])+m[ 2]*(m[16]*m[20]-m[17]*m[19]))*m[ 6]))-m[ 5]*((-(m[11]*(m[10]*m[ 3]-m[ 5]*m[ 8])+m[ 2]*(m[14]*m[ 8]-m[10]*m[12])-(m[14]*m[ 3]-m[12]*m[ 5])*m[ 7])*m[ 9])+m[ 7]*(m[13]*(m[10]*m[ 3]-m[ 5]*m[ 8])+m[ 2]*(m[19]*m[ 8]-m[10]*m[16])-(m[19]*m[ 3]-m[16]*m[ 5])*m[ 7])+m[ 1]*((-m[11]*(m[19]*m[ 8]-m[10]*m[16]))+m[13]*(m[14]*m[ 8]-m[10]*m[12])+(m[12]*m[19]-m[14]*m[16])*m[ 7])-((-m[11]*(m[19]*m[ 3]-m[16]*m[ 5]))+m[13]*(m[14]*m[ 3]-m[12]*m[ 5])+(m[12]*m[19]-m[14]*m[16])*m[ 2])*m[ 6])-m[ 0]*(((-m[11]*(m[20]*m[ 8]-m[10]*m[17]))+m[14]*(m[14]*m[ 8]-m[10]*m[12])+(m[12]*m[20]-m[14]*m[17])*m[ 7])*m[ 9]-m[ 7]*((-m[13]*(m[20]*m[ 8]-m[10]*m[17]))+m[14]*(m[19]*m[ 8]-m[10]*m[16])+(m[16]*m[20]-m[17]*m[19])*m[ 7])-m[10]*((-m[11]*(m[19]*m[ 8]-m[10]*m[16]))+m[13]*(m[14]*m[ 8]-m[10]*m[12])+(m[12]*m[19]-m[14]*m[16])*m[ 7])+(m[11]*(m[16]*m[20]-m[17]*m[19])-m[13]*(m[12]*m[20]-m[14]*m[17])+m[14]*(m[12]*m[19]-m[14]*m[16]))*m[ 6])+m[ 1]*(((-m[11]*(m[20]*m[ 3]-m[17]*m[ 5]))+m[14]*(m[14]*m[ 3]-m[12]*m[ 5])+m[ 2]*(m[12]*m[20]-m[14]*m[17]))*m[ 9]-((-m[13]*(m[20]*m[ 3]-m[17]*m[ 5]))+m[14]*(m[19]*m[ 3]-m[16]*m[ 5])+m[ 2]*(m[16]*m[20]-m[17]*m[19]))*m[ 7]-m[10]*((-m[11]*(m[19]*m[ 3]-m[16]*m[ 5]))+m[13]*(m[14]*m[ 3]-m[12]*m[ 5])+(m[12]*m[19]-m[14]*m[16])*m[ 2])+m[ 1]*(m[11]*(m[16]*m[20]-m[17]*m[19])-m[13]*(m[12]*m[20]-m[14]*m[17])+m[14]*(m[12]*m[19]-m[14]*m[16])))+m[ 4]*(m[ 7]*(m[14]*(m[10]*m[ 3]-m[ 5]*m[ 8])+m[ 2]*(m[20]*m[ 8]-m[10]*m[17])-(m[20]*m[ 3]-m[17]*m[ 5])*m[ 7])-m[10]*(m[11]*(m[10]*m[ 3]-m[ 5]*m[ 8])+m[ 2]*(m[14]*m[ 8]-m[10]*m[12])-(m[14]*m[ 3]-m[12]*m[ 5])*m[ 7])+m[ 1]*((-m[11]*(m[20]*m[ 8]-m[10]*m[17]))+m[14]*(m[14]*m[ 8]-m[10]*m[12])+(m[12]*m[20]-m[14]*m[17])*m[ 7])-((-m[11]*(m[20]*m[ 3]-m[17]*m[ 5]))+m[14]*(m[14]*m[ 3]-m[12]*m[ 5])+m[ 2]*(m[12]*m[20]-m[14]*m[17]))*m[ 6]);
    c[17] = m[ 2]*(m[ 9]*(m[14]*(m[ 3]*m[ 9]-m[ 4]*m[ 8])+m[ 2]*(m[19]*m[ 8]-m[17]*m[ 9])-(m[19]*m[ 3]-m[17]*m[ 4])*m[ 7])-m[10]*(m[13]*(m[ 3]*m[ 9]-m[ 4]*m[ 8])+m[ 2]*(m[18]*m[ 8]-m[16]*m[ 9])-(m[18]*m[ 3]-m[16]*m[ 4])*m[ 7])+m[ 1]*((-m[13]*(m[19]*m[ 8]-m[17]*m[ 9]))+m[14]*(m[18]*m[ 8]-m[16]*m[ 9])+(m[16]*m[19]-m[17]*m[18])*m[ 7])-((-m[13]*(m[19]*m[ 3]-m[17]*m[ 4]))+m[14]*(m[18]*m[ 3]-m[16]*m[ 4])+(m[16]*m[19]-m[17]*m[18])*m[ 2])*m[ 6])-m[ 4]*(m[ 7]*(m[14]*(m[ 3]*m[ 9]-m[ 4]*m[ 8])+m[ 2]*(m[19]*m[ 8]-m[17]*m[ 9])-(m[19]*m[ 3]-m[17]*m[ 4])*m[ 7])-m[10]*(m[11]*(m[ 3]*m[ 9]-m[ 4]*m[ 8])+m[ 2]*(m[13]*m[ 8]-m[12]*m[ 9])-(m[13]*m[ 3]-m[12]*m[ 4])*m[ 7])+m[ 1]*((-m[11]*(m[19]*m[ 8]-m[17]*m[ 9]))+m[14]*(m[13]*m[ 8]-m[12]*m[ 9])+(m[12]*m[19]-m[13]*m[17])*m[ 7])-((-m[11]*(m[19]*m[ 3]-m[17]*m[ 4]))+m[14]*(m[13]*m[ 3]-m[12]*m[ 4])+(m[12]*m[19]-m[13]*m[17])*m[ 2])*m[ 6])+m[ 5]*(m[ 7]*(m[13]*(m[ 3]*m[ 9]-m[ 4]*m[ 8])+m[ 2]*(m[18]*m[ 8]-m[16]*m[ 9])-(m[18]*m[ 3]-m[16]*m[ 4])*m[ 7])-m[ 9]*(m[11]*(m[ 3]*m[ 9]-m[ 4]*m[ 8])+m[ 2]*(m[13]*m[ 8]-m[12]*m[ 9])-(m[13]*m[ 3]-m[12]*m[ 4])*m[ 7])+m[ 1]*((-m[11]*(m[18]*m[ 8]-m[16]*m[ 9]))+m[13]*(m[13]*m[ 8]-m[12]*m[ 9])+(m[12]*m[18]-m[13]*m[16])*m[ 7])-((-m[11]*(m[18]*m[ 3]-m[16]*m[ 4]))+m[13]*(m[13]*m[ 3]-m[12]*m[ 4])+(m[12]*m[18]-m[13]*m[16])*m[ 2])*m[ 6])+m[ 0]*((-m[ 7]*((-m[13]*(m[19]*m[ 8]-m[17]*m[ 9]))+m[14]*(m[18]*m[ 8]-m[16]*m[ 9])+(m[16]*m[19]-m[17]*m[18])*m[ 7]))+m[ 9]*((-m[11]*(m[19]*m[ 8]-m[17]*m[ 9]))+m[14]*(m[13]*m[ 8]-m[12]*m[ 9])+(m[12]*m[19]-m[13]*m[17])*m[ 7])-m[10]*((-m[11]*(m[18]*m[ 8]-m[16]*m[ 9]))+m[13]*(m[13]*m[ 8]-m[12]*m[ 9])+(m[12]*m[18]-m[13]*m[16])*m[ 7])+(m[11]*(m[16]*m[19]-m[17]*m[18])-m[13]*(m[12]*m[19]-m[13]*m[17])+m[14]*(m[12]*m[18]-m[13]*m[16]))*m[ 6])-m[ 1]*(((-m[11]*(m[19]*m[ 3]-m[17]*m[ 4]))+m[14]*(m[13]*m[ 3]-m[12]*m[ 4])+(m[12]*m[19]-m[13]*m[17])*m[ 2])*m[ 9]-((-m[13]*(m[19]*m[ 3]-m[17]*m[ 4]))+m[14]*(m[18]*m[ 3]-m[16]*m[ 4])+(m[16]*m[19]-m[17]*m[18])*m[ 2])*m[ 7]-m[10]*((-m[11]*(m[18]*m[ 3]-m[16]*m[ 4]))+m[13]*(m[13]*m[ 3]-m[12]*m[ 4])+(m[12]*m[18]-m[13]*m[16])*m[ 2])+m[ 1]*(m[11]*(m[16]*m[19]-m[17]*m[18])-m[13]*(m[12]*m[19]-m[13]*m[17])+m[14]*(m[12]*m[18]-m[13]*m[16])));
    c[18] = m[ 2]*(m[ 8]*(m[14]*(m[10]*m[ 3]-m[ 5]*m[ 8])+m[ 2]*(m[20]*m[ 8]-m[10]*m[17])-(m[20]*m[ 3]-m[17]*m[ 5])*m[ 7])-m[10]*(m[12]*(m[10]*m[ 3]-m[ 5]*m[ 8])+m[ 2]*(m[17]*m[ 8]-m[10]*m[15])-(m[17]*m[ 3]-m[15]*m[ 5])*m[ 7])+m[ 1]*((-m[12]*(m[20]*m[ 8]-m[10]*m[17]))+m[14]*(m[17]*m[ 8]-m[10]*m[15])+(m[15]*m[20]-m[17]*m[17])*m[ 7])-((-m[12]*(m[20]*m[ 3]-m[17]*m[ 5]))+m[14]*(m[17]*m[ 3]-m[15]*m[ 5])+m[ 2]*(m[15]*m[20]-m[17]*m[17]))*m[ 6])-m[ 3]*(m[ 7]*(m[14]*(m[10]*m[ 3]-m[ 5]*m[ 8])+m[ 2]*(m[20]*m[ 8]-m[10]*m[17])-(m[20]*m[ 3]-m[17]*m[ 5])*m[ 7])-m[10]*(m[11]*(m[10]*m[ 3]-m[ 5]*m[ 8])+m[ 2]*(m[14]*m[ 8]-m[10]*m[12])-(m[14]*m[ 3]-m[12]*m[ 5])*m[ 7])+m[ 1]*((-m[11]*(m[20]*m[ 8]-m[10]*m[17]))+m[14]*(m[14]*m[ 8]-m[10]*m[12])+(m[12]*m[20]-m[14]*m[17])*m[ 7])-((-m[11]*(m[20]*m[ 3]-m[17]*m[ 5]))+m[14]*(m[14]*m[ 3]-m[12]*m[ 5])+m[ 2]*(m[12]*m[20]-m[14]*m[17]))*m[ 6])+m[ 5]*(m[ 7]*(m[12]*(m[10]*m[ 3]-m[ 5]*m[ 8])+m[ 2]*(m[17]*m[ 8]-m[10]*m[15])-(m[17]*m[ 3]-m[15]*m[ 5])*m[ 7])-m[ 8]*(m[11]*(m[10]*m[ 3]-m[ 5]*m[ 8])+m[ 2]*(m[14]*m[ 8]-m[10]*m[12])-(m[14]*m[ 3]-m[12]*m[ 5])*m[ 7])+m[ 1]*((-m[11]*(m[17]*m[ 8]-m[10]*m[15]))+m[12]*(m[14]*m[ 8]-m[10]*m[12])+(m[12]*m[17]-m[14]*m[15])*m[ 7])-((-m[11]*(m[17]*m[ 3]-m[15]*m[ 5]))+m[12]*(m[14]*m[ 3]-m[12]*m[ 5])+(m[12]*m[17]-m[14]*m[15])*m[ 2])*m[ 6])+m[ 0]*((-m[ 7]*((-m[12]*(m[20]*m[ 8]-m[10]*m[17]))+m[14]*(m[17]*m[ 8]-m[10]*m[15])+(m[15]*m[20]-m[17]*m[17])*m[ 7]))+m[ 8]*((-m[11]*(m[20]*m[ 8]-m[10]*m[17]))+m[14]*(m[14]*m[ 8]-m[10]*m[12])+(m[12]*m[20]-m[14]*m[17])*m[ 7])-m[10]*((-m[11]*(m[17]*m[ 8]-m[10]*m[15]))+m[12]*(m[14]*m[ 8]-m[10]*m[12])+(m[12]*m[17]-m[14]*m[15])*m[ 7])+(m[11]*(m[15]*m[20]-m[17]*m[17])-m[12]*(m[12]*m[20]-m[14]*m[17])+m[14]*(m[12]*m[17]-m[14]*m[15]))*m[ 6])-m[ 1]*(((-m[11]*(m[20]*m[ 3]-m[17]*m[ 5]))+m[14]*(m[14]*m[ 3]-m[12]*m[ 5])+m[ 2]*(m[12]*m[20]-m[14]*m[17]))*m[ 8]-((-m[12]*(m[20]*m[ 3]-m[17]*m[ 5]))+m[14]*(m[17]*m[ 3]-m[15]*m[ 5])+m[ 2]*(m[15]*m[20]-m[17]*m[17]))*m[ 7]-m[10]*((-m[11]*(m[17]*m[ 3]-m[15]*m[ 5]))+m[12]*(m[14]*m[ 3]-m[12]*m[ 5])+(m[12]*m[17]-m[14]*m[15])*m[ 2])+m[ 1]*(m[11]*(m[15]*m[20]-m[17]*m[17])-m[12]*(m[12]*m[20]-m[14]*m[17])+m[14]*(m[12]*m[17]-m[14]*m[15])));
    c[19] = (-m[ 2]*(m[ 8]*(m[14]*(m[ 3]*m[ 9]-m[ 4]*m[ 8])+m[ 2]*(m[19]*m[ 8]-m[17]*m[ 9])-(m[19]*m[ 3]-m[17]*m[ 4])*m[ 7])-m[10]*(m[12]*(m[ 3]*m[ 9]-m[ 4]*m[ 8])+m[ 2]*(m[16]*m[ 8]-m[15]*m[ 9])-(m[16]*m[ 3]-m[15]*m[ 4])*m[ 7])+m[ 1]*((-m[12]*(m[19]*m[ 8]-m[17]*m[ 9]))+m[14]*(m[16]*m[ 8]-m[15]*m[ 9])+(m[15]*m[19]-m[16]*m[17])*m[ 7])-((-m[12]*(m[19]*m[ 3]-m[17]*m[ 4]))+m[14]*(m[16]*m[ 3]-m[15]*m[ 4])+(m[15]*m[19]-m[16]*m[17])*m[ 2])*m[ 6]))+m[ 3]*(m[ 7]*(m[14]*(m[ 3]*m[ 9]-m[ 4]*m[ 8])+m[ 2]*(m[19]*m[ 8]-m[17]*m[ 9])-(m[19]*m[ 3]-m[17]*m[ 4])*m[ 7])-m[10]*(m[11]*(m[ 3]*m[ 9]-m[ 4]*m[ 8])+m[ 2]*(m[13]*m[ 8]-m[12]*m[ 9])-(m[13]*m[ 3]-m[12]*m[ 4])*m[ 7])+m[ 1]*((-m[11]*(m[19]*m[ 8]-m[17]*m[ 9]))+m[14]*(m[13]*m[ 8]-m[12]*m[ 9])+(m[12]*m[19]-m[13]*m[17])*m[ 7])-((-m[11]*(m[19]*m[ 3]-m[17]*m[ 4]))+m[14]*(m[13]*m[ 3]-m[12]*m[ 4])+(m[12]*m[19]-m[13]*m[17])*m[ 2])*m[ 6])-m[ 5]*(m[ 7]*(m[12]*(m[ 3]*m[ 9]-m[ 4]*m[ 8])+m[ 2]*(m[16]*m[ 8]-m[15]*m[ 9])-(m[16]*m[ 3]-m[15]*m[ 4])*m[ 7])-m[ 8]*(m[11]*(m[ 3]*m[ 9]-m[ 4]*m[ 8])+m[ 2]*(m[13]*m[ 8]-m[12]*m[ 9])-(m[13]*m[ 3]-m[12]*m[ 4])*m[ 7])+m[ 1]*((-m[11]*(m[16]*m[ 8]-m[15]*m[ 9]))+m[12]*(m[13]*m[ 8]-m[12]*m[ 9])+(m[12]*m[16]-m[13]*m[15])*m[ 7])-((-m[11]*(m[16]*m[ 3]-m[15]*m[ 4]))+m[12]*(m[13]*m[ 3]-m[12]*m[ 4])+(m[12]*m[16]-m[13]*m[15])*m[ 2])*m[ 6])-m[ 0]*((-m[ 7]*((-m[12]*(m[19]*m[ 8]-m[17]*m[ 9]))+m[14]*(m[16]*m[ 8]-m[15]*m[ 9])+(m[15]*m[19]-m[16]*m[17])*m[ 7]))+m[ 8]*((-m[11]*(m[19]*m[ 8]-m[17]*m[ 9]))+m[14]*(m[13]*m[ 8]-m[12]*m[ 9])+(m[12]*m[19]-m[13]*m[17])*m[ 7])-m[10]*((-m[11]*(m[16]*m[ 8]-m[15]*m[ 9]))+m[12]*(m[13]*m[ 8]-m[12]*m[ 9])+(m[12]*m[16]-m[13]*m[15])*m[ 7])+(m[11]*(m[15]*m[19]-m[16]*m[17])-m[12]*(m[12]*m[19]-m[13]*m[17])+m[14]*(m[12]*m[16]-m[13]*m[15]))*m[ 6])+m[ 1]*(((-m[11]*(m[19]*m[ 3]-m[17]*m[ 4]))+m[14]*(m[13]*m[ 3]-m[12]*m[ 4])+(m[12]*m[19]-m[13]*m[17])*m[ 2])*m[ 8]-((-m[12]*(m[19]*m[ 3]-m[17]*m[ 4]))+m[14]*(m[16]*m[ 3]-m[15]*m[ 4])+(m[15]*m[19]-m[16]*m[17])*m[ 2])*m[ 7]-m[10]*((-m[11]*(m[16]*m[ 3]-m[15]*m[ 4]))+m[12]*(m[13]*m[ 3]-m[12]*m[ 4])+(m[12]*m[16]-m[13]*m[15])*m[ 2])+m[ 1]*(m[11]*(m[15]*m[19]-m[16]*m[17])-m[12]*(m[12]*m[19]-m[13]*m[17])+m[14]*(m[12]*m[16]-m[13]*m[15])));
    c[20] = m[ 2]*(m[ 8]*(m[13]*(m[ 3]*m[ 9]-m[ 4]*m[ 8])+m[ 2]*(m[18]*m[ 8]-m[16]*m[ 9])-(m[18]*m[ 3]-m[16]*m[ 4])*m[ 7])-m[ 9]*(m[12]*(m[ 3]*m[ 9]-m[ 4]*m[ 8])+m[ 2]*(m[16]*m[ 8]-m[15]*m[ 9])-(m[16]*m[ 3]-m[15]*m[ 4])*m[ 7])+m[ 1]*((-m[12]*(m[18]*m[ 8]-m[16]*m[ 9]))+m[13]*(m[16]*m[ 8]-m[15]*m[ 9])+(m[15]*m[18]-m[16]*m[16])*m[ 7])-((-m[12]*(m[18]*m[ 3]-m[16]*m[ 4]))+m[13]*(m[16]*m[ 3]-m[15]*m[ 4])+(m[15]*m[18]-m[16]*m[16])*m[ 2])*m[ 6])-m[ 3]*(m[ 7]*(m[13]*(m[ 3]*m[ 9]-m[ 4]*m[ 8])+m[ 2]*(m[18]*m[ 8]-m[16]*m[ 9])-(m[18]*m[ 3]-m[16]*m[ 4])*m[ 7])-m[ 9]*(m[11]*(m[ 3]*m[ 9]-m[ 4]*m[ 8])+m[ 2]*(m[13]*m[ 8]-m[12]*m[ 9])-(m[13]*m[ 3]-m[12]*m[ 4])*m[ 7])+m[ 1]*((-m[11]*(m[18]*m[ 8]-m[16]*m[ 9]))+m[13]*(m[13]*m[ 8]-m[12]*m[ 9])+(m[12]*m[18]-m[13]*m[16])*m[ 7])-((-m[11]*(m[18]*m[ 3]-m[16]*m[ 4]))+m[13]*(m[13]*m[ 3]-m[12]*m[ 4])+(m[12]*m[18]-m[13]*m[16])*m[ 2])*m[ 6])+m[ 4]*(m[ 7]*(m[12]*(m[ 3]*m[ 9]-m[ 4]*m[ 8])+m[ 2]*(m[16]*m[ 8]-m[15]*m[ 9])-(m[16]*m[ 3]-m[15]*m[ 4])*m[ 7])-m[ 8]*(m[11]*(m[ 3]*m[ 9]-m[ 4]*m[ 8])+m[ 2]*(m[13]*m[ 8]-m[12]*m[ 9])-(m[13]*m[ 3]-m[12]*m[ 4])*m[ 7])+m[ 1]*((-m[11]*(m[16]*m[ 8]-m[15]*m[ 9]))+m[12]*(m[13]*m[ 8]-m[12]*m[ 9])+(m[12]*m[16]-m[13]*m[15])*m[ 7])-((-m[11]*(m[16]*m[ 3]-m[15]*m[ 4]))+m[12]*(m[13]*m[ 3]-m[12]*m[ 4])+(m[12]*m[16]-m[13]*m[15])*m[ 2])*m[ 6])+m[ 0]*((-m[ 7]*((-m[12]*(m[18]*m[ 8]-m[16]*m[ 9]))+m[13]*(m[16]*m[ 8]-m[15]*m[ 9])+(m[15]*m[18]-m[16]*m[16])*m[ 7]))+m[ 8]*((-m[11]*(m[18]*m[ 8]-m[16]*m[ 9]))+m[13]*(m[13]*m[ 8]-m[12]*m[ 9])+(m[12]*m[18]-m[13]*m[16])*m[ 7])-m[ 9]*((-m[11]*(m[16]*m[ 8]-m[15]*m[ 9]))+m[12]*(m[13]*m[ 8]-m[12]*m[ 9])+(m[12]*m[16]-m[13]*m[15])*m[ 7])+(m[11]*(m[15]*m[18]-m[16]*m[16])-m[12]*(m[12]*m[18]-m[13]*m[16])+m[13]*(m[12]*m[16]-m[13]*m[15]))*m[ 6])-m[ 1]*((-((-m[11]*(m[16]*m[ 3]-m[15]*m[ 4]))+m[12]*(m[13]*m[ 3]-m[12]*m[ 4])+(m[12]*m[16]-m[13]*m[15])*m[ 2])*m[ 9])+((-m[11]*(m[18]*m[ 3]-m[16]*m[ 4]))+m[13]*(m[13]*m[ 3]-m[12]*m[ 4])+(m[12]*m[18]-m[13]*m[16])*m[ 2])*m[ 8]-((-m[12]*(m[18]*m[ 3]-m[16]*m[ 4]))+m[13]*(m[16]*m[ 3]-m[15]*m[ 4])+(m[15]*m[18]-m[16]*m[16])*m[ 2])*m[ 7]+m[ 1]*(m[11]*(m[15]*m[18]-m[16]*m[16])-m[12]*(m[12]*m[18]-m[13]*m[16])+m[13]*(m[12]*m[16]-m[13]*m[15])));

    return m[0]*c[0]+m[1]*c[1]+m[2]*c[2]+m[3]*c[3]+m[4]*c[4]+m[5]*c[5];
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
