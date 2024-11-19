/* GB3MBA receiver software for RPI                */
/*                                                 */
/* Heather Nickalls 2023                           */
/*                                                 */
/* versions:                                       */
/*   Aug/2023 0.0 - created version based on Rx    */ 
/*
 * incoming data is:
 *     32 bytes header:
 *        8 bytes : station ID
 *        8 bytes : stream Key
 *       16 bytes : reserved
 *     4096 bytes : 1024 I/Q pairs (shorts)
 *
 * outgoing data is:
 *        LWS_PRE : data for the websockets
 *        1 bytes : station ID
 *     4096 bytes : 1024 floats (4 bytes) FFT data
 *
 * FFT data is:
 *        1 float : not used (ported code from BASIC)
 *    2048 floats : 1024 complex floats
 *
 *------------------------------------------------ */ 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <unistd.h>
#include <time.h>
#include <libwebsockets.h>
#include "protocol.h"
#include "protocol_restream.h"
#include "websocket_server.h"
#include "main.h"
#include "pthread.h"
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <sys/types.h>
#include <time.h> 
#include <fcntl.h>
#include <math.h>
#include "goertzel.h"

#define WATERFALL_GAIN 256.0
//#define SAVE_STREAM_1
#define DATA_PRE 32

unsigned char restream_buffer[LWS_PRE + 1 + 4096*sizeof(unsigned char)];

/* we need to have a load of global data buffers so that the threads can access the right data */
/* note, many of these could become local to the thread if we want to clean it all up */
unsigned char transmit_buffer[LWS_PRE + 32 + 512*sizeof(unsigned char)];
//unsigned char transmit_buffer[LWS_PRE + 32 + 512*sizeof(unsigned char)];

unsigned char   in_dat[NUM_CONNECTIONS][4096+DATA_PRE]; /* the incoming data from clients     */
float     source_dat_i[NUM_CONNECTIONS][N];             /* the I circular buffer for goertzel */
float     source_dat_q[NUM_CONNECTIONS][N];             /* the Q circular buffer for goertzel */
float              mag[NUM_CONNECTIONS][512];           /* output of the Goertzel algorithms  */
int             connfd[NUM_CONNECTIONS];
int     running_totals[NUM_CONNECTIONS];
int         timestamps[NUM_CONNECTIONS];
int       circular_ptr[NUM_CONNECTIONS];
int     circular_index[NUM_CONNECTIONS];
int         data_ready[NUM_CONNECTIONS];
pthread_t      threads[NUM_CONNECTIONS];
int         thread_ids[NUM_CONNECTIONS];
int     restream_ready[NUM_CONNECTIONS];

/*--------------------------------------------------------------------------------------------*/
void transmit_FFT() {
/*--------------------------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------------------------*/
    lws_callback_on_writable_all_protocol(context, protocol);
}


/*--------------------------------------------------------------------------------------------*/
void transmit_restream() {
/*--------------------------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------------------------*/
    lws_callback_on_writable_all_protocol(context, protocol_restreamer);
}


