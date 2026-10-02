#pragma once

#ifdef __cplusplus
#define restrict
#endif


#include "minimath_generated.h"

// In all the following computations I use the expression obtained before a
// maxima expand(). factorsum()-ing the result doesn't appear to do much.  Doing
// an expand() greatly increases the number of multiplications, but reduces the
// number of additions. In the specific case of a 4x4 symmetric determinant,
// expand() changes (23,40) to (16,58) +,*. 18 more *, but 7 fewer +.
// Session:
/*
(%i13) display2d : false;
(%o13) false

(%i31) sym2 : matrix([m0,m1],
                     [m1,m2]);

(%o31) matrix([m0,m1],[m1,m2])
(%i32) sym3 : matrix([m0,m1,m2],
                     [m1,m3,m4],
                     [m2,m4,m5]);

(%o32) matrix([m0,m1,m2],[m1,m3,m4],[m2,m4,m5])
(%i33) sym4 : matrix([m0,m1,m2,m3],
                     [m1,m4,m5,m6],
                     [m2,m5,m7,m8],
                     [m3,m6,m8,m9]);
(%i99) sym5 : matrix([m0,m1,m2, m3, m4 ],
                     [m1,m5,m6, m7, m8 ],
                     [m2,m6,m9, m10,m11],
                     [m3,m7,m10,m12,m13],
                     [m4,m8,m11,m13,m14]);

(%o99) matrix([m0,m1,m2,m3,m4],[m1,m5,m6,m7,m8],[m2,m6,m9,m10,m11],
              [m3,m7,m10,m12,m13],[m4,m8,m11,m13,m14])

(%i34) determinant(sym4);

(%o34) m0*(m4*(m7*m9-m8^2)-m5*(m5*m9-m6*m8)+m6*(m5*m8-m6*m7))
        -m1*(m1*(m7*m9-m8^2)-m5*(m2*m9-m3*m8)+m6*(m2*m8-m3*m7))
        +m2*(m1*(m5*m9-m6*m8)-m4*(m2*m9-m3*m8)+m6*(m2*m6-m3*m5))
        -m3*(m1*(m5*m8-m6*m7)-m4*(m2*m8-m3*m7)+m5*(m2*m6-m3*m5))
(%i35) expand(determinant(sym4));

(%o35) m0*m4*m7*m9-m1^2*m7*m9-m0*m5^2*m9+2*m1*m2*m5*m9-m2^2*m4*m9-m0*m4*m8^2
                  +m1^2*m8^2+2*m0*m5*m6*m8-2*m1*m2*m6*m8-2*m1*m3*m5*m8
                  +2*m2*m3*m4*m8-m0*m6^2*m7+2*m1*m3*m6*m7-m3^2*m4*m7+m2^2*m6^2
                  -2*m2*m3*m5*m6+m3^2*m5^2
*/

/* The session to compute the determinants appears above. The only postprocessing was to
   replace m.^2 -> m.*m, m. -> m[.]

(%i36) determinant(sym2);

(%o36) m0*m2-m1^2
(%i37) determinant(sym3);

(%o37) m0*(m3*m5-m4^2)-m1*(m1*m5-m2*m4)+m2*(m1*m4-m2*m3)
(%i38) determinant(sym4);

(%o38) m0*(m4*(m7*m9-m8^2)-m5*(m5*m9-m6*m8)+m6*(m5*m8-m6*m7))
        -m1*(m1*(m7*m9-m8^2)-m5*(m2*m9-m3*m8)+m6*(m2*m8-m3*m7))
        +m2*(m1*(m5*m9-m6*m8)-m4*(m2*m9-m3*m8)+m6*(m2*m6-m3*m5))
        -m3*(m1*(m5*m8-m6*m7)-m4*(m2*m8-m3*m7)+m5*(m2*m6-m3*m5))

(%i100) determinant(sym5);

(%o100) -m3*(-m8*((m3*m8-m4*m7)*m9+m2*(m11*m7-m10*m8)-(m11*m3-m10*m4)*m6)
            +m1*(-(m14*m7-m13*m8)*m9+m11*(m11*m7-m10*m8)+(m10*m14-m11*m13)*m6)
            -m5*(-(m14*m3-m13*m4)*m9+m11*(m11*m3-m10*m4)+(m10*m14-m11*m13)*m2)
            +m6*(m11*(m3*m8-m4*m7)+m2*(m14*m7-m13*m8)-(m14*m3-m13*m4)*m6))
         +m4*(-m7*((m3*m8-m4*m7)*m9+m2*(m11*m7-m10*m8)-(m11*m3-m10*m4)*m6)
             +m1*(-(m13*m7-m12*m8)*m9+m10*(m11*m7-m10*m8)
                                     +(m10*m13-m11*m12)*m6)
             -m5*(-(m13*m3-m12*m4)*m9+m10*(m11*m3-m10*m4)
                                     +(m10*m13-m11*m12)*m2)
             +m6*(m10*(m3*m8-m4*m7)+m2*(m13*m7-m12*m8)-(m13*m3-m12*m4)*m6))
         +m0*(m7*(-(m14*m7-m13*m8)*m9+m11*(m11*m7-m10*m8)
                                     +(m10*m14-m11*m13)*m6)
             -m8*(-(m13*m7-m12*m8)*m9+m10*(m11*m7-m10*m8)
                                     +(m10*m13-m11*m12)*m6)
             +m5*((m12*m14-m13^2)*m9-m10*(m10*m14-m11*m13)
                                    +m11*(m10*m13-m11*m12))
             -m6*(-m10*(m14*m7-m13*m8)+m11*(m13*m7-m12*m8)
                                      +(m12*m14-m13^2)*m6))
         -m1*(m7*(-(m14*m3-m13*m4)*m9+m11*(m11*m3-m10*m4)
                                     +(m10*m14-m11*m13)*m2)
             -m8*(-(m13*m3-m12*m4)*m9+m10*(m11*m3-m10*m4)
                                     +(m10*m13-m11*m12)*m2)
             +m1*((m12*m14-m13^2)*m9-m10*(m10*m14-m11*m13)
                                    +m11*(m10*m13-m11*m12))
             -(-m10*(m14*m3-m13*m4)+m11*(m13*m3-m12*m4)+(m12*m14-m13^2)*m2)
              *m6)
         +m2*(m7*(m11*(m3*m8-m4*m7)+m2*(m14*m7-m13*m8)-(m14*m3-m13*m4)*m6)
             -m8*(m10*(m3*m8-m4*m7)+m2*(m13*m7-m12*m8)-(m13*m3-m12*m4)*m6)
             +m1*(-m10*(m14*m7-m13*m8)+m11*(m13*m7-m12*m8)+(m12*m14-m13^2)*m6)
             -(-m10*(m14*m3-m13*m4)+m11*(m13*m3-m12*m4)+(m12*m14-m13^2)*m2)
              *m5)

*/

