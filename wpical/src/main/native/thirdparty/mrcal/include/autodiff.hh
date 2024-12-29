// Copyright (c) 2017-2023 California Institute of Technology ("Caltech"). U.S.
// Government sponsorship acknowledged. All rights reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0

#pragma once

/*
  Automatic differentiation routines. Used in poseutils-uses-autodiff.cc. See
  that file for usage examples
 */

// Apparently I need this in MSVC to get constants
#define _USE_MATH_DEFINES
#include <math.h>
#include <string.h>
#include "strides.h"

template<int NGRAD, int NVEC> struct vec_withgrad_t;


template<int NGRAD>
struct val_withgrad_t
{
    double x;
    double j[NGRAD == 0 ? 1 : NGRAD];

    
    val_withgrad_t(double _x = 0.0) : x(_x)
    {
        for(int i=0; i<NGRAD; i++) j[i] = 0.0;
    }

    
    val_withgrad_t<NGRAD> operator+( const val_withgrad_t<NGRAD>& b ) const
    {
        val_withgrad_t<NGRAD> y = *this;
        y.x += b.x;
        for(int i=0; i<NGRAD; i++)
            y.j[i] += b.j[i];
        return y;
    }
    
    val_withgrad_t<NGRAD> operator+( double b ) const
    {
        val_withgrad_t<NGRAD> y = *this;
        y.x += b;
        return y;
    }
    
    void operator+=( const val_withgrad_t<NGRAD>& b )
    {
        *this = (*this) + b;
    }
    
    val_withgrad_t<NGRAD> operator-( const val_withgrad_t<NGRAD>& b ) const
    {
        val_withgrad_t<NGRAD> y = *this;
        y.x -= b.x;
        for(int i=0; i<NGRAD; i++)
            y.j[i] -= b.j[i];
        return y;
    }
    
    val_withgrad_t<NGRAD> operator-( double b ) const
    {
        val_withgrad_t<NGRAD> y = *this;
        y.x -= b;
        return y;
    }
    
    void operator-=( const val_withgrad_t<NGRAD>& b )
    {
        *this = (*this) - b;
    }
    
    val_withgrad_t<NGRAD> operator-() const
    {
        return (*this) * (-1);
    }
    
    val_withgrad_t<NGRAD> operator*( const val_withgrad_t<NGRAD>& b ) const
    {
        val_withgrad_t<NGRAD> y;
        y.x = x * b.x;
        for(int i=0; i<NGRAD; i++)
            y.j[i] = j[i]*b.x + x*b.j[i];
        return y;
    }
    
    val_withgrad_t<NGRAD> operator*( double b ) const
    {
        val_withgrad_t<NGRAD> y;
        y.x = x * b;
        for(int i=0; i<NGRAD; i++)
            y.j[i] = j[i]*b;
        return y;
    }
    
    void operator*=( const val_withgrad_t<NGRAD>& b )
    {
        *this = (*this) * b;
    }
    
    void operator*=( const double b )
    {
        *this = (*this) * b;
    }
    
    val_withgrad_t<NGRAD> operator/( const val_withgrad_t<NGRAD>& b ) const
    {
        val_withgrad_t<NGRAD> y;
        y.x = x / b.x;
        for(int i=0; i<NGRAD; i++)
            y.j[i] = (j[i]*b.x - x*b.j[i]) / (b.x*b.x);
        return y;
    }
    
    val_withgrad_t<NGRAD> operator/( double b ) const
    {
        return (*this) * (1./b);
    }
    
    void operator/=( const val_withgrad_t<NGRAD>& b )
    {
        *this = (*this) / b;
    }
    
    void operator/=( const double b )
    {
        *this = (*this) / b;
    }
    