/*--------------------------------------------------------------------------------------------*/
void *client_thread_fn(void *thread_id) { 
/*--------------------------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------------------------*/
    bool running;
    struct timespec ts;
    unsigned int theTick;
    float *source_dat_i_ptr;
    float *source_dat_q_ptr;
    uint8_t *in_dat_ptr;
    uint16_t q_dat_u16, i_dat_u16;
    int n;
    int i;
    int buffer_cnt;

    running = true;
    i = (int)((long int)thread_id);
    printf("Thread connected %i, %i\n", i, connfd[i]);

    /* deal with timestamps */
    clock_gettime(CLOCK_MONOTONIC, &ts);
    timestamps[i] = ts.tv_sec;
        
    while (running) {
        /* for each connection we keep track of how long since it last send something */
        /* and if it has been away too long we chuck it out of the queue */
        clock_gettime(CLOCK_MONOTONIC, &ts);
        if (ts.tv_sec - timestamps[i]>10) {
            printf("Thread Closing %i, %i\n", i, connfd[i]);
            close(connfd[i]);
            connfd[i]=-1;
            running_totals[i]=0;
            circular_ptr[i]=0;
            circular_index[i]=0;
            running=false;
        }

        /* Now we come to actually getting data for each connection */
        if ((n = read(connfd[i], &(in_dat[i][running_totals[i]]), DATA_PRE+4096-running_totals[i])) > 0) {
        //     printf("i=%i, n=%i, fd=%i, id=%i,running=%i\n", i,n,connfd[i],in_dat[i][8],running_totals[i]);
            /* we need to check if we have a valid start of stream. there are two possibles, either 'Heather'           */
            /*  or 'H'x where x is the second byte = 0b101xxxxx i.e top 3 bits are 101 and rest is part of the latitude */
            if ((in_dat[i][0]=='H') && (in_dat[i][1]=='e') && (in_dat[i][2]=='a') && (in_dat[i][3]=='t') &&
                (in_dat[i][4]=='h') && (in_dat[i][5]=='e') && (in_dat[i][6]=='r')) { 
                running_totals[i] += n;
      //      } else if ((in_dat[i][0]=='H') && ((in_dat[i][1] & 0xe0)==0xe0)) {
      //          running_totals[i] += n;
            } else {
                printf("Got bad data %i, running total=%i, n=%i, 0,1,2=%i, %i, %i \n",
                              i, running_totals[i], n, in_dat[i][0], in_dat[i][1], in_dat[i][2]);
                running_totals[i] = 0;
            }
            if (running_totals[i] > DATA_PRE+4096) {
                printf("overrun on running totals %i, %i\n", i, running_totals[i]);
            }
            /* now we have some data, we check to see if we have got a full packet yet */
            if (running_totals[i] == DATA_PRE+4096) {

                running_totals[i]=0;
                /* reset the timeout as this connection just sent us data */
                timestamps[i] = ts.tv_sec;
#ifdef SAVE_STREAM_1
                if (i==0) {
                    fwrite(&in_dat[i][DATA_PRE], sizeof(unsigned char), 4096, outfile);
                }
#endif
                /* we need to re-stream this data out, so iwait for the last to be sent */
                /* and then tell the display task to go ahead and do it for this one    */
           //     if (i==10) {   // use this to disable to restreamer
                    while (restream_ready[i] == 1) {};
		    restream_ready[i] = 1;
           //     }

                /* now we set up the pointers for inserting the data into the buffers */
                in_dat_ptr = (uint8_t *)&(in_dat[i][DATA_PRE]);
                source_dat_i_ptr = &(source_dat_i[i][circular_ptr[i]]);
                source_dat_q_ptr = &(source_dat_q[i][circular_ptr[i]]);

                for (buffer_cnt=0; buffer_cnt<1024; buffer_cnt++) {
                    /* copy one complete IQ sample into the goertzle circular buffer, and convert to double */
                    q_dat_u16 = (uint16_t)(*in_dat_ptr);
                    in_dat_ptr++;
                    q_dat_u16 |= ((uint16_t)(*in_dat_ptr))<<8;
                    in_dat_ptr++;
                    i_dat_u16 = (uint16_t)(*in_dat_ptr);
                    in_dat_ptr++;
                    i_dat_u16 |= ((uint16_t)(*in_dat_ptr))<<8;
                    in_dat_ptr++;
                    if (q_dat_u16 > 32767) {
                        *source_dat_q_ptr = (float)(q_dat_u16) - 65536.0;
                    } else {
                        *source_dat_q_ptr = (float)(q_dat_u16);
                    }
                    source_dat_q_ptr++;
                    if (i_dat_u16 > 32767) {
                        *source_dat_i_ptr = (float)(i_dat_u16) - 65536.0;
                    } else {
                        *source_dat_i_ptr = (float)(i_dat_u16);
                    }
                    source_dat_i_ptr++;
  
                    /* if we have copied N_PER_FFT samples then we do the FFT and display */
                    /* first we move the insertion point onto the next sample, so it now points to the oldest sample */
                    /* note this is a pointer to a sample, not a pointer to a byte */
                    circular_ptr[i]++;
                    if (circular_ptr[i]>N) {
                        printf("Circular ptr overrun %i, %i\n", i, circular_ptr[i]);
                    }
                    /* if we have got to the end of the circular buffer then we go back to the beginning */
                    if (circular_ptr[i]==N) {
                        circular_ptr[i]=0;
                        source_dat_i_ptr = &(source_dat_i[i][0]);
                        source_dat_q_ptr = &(source_dat_q[i][0]);
                    }
  
                    /* since we are re-using data to get the display rate up, we need to decide if we have got there yet */
                    circular_index[i]++;
                    if (circular_index[i]>N_PER_FFT) {
                        printf("circular index overrun %i, %i\n", i, circular_index[i]);
                    }
                    /* if we have got enough data for another FFT, then we can do it now */
                    if (circular_index[i]==N_PER_FFT) {
                        circular_index[i]=0;
  
                        /* let's start timing the hard work bit */
                        clock_gettime( CLOCK_MONOTONIC, &ts );
                        theTick  = ts.tv_nsec / 1000000;
                        theTick += ts.tv_sec * 1000;
  
                        /* now we do the hard work - first off the hamming windowing */
                        goertzel_HammingWind(i, circular_ptr[i]);
                        /* we need to wait on the host thread to send the data to the web, so we can overwrite */
                        while (data_ready[i] == 1) {};
                        /* and then the DFTs */
                        goertzel(i);
  
                        /* now we finish timing the hard work and print it */
                        clock_gettime( CLOCK_MONOTONIC, &ts );
                        theTick = (ts.tv_nsec / 1000000 + ts.tv_sec * 1000) - theTick;
//                        printf("this one, %i, took me %i ticks\n", i, theTick);
  
                        /* now we tell the host thread that we have data ready to send out */
                        data_ready[i]=1;
                    }
                }
            }
        } else {
            usleep(1000);
        }
    }
    return NULL;
}