#if 0
#error You really should be using the cofactors functions below to do this. So far Ive only needed the determinant as a part of computing the inverse, and the below functions do this much more efficiently
static inline double det_sym2(const double* m)
{
  return m[0]*m[2]-m[1]*m[1];
}
static inline double det_sym3(const double* m)
{
  return m[0]*(m[3]*m[5]-m[4]*m[4])-m[1]*(m[1]*m[5]-m[2]*m[4])+m[2]*(m[1]*m[4]-m[2]*m[3]);
}
static inline double det_sym4(const double* m)
{
  return
    +m[0]*(m[4]*(m[7]*m[9]-m[8]*m[8])-m[5]*(m[5]*m[9]-m[6]*m[8])+m[6]*(m[5]*m[8]-m[6]*m[7]))
    -m[1]*(m[1]*(m[7]*m[9]-m[8]*m[8])-m[5]*(m[2]*m[9]-m[3]*m[8])+m[6]*(m[2]*m[8]-m[3]*m[7]))
    +m[2]*(m[1]*(m[5]*m[9]-m[6]*m[8])-m[4]*(m[2]*m[9]-m[3]*m[8])+m[6]*(m[2]*m[6]-m[3]*m[5]))
    -m[3]*(m[1]*(m[5]*m[8]-m[6]*m[7])-m[4]*(m[2]*m[8]-m[3]*m[7])+m[5]*(m[2]*m[6]-m[3]*m[5]));
}
static inline double det_sym5(const double* m)
{
  return
    -m[3]*(-m[8]*((m[3]*m[8]-m[4]*m[7])*m[9]+m[2]*(m[11]*m[7]-m[10]*m[8])-(m[11]*m[3]-m[10]*m[4])*m[6])
           +m[1]*(-(m[14]*m[7]-m[13]*m[8])*m[9]+m[11]*(m[11]*m[7]-m[10]*m[8])+(m[10]*m[14]-m[11]*m[13])*m[6])
           -m[5]*(-(m[14]*m[3]-m[13]*m[4])*m[9]+m[11]*(m[11]*m[3]-m[10]*m[4])+(m[10]*m[14]-m[11]*m[13])*m[2])
           +m[6]*(m[11]*(m[3]*m[8]-m[4]*m[7])+m[2]*(m[14]*m[7]-m[13]*m[8])-(m[14]*m[3]-m[13]*m[4])*m[6]))
    +m[4]*(-m[7]*((m[3]*m[8]-m[4]*m[7])*m[9]+m[2]*(m[11]*m[7]-m[10]*m[8])-(m[11]*m[3]-m[10]*m[4])*m[6])
           +m[1]*(-(m[13]*m[7]-m[12]*m[8])*m[9]+m[10]*(m[11]*m[7]-m[10]*m[8])
                  +(m[10]*m[13]-m[11]*m[12])*m[6])
           -m[5]*(-(m[13]*m[3]-m[12]*m[4])*m[9]+m[10]*(m[11]*m[3]-m[10]*m[4])
                  +(m[10]*m[13]-m[11]*m[12])*m[2])
           +m[6]*(m[10]*(m[3]*m[8]-m[4]*m[7])+m[2]*(m[13]*m[7]-m[12]*m[8])-(m[13]*m[3]-m[12]*m[4])*m[6]))
    +m[0]*(m[7]*(-(m[14]*m[7]-m[13]*m[8])*m[9]+m[11]*(m[11]*m[7]-m[10]*m[8])
                 +(m[10]*m[14]-m[11]*m[13])*m[6])
           -m[8]*(-(m[13]*m[7]-m[12]*m[8])*m[9]+m[10]*(m[11]*m[7]-m[10]*m[8])
                  +(m[10]*m[13]-m[11]*m[12])*m[6])
           +m[5]*((m[12]*m[14]-m[13]*m[13])*m[9]-m[10]*(m[10]*m[14]-m[11]*m[13])
                  +m[11]*(m[10]*m[13]-m[11]*m[12]))
           -m[6]*(-m[10]*(m[14]*m[7]-m[13]*m[8])+m[11]*(m[13]*m[7]-m[12]*m[8])
                  +(m[12]*m[14]-m[13]*m[13])*m[6]))
    -m[1]*(m[7]*(-(m[14]*m[3]-m[13]*m[4])*m[9]+m[11]*(m[11]*m[3]-m[10]*m[4])
                 +(m[10]*m[14]-m[11]*m[13])*m[2])
           -m[8]*(-(m[13]*m[3]-m[12]*m[4])*m[9]+m[10]*(m[11]*m[3]-m[10]*m[4])
                  +(m[10]*m[13]-m[11]*m[12])*m[2])
           +m[1]*((m[12]*m[14]-m[13]*m[13])*m[9]-m[10]*(m[10]*m[14]-m[11]*m[13])
                  +m[11]*(m[10]*m[13]-m[11]*m[12]))
           -(-m[10]*(m[14]*m[3]-m[13]*m[4])+m[11]*(m[13]*m[3]-m[12]*m[4])+(m[12]*m[14]-m[13]*m[13])*m[2])
           *m[6])
    +m[2]*(m[7]*(m[11]*(m[3]*m[8]-m[4]*m[7])+m[2]*(m[14]*m[7]-m[13]*m[8])-(m[14]*m[3]-m[13]*m[4])*m[6])
           -m[8]*(m[10]*(m[3]*m[8]-m[4]*m[7])+m[2]*(m[13]*m[7]-m[12]*m[8])-(m[13]*m[3]-m[12]*m[4])*m[6])
           +m[1]*(-m[10]*(m[14]*m[7]-m[13]*m[8])+m[11]*(m[13]*m[7]-m[12]*m[8])+(m[12]*m[14]-m[13]*m[13])*m[6])
           -(-m[10]*(m[14]*m[3]-m[13]*m[4])+m[11]*(m[13]*m[3]-m[12]*m[4])+(m[12]*m[14]-m[13]*m[13])*m[2])
           *m[5]);
}
#endif