    val_withgrad_t<NGRAD> sqrt(void) const
    {
        val_withgrad_t<NGRAD> y;
        y.x = ::sqrt(x);
        for(int i=0; i<NGRAD; i++)
            y.j[i] = j[i] / (2. * y.x);
        return y;
    }

    
    val_withgrad_t<NGRAD> square(void) const
    {
        val_withgrad_t<NGRAD> s;
        s.x = x*x;
        for(int i=0; i<NGRAD; i++)
            s.j[i] = 2. * x * j[i];
        return s;
    }

    
    val_withgrad_t<NGRAD> sin(void) const
    {
        const double s = ::sin(x);
        const double c = ::cos(x);
        val_withgrad_t<NGRAD> y;
        y.x = s;
        for(int i=0; i<NGRAD; i++)
            y.j[i] = c*j[i];
        return y;
    }

    
    val_withgrad_t<NGRAD> cos(void) const
    {
        const double s = ::sin(x);
        const double c = ::cos(x);
        val_withgrad_t<NGRAD> y;
        y.x = c;
        for(int i=0; i<NGRAD; i++)
            y.j[i] = -s*j[i];
        return y;
    }

    
    vec_withgrad_t<NGRAD, 2> sincos(void) const
    {
        const double s = ::sin(x);
        const double c = ::cos(x);
        vec_withgrad_t<NGRAD, 2> sc;
        sc.v[0].x = s;
        sc.v[1].x = c;
        for(int i=0; i<NGRAD; i++)
        {
            sc.v[0].j[i] =  c*j[i];
            sc.v[1].j[i] = -s*j[i];
        }
        return sc;
    }

    
    val_withgrad_t<NGRAD> tan(void) const
    {
        const double s = ::sin(x);
        const double c = ::cos(x);
        val_withgrad_t<NGRAD> y;
        y.x = s/c;
        for(int i=0; i<NGRAD; i++)
            y.j[i] = j[i] / (c*c);
        return y;
    }

    
    val_withgrad_t<NGRAD> atan2(val_withgrad_t<NGRAD>& x) const
    {
        val_withgrad_t<NGRAD> th;
        const val_withgrad_t<NGRAD>& y = *this;

        th.x = ::atan2(y.x, x.x);
        // dth/dv = d/dv atan2(y,x)
        //        = d/dv atan(y/x)
        //        = 1 / (1 + y^2/x^2) d/dv (y/x)
        //        = x^2 / (x^2 + y^2) / x^2 * (dy/dv x - y dx/dv)
        //        = 1 / (x^2 + y^2) * (dy/dv x - y dx/dv)
        double norm2 = y.x*y.x + x.x*x.x;
        for(int i=0; i<NGRAD; i++)
            th.j[i] = (y.j[i]*x.x - y.x*x.j[i]) / norm2;
        return th;
    }

    // This function does NOT check for overflow. The gradient is infinite at
    // the valid bounds of the input. So the caller has to do the right thing in
    // those cases
    
    val_withgrad_t<NGRAD> asin(void) const
    {
        val_withgrad_t<NGRAD> th;
        th.x = ::asin(x);
        double dasin_dx = 1. / ::sqrt( 1. - x*x );
        for(int i=0; i<NGRAD; i++)
            th.j[i] = dasin_dx * j[i];
        return th;
    }

    // This function does NOT check for overflow. The gradient is infinite at
    // the valid bounds of the input. So the caller has to do the right thing in
    // those cases
    
    val_withgrad_t<NGRAD> acos(void) const
    {
        val_withgrad_t<NGRAD> th;
        th.x = ::acos(x);
        double dacos_dx = -1. / ::sqrt( 1. - x*x );
        for(int i=0; i<NGRAD; i++)
            th.j[i] = dacos_dx * j[i];
        return th;
    }

    
    val_withgrad_t<NGRAD> sinx_over_x(// To avoid recomputing it
                                      const val_withgrad_t<NGRAD>& sinx) const
    {
        // For small x I need special-case logic. In the limit as x->0 I have
        // sin(x)/x -> 1. But I'm propagating gradients, so I need to capture
        // that. I have
        //
        //   d(sin(x)/x)/dx =
        //     (x cos(x) - sin(x))/x^2
        //
        // As x -> 0 this is
        //
        //     (cos(x) - x sin(x) - cos(x)) / (2x) =
        //     (- x sin(x)) / (2x) =
        //     -sin(x) / 2 =
        //     0
        //
        // So for small x the gradient is 0
        if(fabs(x) < 1e-5)
            return val_withgrad_t<NGRAD>(1.0);

        return sinx / (*this);
    }
};


template<int NGRAD, int NVEC>
struct vec_withgrad_t
{
    val_withgrad_t<NGRAD> v[NVEC == 0 ? 1 : NVEC];

