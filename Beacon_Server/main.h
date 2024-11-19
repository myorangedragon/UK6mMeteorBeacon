#include <libwebsockets.h>

#define NUM_DFTS        512
#define NUM_CONNECTIONS 6

/* N defines the resolution we are going to display (Hz/Bin) = Sampling rate / N             */
/*   so at 7812 samples/sec, we achive 2Hz/bin = +/- 500Hz on a 512 bin waterfall via 7812/2 */
/*   or for +/- 100Hz = 0.4Hz / Bin = 7812/0.4 = 19530                                       */
/* N_PER_FFT defines how often we display a line on the waterfall = sample_rate / N_PER_FFT  */
/*   so 10 lines/sec is achived via 7812/10 = 781                                            */
/*   or, for 5 lines per second, we need 7812 / 5 = 1562                                     */ 
/* we need to have NUM_DFTS bins, centred around the middle of the range      */
/*   so, for an N = 3906 (2Hz/bin) the range is N/2 +/- 256, ie start at 1697 */
/* K_BREAK is the midpoint = N/2                                              */


// 5 lines/sec, 0.4Hz/bin
#define N_PER_FFT 1562
#define START_BIN 9509
#define END_BIN   (START_BIN+NUM_DFTS)
#define N         19530
#define K_BREAK   9765

// 2Hz/bin 10 lines per sec
//#define N_PER_FFT 781
//#define START_BIN 1697
//#define END_BIN   (START_BIN+NUM_DFTS)
//#define N         3906
//#define K_BREAK   1953

extern float source_dat_i[NUM_CONNECTIONS][N];
extern float source_dat_q[NUM_CONNECTIONS][N];
extern float          mag[NUM_CONNECTIONS][NUM_DFTS];
//extern unsigned char transmit_buffer[LWS_PRE + 32 + 512 *  sizeof(unsigned char)];
extern unsigned char transmit_buffer[LWS_PRE + 32 + 512 *  sizeof(unsigned char)];
extern unsigned char restream_buffer[LWS_PRE + 1 + 4096 * sizeof(unsigned char)];