/* I now do inverses. I return transposed cofactors. Original matrix * cofactors / det = identity
As before, I replaced  m.^2 -> m.*m, m. -> m[.]
I also removed the lower triangle, since I'm dealing with symmetric matrices here
Session:


(%i64) num( ev(invert(sym2),detout) );

(%o64) matrix([m2,-m1],[-m1,m0])
(%i65) num( ev(invert(sym3),detout) );

(%o65) matrix([m3*m5-m4^2,m2*m4-m1*m5,m1*m4-m2*m3],
              [m2*m4-m1*m5,m0*m5-m2^2,m1*m2-m0*m4],
              [m1*m4-m2*m3,m1*m2-m0*m4,m0*m3-m1^2])
(%i66) num( ev(invert(sym4),detout) );

(%o66) matrix([m4*(m7*m9-m8^2)-m5*(m5*m9-m6*m8)+m6*(m5*m8-m6*m7),
               -m1*(m7*m9-m8^2)+m2*(m5*m9-m6*m8)-m3*(m5*m8-m6*m7),
               m1*(m5*m9-m6*m8)-m2*(m4*m9-m6^2)+m3*(m4*m8-m5*m6),
               -m1*(m5*m8-m6*m7)+m2*(m4*m8-m5*m6)-m3*(m4*m7-m5^2)],
              [-m1*(m7*m9-m8^2)+m5*(m2*m9-m3*m8)-m6*(m2*m8-m3*m7),
               m0*(m7*m9-m8^2)-m2*(m2*m9-m3*m8)+m3*(m2*m8-m3*m7),
               -m0*(m5*m9-m6*m8)+m2*(m1*m9-m3*m6)-m3*(m1*m8-m3*m5),
               m0*(m5*m8-m6*m7)-m2*(m1*m8-m2*m6)+m3*(m1*m7-m2*m5)],
              [m1*(m5*m9-m6*m8)-m4*(m2*m9-m3*m8)+m6*(m2*m6-m3*m5),
               -m0*(m5*m9-m6*m8)+m1*(m2*m9-m3*m8)-m3*(m2*m6-m3*m5),
               m0*(m4*m9-m6^2)-m1*(m1*m9-m3*m6)+m3*(m1*m6-m3*m4),
               -m0*(m4*m8-m5*m6)+m1*(m1*m8-m2*m6)-m3*(m1*m5-m2*m4)],
              [-m1*(m5*m8-m6*m7)+m4*(m2*m8-m3*m7)-m5*(m2*m6-m3*m5),
               m0*(m5*m8-m6*m7)-m1*(m2*m8-m3*m7)+m2*(m2*m6-m3*m5),
               -m0*(m4*m8-m5*m6)+m1*(m1*m8-m3*m5)-m2*(m1*m6-m3*m4),
               m0*(m4*m7-m5^2)-m1*(m1*m7-m2*m5)+m2*(m1*m5-m2*m4)])
(%i103) num( ev(invert(sym5),detout) );

(%o103) matrix([m7*(-(m14*m7-m13*m8)*m9+m11*(m11*m7-m10*m8)
                                       +(m10*m14-m11*m13)*m6)
                 -m8*(-(m13*m7-m12*m8)*m9+m10*(m11*m7-m10*m8)
                                         +(m10*m13-m11*m12)*m6)
                 +m5*((m12*m14-m13^2)*m9-m10*(m10*m14-m11*m13)
                                        +m11*(m10*m13-m11*m12))
                 -m6*(-m10*(m14*m7-m13*m8)+m11*(m13*m7-m12*m8)
                                          +(m12*m14-m13^2)*m6),
                -m3*(-(m14*m7-m13*m8)*m9+m11*(m11*m7-m10*m8)
                                        +(m10*m14-m11*m13)*m6)
                 +m4*(-(m13*m7-m12*m8)*m9+m10*(m11*m7-m10*m8)
                                         +(m10*m13-m11*m12)*m6)
                 -m1*((m12*m14-m13^2)*m9-m10*(m10*m14-m11*m13)
                                        +m11*(m10*m13-m11*m12))
                 +m2*(-m10*(m14*m7-m13*m8)+m11*(m13*m7-m12*m8)
                                          +(m12*m14-m13^2)*m6),
                -m2*(-m7*(m14*m7-m13*m8)+m8*(m13*m7-m12*m8)
                                        +(m12*m14-m13^2)*m5)
                 +m3*(-m6*(m14*m7-m13*m8)+m8*(m11*m7-m10*m8)
                                         +(m10*m14-m11*m13)*m5)
                 -m4*(-m6*(m13*m7-m12*m8)+m7*(m11*m7-m10*m8)
                                         +(m10*m13-m11*m12)*m5)
                 +m1*((m10*m13-m11*m12)*m8-(m10*m14-m11*m13)*m7
                                          +(m12*m14-m13^2)*m6),
                -m3*(m8*(m11*m6-m8*m9)+m5*(m14*m9-m11^2)-m6*(m14*m6-m11*m8))
                 +m4*(m7*(m11*m6-m8*m9)+m5*(m13*m9-m10*m11)
                                       -m6*(m13*m6-m10*m8))
                 -m1*(-m7*(m14*m9-m11^2)+m8*(m13*m9-m10*m11)
                                        +(m10*m14-m11*m13)*m6)
                 +m2*(-m7*(m14*m6-m11*m8)+m8*(m13*m6-m10*m8)
                                         +(m10*m14-m11*m13)*m5),
                m3*(m8*(m10*m6-m7*m9)+m5*(m13*m9-m10*m11)-m6*(m13*m6-m11*m7))
                 -m4*(m7*(m10*m6-m7*m9)+m5*(m12*m9-m10^2)-m6*(m12*m6-m10*m7))
                 +m1*(-m7*(m13*m9-m10*m11)+m8*(m12*m9-m10^2)
                                          +(m10*m13-m11*m12)*m6)
                 -m2*((m12*m6-m10*m7)*m8-m7*(m13*m6-m11*m7)
                                        +(m10*m13-m11*m12)*m5)],
               [-m7*(-(m14*m3-m13*m4)*m9+m11*(m11*m3-m10*m4)
                                        +(m10*m14-m11*m13)*m2)
                 +m8*(-(m13*m3-m12*m4)*m9+m10*(m11*m3-m10*m4)
                                         +(m10*m13-m11*m12)*m2)
                 -m1*((m12*m14-m13^2)*m9-m10*(m10*m14-m11*m13)
                                        +m11*(m10*m13-m11*m12))
                 +(-m10*(m14*m3-m13*m4)+m11*(m13*m3-m12*m4)
                                       +(m12*m14-m13^2)*m2)
                  *m6,
                m3*(-(m14*m3-m13*m4)*m9+m11*(m11*m3-m10*m4)
                                       +(m10*m14-m11*m13)*m2)
                 -m4*(-(m13*m3-m12*m4)*m9+m10*(m11*m3-m10*m4)
                                         +(m10*m13-m11*m12)*m2)
                 +m0*((m12*m14-m13^2)*m9-m10*(m10*m14-m11*m13)
                                        +m11*(m10*m13-m11*m12))
                 -m2*(-m10*(m14*m3-m13*m4)+m11*(m13*m3-m12*m4)
                                          +(m12*m14-m13^2)*m2),
                m2*((m13*m3-m12*m4)*m8-(m14*m3-m13*m4)*m7+m1*(m12*m14-m13^2))
                 -m3*((m11*m3-m10*m4)*m8-(m14*m3-m13*m4)*m6
                                        +m1*(m10*m14-m11*m13))
                 -m0*((m10*m13-m11*m12)*m8-(m10*m14-m11*m13)*m7
                                          +(m12*m14-m13^2)*m6)
                 +m4*((m11*m3-m10*m4)*m7-(m13*m3-m12*m4)*m6
                                        +m1*(m10*m13-m11*m12)),
                m3*(m8*(m11*m2-m4*m9)+m1*(m14*m9-m11^2)-(m14*m2-m11*m4)*m6)
                 -m4*(m7*(m11*m2-m4*m9)+m1*(m13*m9-m10*m11)
                                       -(m13*m2-m10*m4)*m6)
                 +m0*(-m7*(m14*m9-m11^2)+m8*(m13*m9-m10*m11)
                                        +(m10*m14-m11*m13)*m6)
                 -m2*((m13*m2-m10*m4)*m8-(m14*m2-m11*m4)*m7
                                        +m1*(m10*m14-m11*m13)),
                -m3*(m8*(m10*m2-m3*m9)+m1*(m13*m9-m10*m11)-(m13*m2-m11*m3)*m6)
                 +m4*(m7*(m10*m2-m3*m9)+m1*(m12*m9-m10^2)-(m12*m2-m10*m3)*m6)
                 -m0*(-m7*(m13*m9-m10*m11)+m8*(m12*m9-m10^2)
                                          +(m10*m13-m11*m12)*m6)
                 +m2*((m12*m2-m10*m3)*m8-(m13*m2-m11*m3)*m7
                                        +m1*(m10*m13-m11*m12))],
               [m7*(m11*(m3*m8-m4*m7)+m2*(m14*m7-m13*m8)-(m14*m3-m13*m4)*m6)
                 -m8*(m10*(m3*m8-m4*m7)+m2*(m13*m7-m12*m8)-(m13*m3-m12*m4)*m6)
                 +m1*(-m10*(m14*m7-m13*m8)+m11*(m13*m7-m12*m8)
                                          +(m12*m14-m13^2)*m6)
                 -(-m10*(m14*m3-m13*m4)+m11*(m13*m3-m12*m4)
                                       +(m12*m14-m13^2)*m2)
                  *m5,
                -m3*(m11*(m3*m8-m4*m7)+m2*(m14*m7-m13*m8)-(m14*m3-m13*m4)*m6)
                 +m4*(m10*(m3*m8-m4*m7)+m2*(m13*m7-m12*m8)-(m13*m3-m12*m4)*m6)
                 -m0*(-m10*(m14*m7-m13*m8)+m11*(m13*m7-m12*m8)
                                          +(m12*m14-m13^2)*m6)
                 +m1*(-m10*(m14*m3-m13*m4)+m11*(m13*m3-m12*m4)
                                          +(m12*m14-m13^2)*m2),
                m3*(m8*(m3*m8-m4*m7)+m1*(m14*m7-m13*m8)-(m14*m3-m13*m4)*m5)
                 -m4*(m7*(m3*m8-m4*m7)+m1*(m13*m7-m12*m8)-(m13*m3-m12*m4)*m5)
                 +m0*(-m7*(m14*m7-m13*m8)+m8*(m13*m7-m12*m8)
                                         +(m12*m14-m13^2)*m5)
                 -m1*((m13*m3-m12*m4)*m8-(m14*m3-m13*m4)*m7
                                        +m1*(m12*m14-m13^2)),
                -m3*(m8*(m2*m8-m4*m6)+m1*(m14*m6-m11*m8)-(m14*m2-m11*m4)*m5)
                 +m4*(m7*(m2*m8-m4*m6)+m1*(m13*m6-m10*m8)-(m13*m2-m10*m4)*m5)
                 -m0*(-m7*(m14*m6-m11*m8)+m8*(m13*m6-m10*m8)
                                         +(m10*m14-m11*m13)*m5)
                 +m1*((m13*m2-m10*m4)*m8-(m14*m2-m11*m4)*m7
                                        +m1*(m10*m14-m11*m13)),
                m3*((m2*m7-m3*m6)*m8+m1*(m13*m6-m11*m7)-(m13*m2-m11*m3)*m5)
                 +m0*((m12*m6-m10*m7)*m8-m7*(m13*m6-m11*m7)
                                        +(m10*m13-m11*m12)*m5)
                 -m1*((m12*m2-m10*m3)*m8-(m13*m2-m11*m3)*m7
                                        +m1*(m10*m13-m11*m12))
                 -m4*(m7*(m2*m7-m3*m6)+m1*(m12*m6-m10*m7)
                                      -(m12*m2-m10*m3)*m5)],
               [m8*((m3*m8-m4*m7)*m9+m2*(m11*m7-m10*m8)-(m11*m3-m10*m4)*m6)
                 -m1*(-(m14*m7-m13*m8)*m9+m11*(m11*m7-m10*m8)
                                         +(m10*m14-m11*m13)*m6)
                 +m5*(-(m14*m3-m13*m4)*m9+m11*(m11*m3-m10*m4)
                                         +(m10*m14-m11*m13)*m2)
                 -m6*(m11*(m3*m8-m4*m7)+m2*(m14*m7-m13*m8)
                                       -(m14*m3-m13*m4)*m6),
                -m4*((m3*m8-m4*m7)*m9+m2*(m11*m7-m10*m8)-(m11*m3-m10*m4)*m6)
                 +m0*(-(m14*m7-m13*m8)*m9+m11*(m11*m7-m10*m8)
                                         +(m10*m14-m11*m13)*m6)
                 -m1*(-(m14*m3-m13*m4)*m9+m11*(m11*m3-m10*m4)
                                         +(m10*m14-m11*m13)*m2)
                 +m2*(m11*(m3*m8-m4*m7)+m2*(m14*m7-m13*m8)
                                       -(m14*m3-m13*m4)*m6),
                -m2*(m8*(m3*m8-m4*m7)+m1*(m14*m7-m13*m8)-(m14*m3-m13*m4)*m5)
                 +m4*(m6*(m3*m8-m4*m7)+m1*(m11*m7-m10*m8)-(m11*m3-m10*m4)*m5)
                 -m0*(-m6*(m14*m7-m13*m8)+m8*(m11*m7-m10*m8)
                                         +(m10*m14-m11*m13)*m5)
                 +m1*((m11*m3-m10*m4)*m8-(m14*m3-m13*m4)*m6
                                        +m1*(m10*m14-m11*m13)),
                m0*(m8*(m11*m6-m8*m9)+m5*(m14*m9-m11^2)-m6*(m14*m6-m11*m8))
                 -m4*(m1*(m11*m6-m8*m9)-m5*(m11*m2-m4*m9)+m6*(m2*m8-m4*m6))
                 -m1*(m8*(m11*m2-m4*m9)+m1*(m14*m9-m11^2)-(m14*m2-m11*m4)*m6)
                 +m2*(m8*(m2*m8-m4*m6)+m1*(m14*m6-m11*m8)-(m14*m2-m11*m4)*m5),
                -m0*(m8*(m10*m6-m7*m9)+m5*(m13*m9-m10*m11)-m6*(m13*m6-m11*m7))
                 +m4*(m1*(m10*m6-m7*m9)-m5*(m10*m2-m3*m9)+m6*(m2*m7-m3*m6))
                 +m1*(m8*(m10*m2-m3*m9)+m1*(m13*m9-m10*m11)
                                       -(m13*m2-m11*m3)*m6)
                 -m2*((m2*m7-m3*m6)*m8+m1*(m13*m6-m11*m7)
                                      -(m13*m2-m11*m3)*m5)],
               [-m7*((m3*m8-m4*m7)*m9+m2*(m11*m7-m10*m8)-(m11*m3-m10*m4)*m6)
                 +m1*(-(m13*m7-m12*m8)*m9+m10*(m11*m7-m10*m8)
                                         +(m10*m13-m11*m12)*m6)
                 -m5*(-(m13*m3-m12*m4)*m9+m10*(m11*m3-m10*m4)
                                         +(m10*m13-m11*m12)*m2)
                 +m6*(m10*(m3*m8-m4*m7)+m2*(m13*m7-m12*m8)
                                       -(m13*m3-m12*m4)*m6),
                m3*((m3*m8-m4*m7)*m9+m2*(m11*m7-m10*m8)-(m11*m3-m10*m4)*m6)
                 -m0*(-(m13*m7-m12*m8)*m9+m10*(m11*m7-m10*m8)
                                         +(m10*m13-m11*m12)*m6)
                 +m1*(-(m13*m3-m12*m4)*m9+m10*(m11*m3-m10*m4)
                                         +(m10*m13-m11*m12)*m2)
                 -m2*(m10*(m3*m8-m4*m7)+m2*(m13*m7-m12*m8)
                                       -(m13*m3-m12*m4)*m6),
                m2*(m7*(m3*m8-m4*m7)+m1*(m13*m7-m12*m8)-(m13*m3-m12*m4)*m5)
                 -m3*(m6*(m3*m8-m4*m7)+m1*(m11*m7-m10*m8)-(m11*m3-m10*m4)*m5)
                 +m0*(-m6*(m13*m7-m12*m8)+m7*(m11*m7-m10*m8)
                                         +(m10*m13-m11*m12)*m5)
                 -m1*((m11*m3-m10*m4)*m7-(m13*m3-m12*m4)*m6
                                        +m1*(m10*m13-m11*m12)),
                -m0*(m7*(m11*m6-m8*m9)+m5*(m13*m9-m10*m11)-m6*(m13*m6-m10*m8))
                 +m3*(m1*(m11*m6-m8*m9)-m5*(m11*m2-m4*m9)+m6*(m2*m8-m4*m6))
                 +m1*(m7*(m11*m2-m4*m9)+m1*(m13*m9-m10*m11)
                                       -(m13*m2-m10*m4)*m6)
                 -m2*(m7*(m2*m8-m4*m6)+m1*(m13*m6-m10*m8)-(m13*m2-m10*m4)*m5),
                m0*(m7*(m10*m6-m7*m9)+m5*(m12*m9-m10^2)-m6*(m12*m6-m10*m7))
                 -m3*(m1*(m10*m6-m7*m9)-m5*(m10*m2-m3*m9)+m6*(m2*m7-m3*m6))
                 -m1*(m7*(m10*m2-m3*m9)+m1*(m12*m9-m10^2)-(m12*m2-m10*m3)*m6)
                 +m2*(m7*(m2*m7-m3*m6)+m1*(m12*m6-m10*m7)
                                      -(m12*m2-m10*m3)*m5)])
*/