    vec_withgrad_t() {}

    
    void init_vars(const double* x_in, int ivar0, int Nvars, int i_gradvec0 = -1,
                   int stride = sizeof(double))
    {
        // Initializes vector entries ivar0..ivar0+Nvars-1 inclusive using the
        // data in x_in[]. x_in[0] corresponds to vector entry ivar0. If
        // i_gradvec0 >= 0 then vector ivar0 corresponds to gradient index
        // i_gradvec0, with all subsequent entries being filled-in
        // consecutively. It's very possible that NGRAD > Nvars. Initially the
        // subset of the gradient array corresponding to variables
        // i_gradvec0..i_gradvec0+Nvars-1 is an identity, with the rest being 0
        memset((char*)&v[ivar0], 0, Nvars*sizeof(v[0]));
        for(int i=ivar0; i<ivar0+Nvars; i++)
        {
            v[i].x = _P1(x_in,stride,  i-ivar0);
            if(i_gradvec0 >= 0)
                v[i].j[i_gradvec0+i-ivar0] = 1.0;
        }
    }

    
    vec_withgrad_t(const double* x_in, int i_gradvec0 = -1,
                   int stride = sizeof(double))
    {
        init_vars(x_in, 0, NVEC, i_gradvec0, stride);
    }

    
    val_withgrad_t<NGRAD>& operator[](int i)
    {
        return v[i];
    }

    
    const val_withgrad_t<NGRAD>& operator[](int i) const
    {
        return v[i];
    }

    
    void operator+=( const vec_withgrad_t<NGRAD,NVEC>& x )
    {
        (*this) = (*this) + x;
    }
    
    vec_withgrad_t<NGRAD,NVEC> operator+( const vec_withgrad_t<NGRAD,NVEC>& x ) const
    {
        vec_withgrad_t<NGRAD,NVEC> p;
        for(int i=0; i<NVEC; i++)
            p[i] = v[i] + x.v[i];
        return p;
    }

    
    void operator+=( const val_withgrad_t<NGRAD>& x )
    {
        (*this) = (*this) + x;
    }
    
    vec_withgrad_t<NGRAD,NVEC> operator+( const val_withgrad_t<NGRAD>& x ) const
    {
        vec_withgrad_t<NGRAD,NVEC> p;
        for(int i=0; i<NVEC; i++)
            p[i] = v[i] + x;
        return p;
    }

    
    void operator+=( double x )
    {
        (*this) = (*this) + x;
    }
    
    vec_withgrad_t<NGRAD,NVEC> operator+( double x ) const
    {
        vec_withgrad_t<NGRAD,NVEC> p;
        for(int i=0; i<NVEC; i++)
            p[i] = v[i] + x;
        return p;
    }

    
    void operator-=( const vec_withgrad_t<NGRAD,NVEC>& x )
    {
        (*this) = (*this) - x;
    }
    
    vec_withgrad_t<NGRAD,NVEC> operator-( const vec_withgrad_t<NGRAD,NVEC>& x ) const
    {
        vec_withgrad_t<NGRAD,NVEC> p;
        for(int i=0; i<NVEC; i++)
            p[i] = v[i] - x.v[i];
        return p;
    }

    
    void operator-=( const val_withgrad_t<NGRAD>& x )
    {
        (*this) = (*this) - x;
    }
    
    vec_withgrad_t<NGRAD,NVEC> operator-( const val_withgrad_t<NGRAD>& x ) const
    {
        vec_withgrad_t<NGRAD,NVEC> p;
        for(int i=0; i<NVEC; i++)
            p[i] = v[i] - x;
        return p;
    }

    
    void operator-=( double x )
    {
        (*this) = (*this) - x;
    }
    
    vec_withgrad_t<NGRAD,NVEC> operator-( double x ) const
    {
        vec_withgrad_t<NGRAD,NVEC> p;
        for(int i=0; i<NVEC; i++)
            p[i] = v[i] - x;
        return p;
    }

    
    void operator*=( const vec_withgrad_t<NGRAD,NVEC>& x )
    {
        (*this) = (*this) * x;
    }
    
    vec_withgrad_t<NGRAD,NVEC> operator*( const vec_withgrad_t<NGRAD,NVEC>& x ) const
    {
        vec_withgrad_t<NGRAD,NVEC> p;
        for(int i=0; i<NVEC; i++)
            p[i] = v[i] * x.v[i];
        return p;
    }

    
    void operator*=( const val_withgrad_t<NGRAD>& x )
    {
        (*this) = (*this) * x;
    }
    
    vec_withgrad_t<NGRAD,NVEC> operator*( const val_withgrad_t<NGRAD>& x ) const
    {
        vec_withgrad_t<NGRAD,NVEC> p;
        for(int i=0; i<NVEC; i++)
            p[i] = v[i] * x;
        return p;
    }

    
    void operator*=( double x )
    {
        (*this) = (*this) * x;
    }
    
