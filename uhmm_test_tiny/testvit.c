/*
**      Author: Nyx Zheng, zyh@rock-chips.com
**      Date:   16 December 2016 
**      File:   testvit.c
**      Purpose: driver for testing the Viterbi code.
**      Organization: Rockchip
**
**      $Id: testvit.c,v 1.0 2016/12/16 11:29:07 $
*/

#include <stdio.h> 
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include "nrutil.h"
#include "hmm.h"
#include "mfcc.h"
static char rcsid[] = "$Id: testvit.c,v 1.0 2016/12/16 11:29:07 $";
#define MAX_FRAME 2000

int main (int argc, char **argv)
{
    int 	i,j,t, T = 13,T_mfcc; 
    HMM  	hmm;
    float	**O_mfcc,*O_prob;
    int	*q;	/* state sequence q[1..state] */
    double **delta;
    int	**psi;
    double 	proba, logproba;
    FILE	*fp;

    TWavHeader wavHeader;
    FBankInfo fbInfo;
    int sampleRate,bitNum,frame_len_ms = 20;//16k 20ms 10ms
    int numChan = 26;//number of filter
    int frame = 0;
    
    int ipframesize; 
    short *buffer; //= {0};
    float *data;
    int last_state;
    O_mfcc = dmatrix(1,MAX_FRAME,1,T);//帧长20ms，最大时长20s
    if (argc != 3) {
        LOG("Usage error \n");
        exit (1);
    }

    hmm.N = 6;	
    hmm.M = 13;	

    hmm.A = (double **) dmatrix(1, hmm.N, 1, hmm.N);
    hmm.B = (double **) dmatrix(1, hmm.N, 1, hmm.M);
    hmm.pi = (double *) dvector(1, hmm.N);


    fp = fopen(argv[2], "r");
    ReadHMM_B(fp, &hmm,T);
    //fclose(fp);

    fp = fopen(argv[1], "r");
    //ReadSequence_mfcc(fp, &T_mfcc, &O_mfcc);
    //ReadSequence_1(fp, &T_mfcc, &O_mfcc);
    fread(&wavHeader,sizeof(struct _TWavHeader),1,fp);
    sampleRate = wavHeader.nSamplesPerSec/1000;//16K
    bitNum = wavHeader.wBitsPerSample;
    
    MFCC_init(&fbInfo,sampleRate,bitNum,frame_len_ms,numChan);
    LOG("fbInfo.frameSize:%d\n",fbInfo.frameSize);
    
    fseek(fp,2L,SEEK_CUR);
    ipframesize = fbInfo.frameSize;
    data = dvector(1,ipframesize+1);
    buffer = ivector(1,ipframesize);
    q = dvector(1,MAX_FRAME);
    delta = dmatrix(1, MAX_FRAME, 1, hmm.N);
    psi = imatrix(1, MAX_FRAME, 1, hmm.N);
    Viterbi_init(&hmm);
    last_state = 2;
    while(fread(buffer,sizeof(short),ipframesize,fp) == ipframesize)
    {
        frame++;
#if 1
        for(i=0;i<ipframesize;i++) {
            data[i+1]=buffer[i];
        }
#endif
        MFCC_process(fbInfo,data,O_mfcc[frame]);
        T_mfcc = frame;

        LOG("frame :%d:\n",frame);
        for(i = 1;i<=13;i++) {
            LOG("%.3f ",O_mfcc[frame][i]);//for check mfcc data
        }
        LOG("\n");

        Viterbi(&hmm, T_mfcc, O_mfcc, delta, psi, q, &proba,&last_state);
        if(last_state == 6) {
            last_state = 2;
        }
        fseek(fp,-ipframesize/2*bitNum/8L,SEEK_CUR);
    }
    for (i = T_mfcc - 1; i >= 1; i--) {
        q[i] = psi[i+1][q[i+1]];
        printf("%d ",q[i]);
    }
    printf("\n");


    /* note: ViterbiLog() returns back with log(A[i][j]) instead
     ** of leaving the A matrix alone. If you need the original A,
     ** you can make a copy of hmm by calling CopyHMM */ 

    /* ViterbiLog(&hmm, T, O, delta, psi, q, &logproba); */

    printf("Viterbi  MLE prob = %E\n", proba);

    //free_dvector(q, 1, T_mfcc);
    free_imatrix(psi, 1, T, 1, hmm.N);
    free_dmatrix(delta, 1, T, 1, hmm.N);
    FreeHMM(&hmm);
}