static inline double cofactors_sym2(const double* restrict m, double* restrict c)
{
  c[0] = m[2];
  c[1] = -m[1];
  c[2] = -m[1];

  return m[0]*c[0] + m[1]*c[1];
}

static inline double cofactors_sym3(const double* restrict m, double* restrict c)
{
  c[0] = m[3]*m[5]-m[4]*m[4];
  c[1] = m[2]*m[4]-m[1]*m[5];
  c[2] = m[1]*m[4]-m[2]*m[3];
  c[3] = m[0]*m[5]-m[2]*m[2];
  c[4] = m[1]*m[2]-m[0]*m[4];
  c[5] = m[0]*m[3]-m[1]*m[1];

  return m[0]*c[0] + m[1]*c[1] + m[2]*c[2];
}

static inline double cofactors_sym4(const double* restrict m, double* restrict c)
{
  c[0] = m[4]*(m[7]*m[9]-m[8]*m[8])-m[5]*(m[5]*m[9]-m[6]*m[8])+m[6]*(m[5]*m[8]-m[6]*m[7]);
  c[1] = -m[1]*(m[7]*m[9]-m[8]*m[8])+m[2]*(m[5]*m[9]-m[6]*m[8])-m[3]*(m[5]*m[8]-m[6]*m[7]);
  c[2] = m[1]*(m[5]*m[9]-m[6]*m[8])-m[2]*(m[4]*m[9]-m[6]*m[6])+m[3]*(m[4]*m[8]-m[5]*m[6]);
  c[3] = -m[1]*(m[5]*m[8]-m[6]*m[7])+m[2]*(m[4]*m[8]-m[5]*m[6])-m[3]*(m[4]*m[7]-m[5]*m[5]);
  c[4] = m[0]*(m[7]*m[9]-m[8]*m[8])-m[2]*(m[2]*m[9]-m[3]*m[8])+m[3]*(m[2]*m[8]-m[3]*m[7]);
  c[5] = -m[0]*(m[5]*m[9]-m[6]*m[8])+m[2]*(m[1]*m[9]-m[3]*m[6])-m[3]*(m[1]*m[8]-m[3]*m[5]);
  c[6] = m[0]*(m[5]*m[8]-m[6]*m[7])-m[2]*(m[1]*m[8]-m[2]*m[6])+m[3]*(m[1]*m[7]-m[2]*m[5]);
  c[7] = m[0]*(m[4]*m[9]-m[6]*m[6])-m[1]*(m[1]*m[9]-m[3]*m[6])+m[3]*(m[1]*m[6]-m[3]*m[4]);
  c[8] = -m[0]*(m[4]*m[8]-m[5]*m[6])+m[1]*(m[1]*m[8]-m[2]*m[6])-m[3]*(m[1]*m[5]-m[2]*m[4]);
  c[9] = m[0]*(m[4]*m[7]-m[5]*m[5])-m[1]*(m[1]*m[7]-m[2]*m[5])+m[2]*(m[1]*m[5]-m[2]*m[4]);

  return m[0]*c[0] + m[1]*c[1] + m[2]*c[2] + m[3]*c[3];
}

