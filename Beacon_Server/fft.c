#include <stdint.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#define PI 3.1415926535897f
#define FREQ 100.0f

extern double dat[6][2049];
extern uint8_t in_dat[6][4096+32];

void FFT(int index) {
    double nn = 2 * 1024;
    uint32_t j = 1;
    uint32_t i;
    uint32_t m;
    double mmax = 2;
    double istep;
    double theta;
    double wpi, wpr, wr, wi;
    double swap;
    double tempr, tempi;
    double wtemp;
    
    for (i = 1; i<=nn; i+=2) { // Bit reversed addressing first 
        if (j > i) {
            swap     = dat[index][j];
            dat[index][j]   = dat[index][i];
            dat[index][i]   = swap;
            swap     = dat[index][j+1];
            dat[index][j+1] = dat[index][i+1];
            dat[index][i+1] = swap;
        }
        m = nn / 2;
        while ((m>=2) && (j>m)) {
            j = j - m;
            m = m / 2; // help what does z mean
        }
        j = j + m;
    }

    while (nn > mmax) {
        istep = 2 * mmax;
        theta = 2 * PI / (double)mmax;
        wpr = cos(theta);
        wpi = sin(theta);
        wr = 1;
        wi = 0;
        for (m=1; m<=mmax; m+=2) {
            for (i=m; i<=nn; i+=istep) {
                j = i + mmax;
                tempr = wr * dat[index][j  ] - wi * dat[index][j+1];
                tempi = wr * dat[index][j+1] + wi * dat[index][j  ];
                dat[index][j  ] = dat[index][i  ] - tempr;
                dat[index][j+1] = dat[index][i+1] - tempi;
                dat[index][i  ] = dat[index][i  ] + tempr;
                dat[index][i+1] = dat[index][i+1] + tempi;
            }
            wtemp = wr;
            wr = wr * wpr - wi    * wpi;
            wi = wi * wpr + wtemp * wpi;
        }
        mmax = istep;
    }
}

#define DATA_PRE 32

void HammingWind(int index) {
    uint16_t k;
    double theta;
    double i_dat, q_dat;
    
    for (k = 0; k < 1024; k++) {
        theta = 2.0 * PI * k / 1024;
        i_dat =      (double)(in_dat[index][DATA_PRE+k*4  ]) +
               256.0*(double)(in_dat[index][DATA_PRE+k*4+1]);
        if (i_dat > 32767.0) i_dat -=65536.0;
        q_dat =      (double)(in_dat[index][DATA_PRE+k*4+2]) +
               256.0*(double)(in_dat[index][DATA_PRE+k*4+3]);
        if (q_dat > 32767.0) q_dat -=65536.0;
        dat[index][k*2+1] = i_dat/32768.0 * (0.54 - 0.46 * cos(theta));
        dat[index][k*2+2] = q_dat/32768.0 * (0.54 - 0.46 * cos(theta));
    }
}

void fake_data() {
    uint16_t i;

    for (i=0; i<1024; i++) {
       dat[0][i*2  ] = sin(((double)i)/1024*2*PI*FREQ);
//       dat[0][i*2+1] = cos(((double)i)/NUM_SAMPLES*2*PI*FREQ);
    }
}

void print_data() {
    uint16_t i;

    for (i=0; i<1024; i++) {
       printf("%f\n",dat[0][i*2]);
    }
}