    vec_withgrad_t<NGRAD,NVEC> operator*( double x ) const
    {
        vec_withgrad_t<NGRAD,NVEC> p;
        for(int i=0; i<NVEC; i++)
            p[i] = v[i] * x;
        return p;
    }

    
    void operator/=( const vec_withgrad_t<NGRAD,NVEC>& x )
    {
        (*this) = (*this) / x;
    }
    
    vec_withgrad_t<NGRAD,NVEC> operator/( const vec_withgrad_t<NGRAD,NVEC>& x ) const
    {
        vec_withgrad_t<NGRAD,NVEC> p;
        for(int i=0; i<NVEC; i++)
            p[i] = v[i] / x.v[i];
        return p;
    }

    
    void operator/=( const val_withgrad_t<NGRAD>& x )
    {
        (*this) = (*this) / x;
    }
    
    vec_withgrad_t<NGRAD,NVEC> operator/( const val_withgrad_t<NGRAD>& x ) const
    {
        vec_withgrad_t<NGRAD,NVEC> p;
        for(int i=0; i<NVEC; i++)
            p[i] = v[i] / x;
        return p;
    }

    
    void operator/=( double x )
    {
        (*this) = (*this) / x;
    }
    
    vec_withgrad_t<NGRAD,NVEC> operator/( double x ) const
    {
        vec_withgrad_t<NGRAD,NVEC> p;
        for(int i=0; i<NVEC; i++)
            p[i] = v[i] / x;
        return p;
    }

    
    val_withgrad_t<NGRAD> dot( const vec_withgrad_t<NGRAD,NVEC>& x) const
    {
        val_withgrad_t<NGRAD> d; // initializes to 0
        for(int i=0; i<NVEC; i++)
        {
            val_withgrad_t<NGRAD> e = x.v[i]*v[i];
            d += e;
        }
        return d;
    }

    
    vec_withgrad_t<NGRAD,NVEC> cross( const vec_withgrad_t<NGRAD,NVEC>& x) const
    {
        vec_withgrad_t<NGRAD,NVEC> c;
        c[0] = v[1]*x.v[2] - v[2]*x.v[1];
        c[1] = v[2]*x.v[0] - v[0]*x.v[2];
        c[2] = v[0]*x.v[1] - v[1]*x.v[0];
        return c;
    }

    
    val_withgrad_t<NGRAD> norm2(void) const
    {
        return dot(*this);
    }

    
    val_withgrad_t<NGRAD> mag(void) const
    {
        val_withgrad_t<NGRAD> l2 = norm2();
        return l2.sqrt();
    }

    
    void extract_value(double* out,
                       int stride = sizeof(double),
                       int ivar0 = 0, int Nvars = NVEC) const
    {
        for(int i=ivar0; i<ivar0+Nvars; i++)
            _P1(out,stride, i-ivar0) = v[i].x;
    }
    
    void extract_grad(double* J,
                      int i_gradvec0, int N_gradout,
                      int ivar0,
                      int J_stride0, int J_stride1,
                      int Nvars = NVEC) const
    {
        for(int i=ivar0; i<ivar0+Nvars; i++)
            for(int j=0; j<N_gradout; j++)
                _P2(J,J_stride0,J_stride1, i-ivar0,j) = v[i].j[i_gradvec0+j];
    }
};

template<int NGRAD>
static
vec_withgrad_t<NGRAD, 3>
cross( const vec_withgrad_t<NGRAD, 3>& a,
       const vec_withgrad_t<NGRAD, 3>& b )
{
    vec_withgrad_t<NGRAD, 3> c;
    c.v[0] = a.v[1]*b.v[2] - a.v[2]*b.v[1];
    c.v[1] = a.v[2]*b.v[0] - a.v[0]*b.v[2];
    c.v[2] = a.v[0]*b.v[1] - a.v[1]*b.v[0];
    return c;
}

template<int NGRAD>
static
val_withgrad_t<NGRAD>
cross_norm2( const vec_withgrad_t<NGRAD, 3>& a,
             const vec_withgrad_t<NGRAD, 3>& b )
{
    vec_withgrad_t<NGRAD, 3> c = cross(a,b);
    return c.norm2();
}

template<int NGRAD>
static
val_withgrad_t<NGRAD>
cross_mag( const vec_withgrad_t<NGRAD, 3>& a,
           const vec_withgrad_t<NGRAD, 3>& b )
{
    vec_withgrad_t<NGRAD, 3> c = cross(a,b);
    return c.mag();
}