static inline double cofactors_sym5(const double* restrict m, double* restrict c)
{
  c[0] =   m[7]*(-(m[14]*m[7]-m[13]*m[8])*m[9]+m[11]*(m[11]*m[7]-m[10]*m[8])+(m[10]*m[14]-m[11]*m[13])*m[6])-m[8]*(-(m[13]*m[7]-m[12]*m[8])*m[9]+m[10]*(m[11]*m[7]-m[10]*m[8])+(m[10]*m[13]-m[11]*m[12])*m[6])+m[5]*((m[12]*m[14]-m[13]*m[13])*m[9]-m[10]*(m[10]*m[14]-m[11]*m[13])+m[11]*(m[10]*m[13]-m[11]*m[12]))-m[6]*(-m[10]*(m[14]*m[7]-m[13]*m[8])+m[11]*(m[13]*m[7]-m[12]*m[8])+(m[12]*m[14]-m[13]*m[13])*m[6]);
  c[1] =   -m[3]*(-(m[14]*m[7]-m[13]*m[8])*m[9]+m[11]*(m[11]*m[7]-m[10]*m[8])+(m[10]*m[14]-m[11]*m[13])*m[6])+m[4]*(-(m[13]*m[7]-m[12]*m[8])*m[9]+m[10]*(m[11]*m[7]-m[10]*m[8])+(m[10]*m[13]-m[11]*m[12])*m[6])-m[1]*((m[12]*m[14]-m[13]*m[13])*m[9]-m[10]*(m[10]*m[14]-m[11]*m[13])+m[11]*(m[10]*m[13]-m[11]*m[12]))+m[2]*(-m[10]*(m[14]*m[7]-m[13]*m[8])+m[11]*(m[13]*m[7]-m[12]*m[8])+(m[12]*m[14]-m[13]*m[13])*m[6]);
  c[2] =   -m[2]*(-m[7]*(m[14]*m[7]-m[13]*m[8])+m[8]*(m[13]*m[7]-m[12]*m[8])+(m[12]*m[14]-m[13]*m[13])*m[5])+m[3]*(-m[6]*(m[14]*m[7]-m[13]*m[8])+m[8]*(m[11]*m[7]-m[10]*m[8])+(m[10]*m[14]-m[11]*m[13])*m[5])-m[4]*(-m[6]*(m[13]*m[7]-m[12]*m[8])+m[7]*(m[11]*m[7]-m[10]*m[8])+(m[10]*m[13]-m[11]*m[12])*m[5])+m[1]*((m[10]*m[13]-m[11]*m[12])*m[8]-(m[10]*m[14]-m[11]*m[13])*m[7]+(m[12]*m[14]-m[13]*m[13])*m[6]);
  c[3] =   -m[3]*(m[8]*(m[11]*m[6]-m[8]*m[9])+m[5]*(m[14]*m[9]-m[11]*m[11])-m[6]*(m[14]*m[6]-m[11]*m[8]))+m[4]*(m[7]*(m[11]*m[6]-m[8]*m[9])+m[5]*(m[13]*m[9]-m[10]*m[11])-m[6]*(m[13]*m[6]-m[10]*m[8]))-m[1]*(-m[7]*(m[14]*m[9]-m[11]*m[11])+m[8]*(m[13]*m[9]-m[10]*m[11])+(m[10]*m[14]-m[11]*m[13])*m[6])+m[2]*(-m[7]*(m[14]*m[6]-m[11]*m[8])+m[8]*(m[13]*m[6]-m[10]*m[8])+(m[10]*m[14]-m[11]*m[13])*m[5]);
  c[4] =   m[3]*(m[8]*(m[10]*m[6]-m[7]*m[9])+m[5]*(m[13]*m[9]-m[10]*m[11])-m[6]*(m[13]*m[6]-m[11]*m[7]))-m[4]*(m[7]*(m[10]*m[6]-m[7]*m[9])+m[5]*(m[12]*m[9]-m[10]*m[10])-m[6]*(m[12]*m[6]-m[10]*m[7]))+m[1]*(-m[7]*(m[13]*m[9]-m[10]*m[11])+m[8]*(m[12]*m[9]-m[10]*m[10])+(m[10]*m[13]-m[11]*m[12])*m[6])-m[2]*((m[12]*m[6]-m[10]*m[7])*m[8]-m[7]*(m[13]*m[6]-m[11]*m[7])+(m[10]*m[13]-m[11]*m[12])*m[5]);
  c[5] =   m[3]*(-(m[14]*m[3]-m[13]*m[4])*m[9]+m[11]*(m[11]*m[3]-m[10]*m[4])+(m[10]*m[14]-m[11]*m[13])*m[2])-m[4]*(-(m[13]*m[3]-m[12]*m[4])*m[9]+m[10]*(m[11]*m[3]-m[10]*m[4])+(m[10]*m[13]-m[11]*m[12])*m[2])+m[0]*((m[12]*m[14]-m[13]*m[13])*m[9]-m[10]*(m[10]*m[14]-m[11]*m[13])+m[11]*(m[10]*m[13]-m[11]*m[12]))-m[2]*(-m[10]*(m[14]*m[3]-m[13]*m[4])+m[11]*(m[13]*m[3]-m[12]*m[4])+(m[12]*m[14]-m[13]*m[13])*m[2]);
  c[6] =   m[2]*((m[13]*m[3]-m[12]*m[4])*m[8]-(m[14]*m[3]-m[13]*m[4])*m[7]+m[1]*(m[12]*m[14]-m[13]*m[13]))-m[3]*((m[11]*m[3]-m[10]*m[4])*m[8]-(m[14]*m[3]-m[13]*m[4])*m[6]+m[1]*(m[10]*m[14]-m[11]*m[13]))-m[0]*((m[10]*m[13]-m[11]*m[12])*m[8]-(m[10]*m[14]-m[11]*m[13])*m[7]+(m[12]*m[14]-m[13]*m[13])*m[6])+m[4]*((m[11]*m[3]-m[10]*m[4])*m[7]-(m[13]*m[3]-m[12]*m[4])*m[6]+m[1]*(m[10]*m[13]-m[11]*m[12]));
  c[7] =   m[3]*(m[8]*(m[11]*m[2]-m[4]*m[9])+m[1]*(m[14]*m[9]-m[11]*m[11])-(m[14]*m[2]-m[11]*m[4])*m[6])-m[4]*(m[7]*(m[11]*m[2]-m[4]*m[9])+m[1]*(m[13]*m[9]-m[10]*m[11])-(m[13]*m[2]-m[10]*m[4])*m[6])+m[0]*(-m[7]*(m[14]*m[9]-m[11]*m[11])+m[8]*(m[13]*m[9]-m[10]*m[11])+(m[10]*m[14]-m[11]*m[13])*m[6])-m[2]*((m[13]*m[2]-m[10]*m[4])*m[8]-(m[14]*m[2]-m[11]*m[4])*m[7]+m[1]*(m[10]*m[14]-m[11]*m[13]));
  c[8] =   -m[3]*(m[8]*(m[10]*m[2]-m[3]*m[9])+m[1]*(m[13]*m[9]-m[10]*m[11])-(m[13]*m[2]-m[11]*m[3])*m[6])+m[4]*(m[7]*(m[10]*m[2]-m[3]*m[9])+m[1]*(m[12]*m[9]-m[10]*m[10])-(m[12]*m[2]-m[10]*m[3])*m[6])-m[0]*(-m[7]*(m[13]*m[9]-m[10]*m[11])+m[8]*(m[12]*m[9]-m[10]*m[10])+(m[10]*m[13]-m[11]*m[12])*m[6])+m[2]*((m[12]*m[2]-m[10]*m[3])*m[8]-(m[13]*m[2]-m[11]*m[3])*m[7]+m[1]*(m[10]*m[13]-m[11]*m[12]));
  c[9] =   m[3]*(m[8]*(m[3]*m[8]-m[4]*m[7])+m[1]*(m[14]*m[7]-m[13]*m[8])-(m[14]*m[3]-m[13]*m[4])*m[5])-m[4]*(m[7]*(m[3]*m[8]-m[4]*m[7])+m[1]*(m[13]*m[7]-m[12]*m[8])-(m[13]*m[3]-m[12]*m[4])*m[5])+m[0]*(-m[7]*(m[14]*m[7]-m[13]*m[8])+m[8]*(m[13]*m[7]-m[12]*m[8])+(m[12]*m[14]-m[13]*m[13])*m[5])-m[1]*((m[13]*m[3]-m[12]*m[4])*m[8]-(m[14]*m[3]-m[13]*m[4])*m[7]+m[1]*(m[12]*m[14]-m[13]*m[13]));
  c[10] =   -m[3]*(m[8]*(m[2]*m[8]-m[4]*m[6])+m[1]*(m[14]*m[6]-m[11]*m[8])-(m[14]*m[2]-m[11]*m[4])*m[5])+m[4]*(m[7]*(m[2]*m[8]-m[4]*m[6])+m[1]*(m[13]*m[6]-m[10]*m[8])-(m[13]*m[2]-m[10]*m[4])*m[5])-m[0]*(-m[7]*(m[14]*m[6]-m[11]*m[8])+m[8]*(m[13]*m[6]-m[10]*m[8])+(m[10]*m[14]-m[11]*m[13])*m[5])+m[1]*((m[13]*m[2]-m[10]*m[4])*m[8]-(m[14]*m[2]-m[11]*m[4])*m[7]+m[1]*(m[10]*m[14]-m[11]*m[13]));
  c[11] =   m[3]*((m[2]*m[7]-m[3]*m[6])*m[8]+m[1]*(m[13]*m[6]-m[11]*m[7])-(m[13]*m[2]-m[11]*m[3])*m[5])+m[0]*((m[12]*m[6]-m[10]*m[7])*m[8]-m[7]*(m[13]*m[6]-m[11]*m[7])+(m[10]*m[13]-m[11]*m[12])*m[5])-m[1]*((m[12]*m[2]-m[10]*m[3])*m[8]-(m[13]*m[2]-m[11]*m[3])*m[7]+m[1]*(m[10]*m[13]-m[11]*m[12]))-m[4]*(m[7]*(m[2]*m[7]-m[3]*m[6])+m[1]*(m[12]*m[6]-m[10]*m[7])-(m[12]*m[2]-m[10]*m[3])*m[5]);
  c[12] =   m[0]*(m[8]*(m[11]*m[6]-m[8]*m[9])+m[5]*(m[14]*m[9]-m[11]*m[11])-m[6]*(m[14]*m[6]-m[11]*m[8]))-m[4]*(m[1]*(m[11]*m[6]-m[8]*m[9])-m[5]*(m[11]*m[2]-m[4]*m[9])+m[6]*(m[2]*m[8]-m[4]*m[6]))-m[1]*(m[8]*(m[11]*m[2]-m[4]*m[9])+m[1]*(m[14]*m[9]-m[11]*m[11])-(m[14]*m[2]-m[11]*m[4])*m[6])+m[2]*(m[8]*(m[2]*m[8]-m[4]*m[6])+m[1]*(m[14]*m[6]-m[11]*m[8])-(m[14]*m[2]-m[11]*m[4])*m[5]);
  c[13] =   -m[0]*(m[8]*(m[10]*m[6]-m[7]*m[9])+m[5]*(m[13]*m[9]-m[10]*m[11])-m[6]*(m[13]*m[6]-m[11]*m[7]))+m[4]*(m[1]*(m[10]*m[6]-m[7]*m[9])-m[5]*(m[10]*m[2]-m[3]*m[9])+m[6]*(m[2]*m[7]-m[3]*m[6]))+m[1]*(m[8]*(m[10]*m[2]-m[3]*m[9])+m[1]*(m[13]*m[9]-m[10]*m[11])-(m[13]*m[2]-m[11]*m[3])*m[6])-m[2]*((m[2]*m[7]-m[3]*m[6])*m[8]+m[1]*(m[13]*m[6]-m[11]*m[7])-(m[13]*m[2]-m[11]*m[3])*m[5]);
  c[14] =   m[0]*(m[7]*(m[10]*m[6]-m[7]*m[9])+m[5]*(m[12]*m[9]-m[10]*m[10])-m[6]*(m[12]*m[6]-m[10]*m[7]))-m[3]*(m[1]*(m[10]*m[6]-m[7]*m[9])-m[5]*(m[10]*m[2]-m[3]*m[9])+m[6]*(m[2]*m[7]-m[3]*m[6]))-m[1]*(m[7]*(m[10]*m[2]-m[3]*m[9])+m[1]*(m[12]*m[9]-m[10]*m[10])-(m[12]*m[2]-m[10]*m[3])*m[6])+m[2]*(m[7]*(m[2]*m[7]-m[3]*m[6])+m[1]*(m[12]*m[6]-m[10]*m[7])-(m[12]*m[2]-m[10]*m[3])*m[5]);

  return m[0]*c[0] + m[1]*c[1] + m[2]*c[2] + m[3]*c[3] + m[4]*c[4];
}