/*--------------------------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------------------------*/
void *display_thread_fn(void *data) {
/*--------------------------------------------------------------------------------------------*/
    int listenfd = 0,  trialfd=0;
    struct sockaddr_in serv_addr; 
    int j;
    int i;
    unsigned char *transmit_buffer_ptr;
    unsigned char clipped;
    float *mag_ptr;
    int data_ptr;

#ifdef SAVE_STREAM_1
    FILE *outfile;
    outfile = fopen("Client1_data.mba","w");
#endif

    /* first we reset all the connections to -1 (inactive) */
    for (i=0;i<NUM_CONNECTIONS;i++) {
        connfd[i]=-1;
        timestamps[i]=0;
        running_totals[i]=0;
        circular_ptr[i]=0;
        circular_index[i]=0;
        data_ready[i]=0;
        thread_ids[i]=i; 
    }

    /* we will pre-calculate all the goertzel constants to save time later */
    calculate_goertzel_consts();

    listenfd = socket(AF_INET, SOCK_STREAM, 0);
    if (listenfd<0) printf("Listenfd Failed\n");
    memset(&serv_addr, '0', sizeof(serv_addr));

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(7680); 

    if (bind(listenfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr))<0) printf("Bind Failed\n");

    /* make the socket itself non-blocking */
    int status = fcntl(listenfd, F_SETFL, fcntl(listenfd, F_GETFL, 0) | O_NONBLOCK);
    printf("non-blocking status = %i\n",status);

    if (listen(listenfd, 10)<0) printf("Listen failed\n"); 

    while(1)
    {
        /* first we have to look for new connections and get them going if one comes in */
        trialfd = accept(listenfd, (struct sockaddr*)NULL, NULL);
        if (trialfd>0) {

            /* we have a new connection so we need to find a place to store it */
            printf("trial connect %i\n", trialfd);
            /* find a place to put this thread's data */
            for (i=0; i<NUM_CONNECTIONS; i++) {
                 if (connfd[i]==-1) {
                    connfd[i]=trialfd;
                    running_totals[i]=0;
                    circular_ptr[i]=0;
                    circular_index[i]=0;
                    /* make the connection non-blocking */
                    fcntl(connfd[i], F_SETFL, fcntl(connfd[i], F_GETFL) | O_NONBLOCK);
                    break;
                 }
            } 
            if (i==NUM_CONNECTIONS) {
                printf("no place to put it\n");
            } else {
                /* create thread for this fd, connfd[i]  */
                /* pass in i for the thread to know where to put it's data */
                pthread_create(&threads[i], NULL, client_thread_fn, (void *)((long)thread_ids[i]));
            }
        } 
        
        for (i=0; i<NUM_CONNECTIONS; i++) {
            /* now we check to see if any connection has done an FFT and wants to display it */
            if (data_ready[i]) {

                /* fill in the data that tells the web which waterfall we are updating, and name of it etc. */
                transmit_buffer_ptr = &(transmit_buffer[LWS_PRE]);
                for (j = 0; j<32; j++) {
                    *transmit_buffer_ptr = in_dat[i][j];
                    transmit_buffer_ptr++;
                }
                
                //*transmit_buffer_ptr = in_dat[i][8];
                //transmit_buffer_ptr++;

                /* now we need to scale and clip the data before we send it to the web */
                mag_ptr = &(mag[i][0]);
                for (j=0;j<512;j++) {
                    if ((WATERFALL_GAIN * (*mag_ptr)) >= 255.0) {
                        clipped = 255;
                    }  else if (*mag_ptr < 0.0) {
                        clipped = 0;
                    } else {
                        clipped=(unsigned char)(WATERFALL_GAIN * (*mag_ptr));
                    }
                    mag_ptr++;
                    *transmit_buffer_ptr = clipped;
                    transmit_buffer_ptr++;
                }

                /* and finally we can send the data to the website */
                transmit_FFT();
//printf("%i\n",i);
                data_ready[i] = 0;
            }
            if (restream_ready[i]) {
    
                /* insert the radio client number into the start of the buffer */
                restream_buffer[LWS_PRE] = in_dat[i][8];
                /* copy over the actual data */
                for (data_ptr=0; data_ptr<4096; data_ptr++) {
                    restream_buffer[LWS_PRE + 1 + data_ptr] = in_dat[i][DATA_PRE + data_ptr];
                }
                transmit_restream();
                restream_ready[i] = 0;
            }
        }
    }
}
/*--------------------------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------------------------*/
int main(int argc, char *argv[]) {
/*--------------------------------------------------------------------------------------------*/
    pthread_t display_thread;

    pthread_create(&display_thread, NULL, display_thread_fn, "X");
    websocket_create();
    return (0);
}
