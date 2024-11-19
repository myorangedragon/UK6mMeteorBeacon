#include <stdint.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include "goertzel.h"
#include "main.h"

#define PI 3.1415926535897f

float    sine[NUM_DFTS];
float  cosine[NUM_DFTS];
float   coeff[NUM_DFTS];
float  hammings[N];

float i_dat[NUM_CONNECTIONS][N];
float q_dat[NUM_CONNECTIONS][N];

/* the cosine and sines of the test freq can be pre-calculated and then just re-used */
void calculate_goertzel_consts() {
    int k;
    int ref_bin;
    float omega;
    float theta;

    for (k=START_BIN; k<END_BIN; k++) {
         if (k>=K_BREAK) {
           ref_bin=k-K_BREAK;
         } else {
           ref_bin=k+K_BREAK;
         }
      omega = (2.0 * PI * ref_bin) / N;
      sine  [k-START_BIN] = sin(omega);
      cosine[k-START_BIN] = cos(omega);
      coeff [k-START_BIN] = 2.0 * cosine[k-START_BIN];
    }

    for (k = 0; k < N; k++) {
        theta = 2.0 * PI * k / N;
        hammings[k] = (0.54 - 0.46 * cos(theta)) / 32768.0;
    }
}

/* this takes a bin and returns the magnitude of the power spectrum at this frequency */
float goertzel_mag(int k, int index, int offset) {
    int    c;
    float  q0_r, q1_r, q2_r;
    float  q0_i, q1_i, q2_i;
    float  p_r, p_i, p;
    float  *i_dat_ptr;
    float  *q_dat_ptr;
    float  this_coeff;

    q0_r=0;
    q1_r=0;
    q2_r=0;
    q0_i=0;
    q1_i=0;
    q2_i=0;

    i_dat_ptr = &(i_dat[index][0]);
    q_dat_ptr = &(q_dat[index][0]);
    this_coeff = coeff[k-offset];
   
    for (c=0; c<N; c++) {
        if (k-offset>=NUM_DFTS) printf("k\n");

        q0_r = this_coeff * q1_r - q2_r + (*i_dat_ptr);
        q2_r = q1_r;
        q1_r = q0_r;
        q0_i = this_coeff * q1_i - q2_i + (*q_dat_ptr);
        q2_i = q1_i;
        q1_i = q0_i;

        i_dat_ptr++;
        q_dat_ptr++;
    }

    p_r = cosine[k-offset] * q1_r - q2_r - sine[k-offset] * q1_i;
    p_i = cosine[k-offset] * q1_i - q2_i + sine[k-offset] * q1_r;
    p = sqrt(p_r*p_r + p_i*p_i);

    return p;
}

/* calculates the power spectrum for all the required bins */
void goertzel(int index) {
    int bin;
    int ref_bin, offset;
    float *mag_ptr;

    mag_ptr = &(mag[index][0]);
    for (bin=START_BIN; bin<END_BIN; bin++) {
//    for (bin=START_BIN; bin<9600; bin++) {
         if (bin>=K_BREAK) {
           ref_bin=bin-K_BREAK;
           offset=START_BIN-K_BREAK;
         } else {
           ref_bin=bin+K_BREAK;
           offset=START_BIN+K_BREAK;
         }
#ifdef use_reduced_display
         if ((bin > 9685) && (bin < 9845)) {
             *mag_ptr = goertzel_mag(ref_bin, index, offset);
         } else {
             *mag_ptr = 0.0;
         }
#else
         *mag_ptr = goertzel_mag(ref_bin, index, offset);
#endif
         mag_ptr++;
    }
}

/* as with FFTs, we need to assume (i.e ensure) that the data starts at 0 and ends at 0 */
/* this is a simple hamming window that will do just that                               */
void  goertzel_HammingWind(int index, int circular_ptr) {
    uint16_t k;
    int in_ptr;
    float *i_dat_ptr;
    float *q_dat_ptr;
    float *source_dat_i_ptr;
    float *source_dat_q_ptr;
    float *hammings_ptr;

    hammings_ptr = &(hammings[0]);
    i_dat_ptr = &(i_dat[index][0]);
    q_dat_ptr = &(q_dat[index][0]);
    source_dat_i_ptr = &(source_dat_i[index][circular_ptr]);
    source_dat_q_ptr = &(source_dat_q[index][circular_ptr]);
    in_ptr = circular_ptr;

    /* for every point in the input array we apply a hamming window and extract the data  */
    /* from the circular buffer (byte) and convert into a normal linear buffer (doubles)  */
    for (k = 0; k < N; k++) {
        /* once we have converted to doubles we can go ahead and hamming window it */
        *i_dat_ptr = (*source_dat_i_ptr) * (*hammings_ptr);
        *q_dat_ptr = (*source_dat_q_ptr) * (*hammings_ptr);

        i_dat_ptr++;
        q_dat_ptr++;
        source_dat_i_ptr++;
        source_dat_q_ptr++;
        hammings_ptr++;

        /* because we are receiving in a circular buffer, we need to wrap the pointer when we reach the end */
        in_ptr++;
        if (in_ptr==N) {
            in_ptr = 0;
            source_dat_i_ptr = &(source_dat_i[index][0]);
            source_dat_q_ptr = &(source_dat_q[index][0]);
        }
    }
}