/*
The upper-triangular and lower-triangular routines have a similar API to the
symmetric ones. Note that as with symmetric matrices, we don't store redundant
data, and we store data row-first. So the upper-triangular matrices have N
elements in the first row in memory, but lower-triangular matrices have only 1.

Inverses of triangular matrices are similarly triangular, and that's how I store
them


Session:

// upper triangular
(%i2) ut2 : matrix([m0,m1],[0,m2]);

(%o2) matrix([m0,m1],[0,m2])
(%i3) ut3 : matrix([m0,m1,m2],[0,m3,m4],[0,0,m5]);

(%o3) matrix([m0,m1,m2],[0,m3,m4],[0,0,m5])
(%i4) ut4 : matrix([m0,m1,m2,m3],[0,m4,m5,m6],[0,0,m7,m8],[0,0,0,m9]);

(%o4) matrix([m0,m1,m2,m3],[0,m4,m5,m6],[0,0,m7,m8],[0,0,0,m9])
(%i5) ut5 : matrix([m0,m1,m2,m3,m4],[0,m5,m6,m7,m8],[0,0,m9,m10,m11],[0,0,0,m12,m13],[0,0,0,0,m14]);

(%o5) matrix([m0,m1,m2,m3,m4],[0,m5,m6,m7,m8],[0,0,m9,m10,m11],
             [0,0,0,m12,m13],[0,0,0,0,m14])

(%i11) num( ev(invert(ut2),detout) );

(%o11) matrix([m2,-m1],[0,m0])
(%i12) num( ev(invert(ut3),detout) );

(%o12) matrix([m3*m5,-m1*m5,m1*m4-m2*m3],[0,m0*m5,-m0*m4],[0,0,m0*m3])
(%i13) num( ev(invert(ut4),detout) );

(%o13) matrix([m4*m7*m9,-m1*m7*m9,m1*m5*m9-m2*m4*m9,
               (-m1*(m5*m8-m6*m7))+m2*m4*m8-m3*m4*m7],
              [0,m0*m7*m9,-m0*m5*m9,m0*(m5*m8-m6*m7)],
              [0,0,m0*m4*m9,-m0*m4*m8],[0,0,0,m0*m4*m7])
(%i14) num( ev(invert(ut5),detout) );

(%o14) matrix([m12*m14*m5*m9,-m1*m12*m14*m9,m1*m12*m14*m6-m12*m14*m2*m5,
               (-m1*(m10*m14*m6-m14*m7*m9))-m14*m3*m5*m9+m10*m14*m2*m5,
               m1*(m12*m8*m9-m13*m7*m9+(m10*m13-m11*m12)*m6)
                -m12*m4*m5*m9+m13*m3*m5*m9-(m10*m13-m11*m12)*m2*m5],
              [0,m0*m12*m14*m9,-m0*m12*m14*m6,m0*(m10*m14*m6-m14*m7*m9),
               -m0*(m12*m8*m9-m13*m7*m9+(m10*m13-m11*m12)*m6)],
              [0,0,m0*m12*m14*m5,-m0*m10*m14*m5,m0*(m10*m13-m11*m12)*m5],
              [0,0,0,m0*m14*m5*m9,-m0*m13*m5*m9],[0,0,0,0,m0*m12*m5*m9])


// lower-triangular
(%i19) lt2 : matrix([m0,0],[m1,m2]);

(%o19) matrix([m0,0],[m1,m2])
(%i20) lt3 : matrix([m0,0,0],[m1,m2,0],[m3,m4,m5]);

(%o20) matrix([m0,0,0],[m1,m2,0],[m3,m4,m5])
(%i21) lt4 : matrix([m0,0,0,0],[m1,m2,0,0],[m3,m4,m5,0],[m6,m7,m8,m9]);

(%o21) matrix([m0,0,0,0],[m1,m2,0,0],[m3,m4,m5,0],[m6,m7,m8,m9])
(%i22) lt5 : matrix([m0,0,0,0,0],[m1,m2,0,0,0],[m3,m4,m5,0,0],[m6,m7,m8,m9,0],[m10,m11,m12,m13,m14]);

(%o22) matrix([m0,0,0,0,0],[m1,m2,0,0,0],[m3,m4,m5,0,0],[m6,m7,m8,m9,0],
              [m10,m11,m12,m13,m14])
(%i23) num( ev(invert(lt2),detout) );

(%o23) matrix([m2,0],[-m1,m0])
(%i24) num( ev(invert(lt3),detout) );

(%o24) matrix([m2*m5,0,0],[-m1*m5,m0*m5,0],[m1*m4-m2*m3,-m0*m4,m0*m2])
(%i25) num( ev(invert(lt4),detout) );

(%o25) matrix([m2*m5*m9,0,0,0],[-m1*m5*m9,m0*m5*m9,0,0],
              [m1*m4*m9-m2*m3*m9,-m0*m4*m9,m0*m2*m9,0],
              [m2*(m3*m8-m5*m6)-m1*(m4*m8-m5*m7),m0*(m4*m8-m5*m7),-m0*m2*m8,
               m0*m2*m5])
(%i26) num( ev(invert(lt5),detout) );

(%o26) matrix([m14*m2*m5*m9,0,0,0,0],[-m1*m14*m5*m9,m0*m14*m5*m9,0,0,0],
              [m1*m14*m4*m9-m14*m2*m3*m9,-m0*m14*m4*m9,m0*m14*m2*m9,0,0],
              [m2*(m14*m3*m8-m14*m5*m6)-m1*(m14*m4*m8-m14*m5*m7),
               m0*(m14*m4*m8-m14*m5*m7),-m0*m14*m2*m8,m0*m14*m2*m5,0],
              [m1*(m4*(m13*m8-m12*m9)-m5*(m13*m7-m11*m9))
                -m2*(m3*(m13*m8-m12*m9)-m5*(m13*m6-m10*m9)),
               -m0*(m4*(m13*m8-m12*m9)-m5*(m13*m7-m11*m9)),
               m0*m2*(m13*m8-m12*m9),-m0*m13*m2*m5,m0*m2*m5*m9])

 */
