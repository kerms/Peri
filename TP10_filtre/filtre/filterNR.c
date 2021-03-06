/* ------------------ */
/* --- filterNR.c --- */
/* ------------------ */

/*
 * Copyright (c) 2011-2015 Lionel Lacassagne, all rights reserved,  LRI,  University Paris Sud 11
 * Copyright (c) 2015-2016 Lionel Lacassagne, all rights reserved,  LIP6, UPMC
 */

#include <stdio.h>
#include <math.h>
#include <stdlib.h>

#include "def.h"
#include "nrutil.h"
//#include "util.h"
//#include "kernel.h"

//#include "all.h"
//#include "mem.h"
//#include "outils.h"

#include "gauss.h"
#include "filterNR.h"

// -----------------------------------
float32* alloc_f32kernel1D(int radius)
// -----------------------------------
{
    float32 *K;
    K = f32vector(-radius, radius);
    return K;
}
// ------------------------------------
float32** alloc_f32kernel2D(int radius)
// ------------------------------------
{
    float32 **K;
    K = f32matrix(-radius, radius, -radius, radius);
    return K;
}
// ------------------------------------------
void free_f32kernel1D(float32 *K, int radius)
// ------------------------------------------
{
    free_f32vector(K, -radius, radius);
}
// -------------------------------------------
void free_f32kernel2D(float32 **K, int radius)
// -------------------------------------------
{
    free_f32matrix(K, -radius, radius, -radius, radius);
}
// -----------------------------------------------------------------------
void display_f32kernel1D(float32 *K, int radius, char *format, char *name)
// -----------------------------------------------------------------------
{
    display_f32vector(K, -radius, +radius, format, name);
}
// ------------------------------------------------------------------------
void display_f32kernel2D(float32 **K, int radius, char *format, char *name)
// ------------------------------------------------------------------------
{
    display_f32matrix(K, -radius, +radius, -radius, +radius, format, name);
}
// ------------------------------------------------------------------
void init_gaussian_f32kernel1D(float32 *K, int radius, float32 sigma)
// ------------------------------------------------------------------
{
    int i;
    float32 x, sx;
    
    sx = 0.0f;
    for(i = -radius; i <= radius; i++) {
        x = gauss1D(sigma, i);
        K[i] = x;
        sx += x; // sommation pour normalisation
    }
    
    for(i=-radius; i<=radius; i++) {
        K[i] /= sx; // normalisation
    }
}
// -------------------------------------------------------------------
void init_gaussian_f32kernel2D(float32 **K, int radius, float32 sigma)
// -------------------------------------------------------------------
{
    int i, j;
    float32 x, sx;
    
    sx = 0.0f;
    for(i=-radius; i<=radius; i++) {
        for(j=-radius; j<=radius; j++) {
            x = gauss2D(sigma, i, j);
            K[i][j] = x;
            sx += x; // sommation pour normalisation
        }
    }
    for(i=-radius; i<=radius; i++) {
        for(j=-radius; j<=radius; j++) {
            K[i][j] /= sx;  // normalisation
        }
    }
}
// --------------------------------------------------
void init_average_f32kernel1D(float32 *K, int radius)
// --------------------------------------------------
{
    int i;
    float32 x, sx = (2*radius+1);
    
    for(i=-radius; i<=radius; i++) {
        x = 1.0f / sx;
        K[i] = x;
    }
}
// ---------------------------------------------------
void init_average_f32kernel2D(float32 **K, int radius)
// ---------------------------------------------------
{
    int i, j;
    float32 x, sx = (2*radius+1)*(2*radius+1); // la somme est connue
    
    for(i=-radius; i<=radius; i++) {
        for(j=-radius; j<=radius; j++) {
            x = 1.0f / sx;
            K[i][j] = x;
        }
    }
}
// --------------------------------------------------------------------------------------------
void convolve_f32kernel1D_ui8vector(uint8 *X, int j0, int j1, float32 *K, int radius, uint8 *Y)
// --------------------------------------------------------------------------------------------
{
    // fait l'hypothese qu'il y a des bords alloues
    int j;
    int jj;
    
    float32 y;
    
    for(j=j0; j<=j1; j++) {
        
        // convolution 1D
        y = 0.0f;
        for(jj=-radius; jj<=radius; jj++) {
            y += X[j+jj] * K[jj];
        }
        
        // clamping [0..255]
        if(y < 0.0f) y = 0.0f;
        if(y > 255.0f) y = 255.0f;
        
        Y[j] = (uint8) lroundf(y);
    }
}
// ---------------------------------------------------------------------------------------------------------------
void convolve_f32kernel2D_ui8matrix(uint8 **X, int i0, int i1, int j0, int j1, float32 **K, int radius, uint8 **Y)
// ---------------------------------------------------------------------------------------------------------------
{
    int i, j;
    int ii, jj;
    
    uint8 **T;
    float32 y; 
    
    // tableau temporaire avec bords
    T = ui8matrix(i0-radius, i1+radius, j0-radius, j1+radius); 
    zero_ui8matrix(T, i0-radius, i1+radius, j0-radius, j1+radius);
    dup_ui8matrix(X, i0, i1, j0, j1, T);
    extend_ui8matrix(T, i0, i1, j0, j1, radius);
    
    for(i=i0; i<=i1; i++) {
        for(j=j0; j<=j1; j++) {
            
            // convolution 2D
            y = 0.0f;
            for(ii=-radius; ii<=radius; ii++) {
                for(jj=-radius; jj<=radius; jj++) {
                    y += T[i+ii][j+jj] * K[ii][jj];
                }
            }
            
            // clamipng
            if(y < 0.0f) y = 0.0f;
            if(y > 255.0f) y = 255.0f;

            Y[i][j] = (uint8) y;
        }
    }
    free_ui8matrix(T, i0-radius, i1+radius, j0-radius, j1+radius); 
}
/* ------------------------------------------------- */
void sort_selection_ui8vector(uint8 *X, int i0, int i1)
/* ------------------------------------------------- */
{
}
/* --------------------------------------------------------------------------------- */
void median_ui8matrix(uint8 **X, int i0, int i1, int j0, int j1, int radius, uint8 **Y)
/* --------------------------------------------------------------------------------- */
{    
}
// --------------------------------------------------------
void fir_average_f32(uint8 *X, int n, int radius, uint8 *Y)
// --------------------------------------------------------
{
    int i, di;
    int d; // diametre du filtre
    float32 s, y;
    uint8 *T; // tableau temporaire avec bords
    
    d = 2*radius+1;
    
    // allocation et copie dans un tableau temporaire avec bords
    T = ui8vector(0-radius, n-1+radius);
    dup_ui8vector(X, 0, n-1, T);
    extend_ui8vector(T, 0, n-1, radius); // duplication des bords
    
    for(i = 0; i < n; i++) {
        
        s = 0.0f;
        for(di = -radius; di <= radius; di++) {
            s += T[i+di];
        }
        y = s / d;
        Y[i] = (uint8) lroundf(y);
    }
    
    free_ui8vector(T, 0-radius, n-1+radius);
}
// --------------------------------------------------------
void fir_average_i16(uint8 *X, int n, int radius, uint8 *Y)
// --------------------------------------------------------
{
    // normalisation par division
    
    int samples, iter_sample;
    uint8 *T; // tableau temporaire avec bords
    uint16 sum = 0;
    uint16 div = 2*radius +1;
    uint16 res;
    
    // allocation et copie dans un tableau temporaire avec bords
    T = ui8vector(0-radius, n-1+radius);
    dup_ui8vector(X, 0, n-1, T);
    extend_ui8vector(T, 0, n-1, radius); // duplication des bords

    zero_ui8vector(Y, 0, n-1); // ligne de code a supprimer
    // CODE A COMPLETER ICI

    for (samples = 0; samples < n; ++samples)
    {
        for (iter_sample = -radius; 
             iter_sample < radius+1; 
             ++iter_sample)
        {
            sum += T[samples + iter_sample];
        }

        /* round mode */
        sum = (sum + div/2 )/ div;
        Y[samples] = (uint8) sum;
        sum = 0;
    }
    
    free_ui8vector(T, 0-radius, n-1+radius);
}
// ---------------------------------------------------------------
void fir_average_q16(uint8 *X, int n, int radius, int q, uint8 *Y)
// ---------------------------------------------------------------
{
    // normalisation par fraction equivalente
    uint8 *T; // tableau temporaire avec bords
    int samples, iter_sample;
    uint16 sum = 0;
    uint16 div = 2*radius +1;
    uint16 res;
    uint16 pow_value = 1 << q;
    uint16 mult = pow_value / div;

    // allocation et copie dans un tableau temporaire avec bords
    T = ui8vector(0-radius, n-1+radius);
    dup_ui8vector(X, 0, n-1, T);
    extend_ui8vector(T, 0, n-1, radius); // duplication des bords
    
    zero_ui8vector(Y, 0, n-1); // ligne de code a supprimer
    // CODE A COMPLETER ICI
    for (samples = 0; samples < n; ++samples)
    {
        for (iter_sample = -radius; 
             iter_sample < radius+1; 
             ++iter_sample)
        {
            sum += T[samples + iter_sample];
        }

        /* round mode */
        sum = (mult*sum  +  (pow_value/2) )/ pow_value;
        Y[samples] = (uint8) sum;
        sum = 0;
    }

    
    free_ui8vector(T, 0-radius, n-1+radius);
}
// ---------------------------------------------------------
void fir_gauss_f32(uint8 *X, int n, float32 sigma, uint8 *Y)
// ---------------------------------------------------------
{
    int radius;
    uint8   *T; // tableau temporaire avec bords
    float32 *K; // kernel

    // allocation du kernel
    //printf("[gauss] sigma = %.1f\n", sigma);
    radius = (int) ceilf(2.0f * sigma) + 1; // relation entre rayon et sigma
    //printf("[gauss] radius = %d\n", radius);
    K = alloc_f32kernel1D(radius);
    init_gaussian_f32kernel1D(K, radius, sigma);
    //display_f32kernel1D(K, radius, "%6.3f", "Gauss kernel");
    
    // allocation et copie dans un tableau temporaire avec bords
    T = ui8vector(0-radius, n-1+radius);
    dup_ui8vector(X, 0, n-1, T);
    extend_ui8vector(T, 0, n-1, radius); // duplication des bords
    
    convolve_f32kernel1D_ui8vector(T, 0, n-1, K, radius, Y);
    
    free_ui8vector(T, 0-radius, n-1+radius);
}
// ---------------------------------------------------
void iir_f32(uint8 *X, int n, float32 alpha, uint8 *Y)
// ---------------------------------------------------
{
    int i;
    int radius = 2;
    
    float32 x0, y0, y1, y2;
    
    float32 gamma;
    float32  b0, a1, a2;
    
    uint8 *T; // tableau temporaire avec bords
    
    gamma = exp(-alpha);
    
    b0 = (1.0f - gamma) * (1.0f - gamma);
    a1 = 2.0f * gamma;
    a2 = - gamma * gamma;
    
    y1 = y2 = X[0];
    
    for(i = 0; i < n; i++) {
        
        x0 = X[i];
        y0 = b0 * x0 + a1 * y1 + a2 * y2;
        
        if(y0 <   0.0f) y0 =   0.0f;
        if(y0 > 255.0f) y0 = 255.0f;
        
        Y[i] = (uint8) lroundf(y0);
        
        y2 = y1; y1 = y0;
    }
}
// ----------------------------------------------------------
void iir_q16(uint8 *X, int n, float32 alpha, int q, uint8 *Y)
// ----------------------------------------------------------
{
    //zero_ui8vector(Y, 0, n-1); // ligne de code a supprimer
    
    int i;
    int radius = 2;
    
    sint16 x0, y0, y1, y2;
    sint16 Q = 1 << q;
    sint16 B0, A1, A2;

    /* float32 allowed because not in loop */
    float32 gamma;
    float32 b0, a1, a2;
    
    

    gamma = exp(-alpha);
    
    b0 = (1.0f - gamma) * (1.0f - gamma);
    a1 = 2.0f * gamma;
    a2 = - gamma * gamma;
    
    B0 = b0 * Q;
    A1 = a1 * Q;
    A2 = a2 * Q;

    /* need to be 256 */
    if (B0 + A1 + A2 == 255) {
        A2 += 1;
    }
    printf("    sum of coeff A1 + A2 + B0 : %d\n", A1 + A2 + B0);

    y1 = y2 = X[0];
    
    printf("    b0 : %f, ", b0);
    printf("a1 : %f, ", a1);
    printf("a2 : %f\n", a2);
    
    printf("    B0 : %d, ", B0);
    printf("A1 : %d, ", A1);
    printf("A2 : %d\n", A2);
    //printf("sum of coeff a1 + a2 + b0 : %f\n", a1 + a2 + b0);
    for(i = 0; i < n; i++) {
        
        x0 = X[i];
        
        y0 = ( (B0 * x0 + A1 * y1 + A2 * y2) + Q/2);
        y0 /= Q;

        if(y0 <   0) y0 =   0;
        if(y0 > 255) y0 = 255;

        Y[i] = (uint8) y0;

        y2 = y1;
        y1 = y0;
    }
    
}
// ----------------------------------------------------------
void iir_q32(uint8 *X, int n, float32 alpha, int q, uint8 *Y)
// ----------------------------------------------------------
{
    //zero_ui8vector(Y, 0, n-1); // ligne de code a supprimer
    
    // CODE A COMPLETER ICI
    int i;
    int radius = 2;
    
    sint32 Q = 1 << q;
    sint32 x0, y0, y1, y2;
    sint32 X0, Y0, Y1, Y2;  
    sint32 B0, A1, A2;

    float32 gamma;
    float32  b0, a1, a2;

    gamma = exp(-alpha);
    
    b0 = (1.0f - gamma) * (1.0f - gamma);
    a1 = 2.0f * gamma;
    a2 = - gamma * gamma;
    
    
    B0 = b0 * Q;
    A1 = a1 * Q;
    A2 = a2 * Q;

    y1 = y2 = X[0];

    //X0 = x0 * Q;
    //Y0 = y0 * Q;
    Y1 = y1 * Q; 
    Y2 = y2 * Q;
    printf("    b0 : %f, ", b0);
    printf("a1 : %f, ", a1);
    printf("a2 : %f\n", a2);
    
    printf("    B0 : %d, ", B0);
    printf("A1 : %d, ", A1);
    printf("A2 : %d\n", A2);

    //printf("    sum of coeff A1 + A2 + B0 : %d\n", A1 + A2 + B0);
        /* need to be 256 */
    if (B0 + A1 + A2 == 255) {
        B0 += 1;
    }
    
    for(i = 0; i < n; i++) {
        X0 = X[i] * Q;

        Y0 = (B0 * X0 + A1 * Y1 + A2 * Y2) + (Q)/2;
        //printf("    Y0 : %d\n", Y0);
        Y0 /= Q;
        //printf("    Y0/Q : %d\n", Y0);

        y0 = (Y0 + Q/2) / (Q);
        //printf("    y0 : %d\n", y0);
        if(y0 <   0) y0 =   0;
        if(y0 > 255) y0 = 255;

        Y[i] = (uint8) y0;

        Y2 = Y1; Y1 = Y0;
        /*
        printf("    X0 : %d, ", X0);
        printf("Y0 : %d, ", Y0);
        printf("Y1 : %d, ", Y1);
        printf("Y2 : %d\n", Y2);
        */
    }
}