static inline double cofactors_ut2(const double* restrict m, double* restrict c)
{
    int i=0;
    c[i++] = m[2];
    c[i++] = -m[1];
    c[i++] = m[0];
    return m[0]*m[2];
}
static inline double cofactors_ut3(const double* restrict m, double* restrict c)
{
    int i=0;
    c[i++] = m[3]*m[5];
    c[i++] = -m[1]*m[5];
    c[i++] = m[1]*m[4]-m[2]*m[3];
    c[i++] = m[0]*m[5];
    c[i++] = -m[0]*m[4];
    c[i++] = m[0]*m[3];
    return m[0]*m[3]*m[5];
}
static inline double cofactors_ut4(const double* restrict m, double* restrict c)
{
    int i=0;
    c[i++] = m[4]*m[7]*m[9];
    c[i++] = -m[1]*m[7]*m[9];
    c[i++] = m[1]*m[5]*m[9]-m[2]*m[4]*m[9];
    c[i++] = (-m[1]*(m[5]*m[8]-m[6]*m[7]))+m[2]*m[4]*m[8]-m[3]*m[4]*m[7];
    c[i++] = m[0]*m[7]*m[9];
    c[i++] = -m[0]*m[5]*m[9];
    c[i++] = m[0]*(m[5]*m[8]-m[6]*m[7]);
    c[i++] = m[0]*m[4]*m[9];
    c[i++] = -m[0]*m[4]*m[8];
    c[i++] = m[0]*m[4]*m[7];
    return m[0]*m[4]*m[7]*m[9];
}
static inline double cofactors_ut5(const double* restrict m, double* restrict c)
{
    int i=0;
    c[i++] = m[12]*m[14]*m[5]*m[9];
    c[i++] = -m[1]*m[12]*m[14]*m[9];
    c[i++] = m[1]*m[12]*m[14]*m[6]-m[12]*m[14]*m[2]*m[5];
    c[i++] = (-m[1]*(m[10]*m[14]*m[6]-m[14]*m[7]*m[9]))-m[14]*m[3]*m[5]*m[9]+m[10]*m[14]*m[2]*m[5];
    c[i++] = m[1]*(m[12]*m[8]*m[9]-m[13]*m[7]*m[9]+(m[10]*m[13]-m[11]*m[12])*m[6])-m[12]*m[4]*m[5]*m[9]+m[13]*m[3]*m[5]*m[9]-(m[10]*m[13]-m[11]*m[12])*m[2]*m[5];
    c[i++] = m[0]*m[12]*m[14]*m[9];
    c[i++] = -m[0]*m[12]*m[14]*m[6];
    c[i++] = m[0]*(m[10]*m[14]*m[6]-m[14]*m[7]*m[9]);
    c[i++] = -m[0]*(m[12]*m[8]*m[9]-m[13]*m[7]*m[9]+(m[10]*m[13]-m[11]*m[12])*m[6]);
    c[i++] = m[0]*m[12]*m[14]*m[5];
    c[i++] = -m[0]*m[10]*m[14]*m[5];
    c[i++] = m[0]*(m[10]*m[13]-m[11]*m[12])*m[5];
    c[i++] = m[0]*m[14]*m[5]*m[9];
    c[i++] = -m[0]*m[13]*m[5]*m[9];
    c[i++] = m[0]*m[12]*m[5]*m[9];
    return m[0]*m[5]*m[9]*m[12]*m[14];
}
static inline double cofactors_lt2(const double* restrict m, double* restrict c)
{
    int i=0;
    c[i++] = m[2];
    c[i++] = -m[1];
    c[i++] = m[0];
    return m[0]*m[2];
}
static inline double cofactors_lt3(const double* restrict m, double* restrict c)
{
    int i=0;
    c[i++] = m[2]*m[5];
    c[i++] = -m[1]*m[5];
    c[i++] = m[0]*m[5];
    c[i++] = m[1]*m[4]-m[2]*m[3];
    c[i++] = -m[0]*m[4];
    c[i++] = m[0]*m[2];
    return m[0]*m[2]*m[5];
}
static inline double cofactors_lt4(const double* restrict m, double* restrict c)
{
    int i=0;
    c[i++] = m[2]*m[5]*m[9];
    c[i++] = -m[1]*m[5]*m[9];
    c[i++] = m[0]*m[5]*m[9];
    c[i++] = m[1]*m[4]*m[9]-m[2]*m[3]*m[9];
    c[i++] = -m[0]*m[4]*m[9];
    c[i++] = m[0]*m[2]*m[9];
    c[i++] = m[2]*(m[3]*m[8]-m[5]*m[6])-m[1]*(m[4]*m[8]-m[5]*m[7]);
    c[i++] = m[0]*(m[4]*m[8]-m[5]*m[7]);
    c[i++] = -m[0]*m[2]*m[8];
    c[i++] = m[0]*m[2]*m[5];
    return m[0]*m[2]*m[5]*m[9];
}
static inline double cofactors_lt5(const double* restrict m, double* restrict c)
{
    int i=0;
    c[i++] = m[14]*m[2]*m[5]*m[9];
    c[i++] = -m[1]*m[14]*m[5]*m[9];
    c[i++] = m[0]*m[14]*m[5]*m[9];
    c[i++] = m[1]*m[14]*m[4]*m[9]-m[14]*m[2]*m[3]*m[9];
    c[i++] = -m[0]*m[14]*m[4]*m[9];
    c[i++] = m[0]*m[14]*m[2]*m[9];
    c[i++] = m[2]*(m[14]*m[3]*m[8]-m[14]*m[5]*m[6])-m[1]*(m[14]*m[4]*m[8]-m[14]*m[5]*m[7]);
    c[i++] = m[0]*(m[14]*m[4]*m[8]-m[14]*m[5]*m[7]);
    c[i++] = -m[0]*m[14]*m[2]*m[8];
    c[i++] = m[0]*m[14]*m[2]*m[5];
    c[i++] = m[1]*(m[4]*(m[13]*m[8]-m[12]*m[9])-m[5]*(m[13]*m[7]-m[11]*m[9]))-m[2]*(m[3]*(m[13]*m[8]-m[12]*m[9])-m[5]*(m[13]*m[6]-m[10]*m[9]));
    c[i++] = -m[0]*(m[4]*(m[13]*m[8]-m[12]*m[9])-m[5]*(m[13]*m[7]-m[11]*m[9]));
    c[i++] = m[0]*m[2]*(m[13]*m[8]-m[12]*m[9]);
    c[i++] = -m[0]*m[13]*m[2]*m[5];
    c[i++] = m[0]*m[2]*m[5]*m[9];
    return m[0]*m[2]*m[5]*m[9]*m[14];
}

/*
(%i27) a : matrix([a0,a1,a2],[0,a3,a4],[0,0,a5]);

(%o27) matrix([a0,a1,a2],[0,a3,a4],[0,0,a5])
(%i28) b : matrix([b0,b1,b2],[0,b3,b4],[0,0,b5]);

(%o28) matrix([b0,b1,b2],[0,b3,b4],[0,0,b5])
(%i29) a . b;

(%o29) matrix([a0*b0,a1*b3+a0*b1,a2*b5+a1*b4+a0*b2],[0,a3*b3,a4*b5+a3*b4],[0,0,a5*b5])
*/
static inline void mul_ut3_ut3(const double* restrict a, const double* restrict b,
                               double* restrict ab)
{
    ab[0] = a[0] * b[0];
    ab[1] = a[1] * b[3]+a[0] * b[1];
    ab[2] = a[2] * b[5]+a[1] * b[4]+a[0] * b[2];
    ab[3] = a[3] * b[3];
    ab[4] = a[4] * b[5]+a[3] * b[4];
    ab[5] = a[5] * b[5];
}

// symmetrix 3x3 by symmetrix 3x3, written into a new non-symmetric matrix,
// scaled. This is a special-case function that I needed for something...
static inline void mul_sym33_sym33_scaled_out(const double* restrict s0, const double* restrict s1, double* restrict mout, double scale)
{
// (%i106) matrix([m0_0,m0_1,m0_2],
//                [m0_1,m0_3,m0_4],
//                [m0_2,m0_4,m0_5]) .
//         matrix([m1_0,m1_1,m1_2],
//                [m1_1,m1_3,m1_4],
//                [m1_2,m1_4,m1_5]);

// (%o106) matrix([m0_2*m1_2+m0_1*m1_1+m0_0*m1_0,m0_2*m1_4+m0_1*m1_3+m0_0*m1_1,
//                 m0_2*m1_5+m0_1*m1_4+m0_0*m1_2],
//                [m0_4*m1_2+m0_3*m1_1+m0_1*m1_0,m0_4*m1_4+m0_3*m1_3+m0_1*m1_1,
//                 m0_4*m1_5+m0_3*m1_4+m0_1*m1_2],
//                [m0_5*m1_2+m0_4*m1_1+m0_2*m1_0,m0_5*m1_4+m0_4*m1_3+m0_2*m1_1,
//                 m0_5*m1_5+m0_4*m1_4+m0_2*m1_2])

  mout[0] = scale * (s0[2]*s1[2]+s0[1]*s1[1]+s0[0]*s1[0]);
  mout[1] = scale * (s0[2]*s1[4]+s0[1]*s1[3]+s0[0]*s1[1]);
  mout[2] = scale * (s0[2]*s1[5]+s0[1]*s1[4]+s0[0]*s1[2]);
  mout[3] = scale * (s0[4]*s1[2]+s0[3]*s1[1]+s0[1]*s1[0]);
  mout[4] = scale * (s0[4]*s1[4]+s0[3]*s1[3]+s0[1]*s1[1]);
  mout[5] = scale * (s0[4]*s1[5]+s0[3]*s1[4]+s0[1]*s1[2]);
  mout[6] = scale * (s0[5]*s1[2]+s0[4]*s1[1]+s0[2]*s1[0]);
  mout[7] = scale * (s0[5]*s1[4]+s0[4]*s1[3]+s0[2]*s1[1]);
  mout[8] = scale * (s0[5]*s1[5]+s0[4]*s1[4]+s0[2]*s1[2]);
}

static inline void outerproduct3(const double* restrict v, double* restrict P)
{
  P[0] = v[0]*v[0];
  P[1] = v[0]*v[1];
  P[2] = v[0]*v[2];
  P[3] = v[1]*v[1];
  P[4] = v[1]*v[2];
  P[5] = v[2]*v[2];
}

static inline void outerproduct3_scaled(const double* restrict v, double* restrict P, double scale)
{
  P[0] = scale * v[0]*v[0];
  P[1] = scale * v[0]*v[1];
  P[2] = scale * v[0]*v[2];
  P[3] = scale * v[1]*v[1];
  P[4] = scale * v[1]*v[2];
  P[5] = scale * v[2]*v[2];
}

// conjugate 2 vectors (a, b) through a symmetric matrix S: a->transpose x S x b
// (%i2) sym3 : matrix([s0,s1,s2],
//                     [s1,s3,s4],
//                     [s2,s4,s5]);

// (%o2) matrix([s0,s1,s2],[s1,s3,s4],[s2,s4,s5])
// (%i6) a : matrix([a0],[a1],[a2]);

// (%o6) matrix([a0],[a1],[a2])
// (%i7) b : matrix([b0],[b1],[b2]);

// (%o7) matrix([b0],[b1],[b2])
// (%i10) transpose(a) . sym3 . b;

// (%o10) a2*(b2*s5+b1*s4+b0*s2)+a1*(b2*s4+b1*s3+b0*s1)+a0*(b2*s2+b1*s1+b0*s0)
static inline double conj_3(const double* restrict a, const double* restrict s, const double* restrict b)
{
  return a[2]*(b[2]*s[5]+b[1]*s[4]+b[0]*s[2])+a[1]*(b[2]*s[4]+b[1]*s[3]+b[0]*s[1])+a[0]*(b[2]*s[2]+b[1]*s[1]+b[0]*s[0]);
}

// Given an orthonormal matrix, returns the det. This is always +1 or -1
static inline double det_orthonormal33(const double* m)
{
    // cross(row0,row1) = det * row3

    // I find a nice non-zero element of row3, and see if the signs match
    if( m[6] < -0.1 )
    {
        // looking at col0 of the last row. It is <0
        double cross = m[1]*m[5] - m[2]*m[4];
        return cross > 0.0 ? -1.0 : 1.0;
    }
    if( m[6] > 0.1)
    {
        // looking at col0 of the last row. It is > 0
        double cross = m[1]*m[5] - m[2]*m[4];
        return cross > 0.0 ? 1.0 : -1.0;
    }

    if( m[7] < -0.1 )
    {
        // looking at col1 of the last row. It is <0
        double cross = m[2]*m[3] - m[0]*m[5];
        return cross > 0.0 ? -1.0 : 1.0;
    }
    if( m[7] > 0.1)
    {
        // looking at col1 of the last row. It is > 0
        double cross = m[2]*m[3] - m[0]*m[5];
        return cross > 0.0 ? 1.0 : -1.0;
    }

    if( m[8] < -0.1 )
    {
        // looking at col2 of the last row. It is <0
        double cross = m[0]*m[4] - m[1]*m[3];
        return cross > 0.0 ? -1.0 : 1.0;
    }

    // last option. This MUST be true, so I don't even bother to check
    {
        // looking at col2 of the last row. It is > 0
        double cross = m[0]*m[4] - m[1]*m[3];
        return cross > 0.0 ? 1.0 : -1.0;
    }
}

static void minimath_xchg(double* m, int i, int j)
{
    double t = m[i];
    m[i] = m[j];
    m[j] = t;
}
static inline void gen33_transpose(double* m)
{
    minimath_xchg(m, 1, 3);
    minimath_xchg(m, 2, 6);
    minimath_xchg(m, 5, 7);
}

static inline void gen33_transpose_vout(const double* m, double* mout)
{
    for(int i=0; i<3; i++)
        for(int j=0; j<3; j++)
            mout[i*3+j] = m[j*3+i];
}

static inline double cofactors_gen33(// output
                                     double* restrict c,

                                     // input
                                     const double* restrict m)
{
    /*
(%i1) display2d : false;

(%o1) false
(%i5) linel : 100000;

(%o5) 100000
(%i6) mat33 : matrix( [m0,m1,m2], [m3,m4,m5], [m6,m7,m8] );

(%o6) matrix([m0,m1,m2],[m3,m4,m5],[m6,m7,m8])
(%i7) num( ev(invert(mat33)), detout );

(%o7) matrix([(m4*m8-m5*m7)/(m0*(m4*m8-m5*m7)+m1*(m5*m6-m3*m8)+m2*(m3*m7-m4*m6)),(m2*m7-m1*m8)/(m0*(m4*m8-m5*m7)+m1*(m5*m6-m3*m8)+m2*(m3*m7-m4*m6)),(m1*m5-m2*m4)/(m0*(m4*m8-m5*m7)+m1*(m5*m6-m3*m8)+m2*(m3*m7-m4*m6))],[(m5*m6-m3*m8)/(m0*(m4*m8-m5*m7)+m1*(m5*m6-m3*m8)+m2*(m3*m7-m4*m6)),(m0*m8-m2*m6)/(m0*(m4*m8-m5*m7)+m1*(m5*m6-m3*m8)+m2*(m3*m7-m4*m6)),(m2*m3-m0*m5)/(m0*(m4*m8-m5*m7)+m1*(m5*m6-m3*m8)+m2*(m3*m7-m4*m6))],[(m3*m7-m4*m6)/(m0*(m4*m8-m5*m7)+m1*(m5*m6-m3*m8)+m2*(m3*m7-m4*m6)),(m1*m6-m0*m7)/(m0*(m4*m8-m5*m7)+m1*(m5*m6-m3*m8)+m2*(m3*m7-m4*m6)),(m0*m4-m1*m3)/(m0*(m4*m8-m5*m7)+m1*(m5*m6-m3*m8)+m2*(m3*m7-m4*m6))])
(%i8) determinant(mat33);

(%o8) m0*(m4*m8-m5*m7)-m1*(m3*m8-m5*m6)+m2*(m3*m7-m4*m6)
    */

    double det = m[0]*(m[4]*m[8]-m[5]*m[7])-m[1]*(m[3]*m[8]-m[5]*m[6])+m[2]*(m[3]*m[7]-m[4]*m[6]);

    c[0] = m[4]*m[8]-m[5]*m[7];
    c[1] = m[2]*m[7]-m[1]*m[8];
    c[2] = m[1]*m[5]-m[2]*m[4];
    c[3] = m[5]*m[6]-m[3]*m[8];
    c[4] = m[0]*m[8]-m[2]*m[6];
    c[5] = m[2]*m[3]-m[0]*m[5];
    c[6] = m[3]*m[7]-m[4]*m[6];
    c[7] = m[1]*m[6]-m[0]*m[7];
    c[8] = m[0]*m[4]-m[1]*m[3];

    return det;
}

#ifdef __cplusplus
#undef restrict
#endif
