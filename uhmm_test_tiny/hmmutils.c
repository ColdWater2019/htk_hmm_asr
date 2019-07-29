/*
**      Author: Nyx Zheng, zyh@rock-chips.com
**      Date:   19 December 2016
**      File:   hmmutils.c
**      Purpose: utilities for reading, writing HMM stuff. 
**      Organization: Rockchip 
**
**      $Id: hmmutils.c,v 1.4 2016/12/15 15:51:26 Nyx Zheng $
*/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "nrutil.h"
#include "hmm.h"
#include "mfcc.h"
static char rcsid[] = "$Id: hmmutils.c,v 1.4 1998/02/23 07:51:26 kanungo Exp kanungo $";

void ReadHMM(FILE *fp, HMM *phmm)
{
    int i, j, k;

    fscanf(fp, "M= %d\n", &(phmm->M)); 

    fscanf(fp, "N= %d\n", &(phmm->N)); 

    fscanf(fp, "A:\n");
    phmm->A = (double **) dmatrix(1, phmm->N, 1, phmm->N);
    for (i = 1; i <= phmm->N; i++) { 
        for (j = 1; j <= phmm->N; j++) {
            fscanf(fp, "%lf", &(phmm->A[i][j])); 
        }
        fscanf(fp,"\n");
    }

    fscanf(fp, "B:\n");
    phmm->B = (double **) dmatrix(1, phmm->N, 1, phmm->M);
    for (j = 1; j <= phmm->N; j++) { 
        for (k = 1; k <= phmm->M; k++) {
            fscanf(fp, "%lf", &(phmm->B[j][k]));
        }
        fscanf(fp,"\n");
    }

    fscanf(fp, "pi:\n");
    phmm->pi = (double *) dvector(1, phmm->N);
    for (i = 1; i <= phmm->N; i++) 
        fscanf(fp, "%lf", &(phmm->pi[i])); 
}

void ReadSequence_mfcc(FILE *fp, int *pT, float ***pO)
{
    float **O;
    int frame = 0,i;
    int ipframesize = 160; 
    short buffer[160]; //= {0};
    float data[161];
    FBankInfo fbInfo;
    TWavHeader wavHeader;
    int sampleRate,bitNum,frame_len_ms = 10,frame_offset = 5;//16k 10ms 5ms
    int numChan = 26;//number of filter

    O = *pO;
    fread(&wavHeader,sizeof(struct _TWavHeader),1,fp);
    sampleRate = wavHeader.nSamplesPerSec/1000;//16K
    bitNum = wavHeader.wBitsPerSample;
    
    MFCC_init(&fbInfo,sampleRate,bitNum,frame_len_ms,numChan);
    LOG("fbInfo.frameSize:%d\n",fbInfo.frameSize);
    
    fseek(fp,2L,SEEK_CUR);
    while(fread(buffer,sizeof(short),ipframesize,fp) == ipframesize)
    {
        frame++;
#if 1
        for(i=0;i<ipframesize;i++) {
            data[i+1]=buffer[i];
        }
#endif
        MFCC_process(fbInfo,data,O[frame]);
        fseek(fp,-ipframesize/2*bitNum/8L,SEEK_CUR);
    }
    *pT = frame ;
}

void ReadHMM_B(FILE *fp, HMM *phmm,int T) {
    int mean_n,varian_n;
    int i, j, k,t;
    char buf[2048];
    int tmp1,tmp2;
    double gconst;
    double **t_mean,**t_varian;
    int state_total,state_N;
    /*~o
      <STREAMINFO> 1 13
      <VECSIZE> 13<NULLD><MFCC_E><DIAGC>
      ~h "rui"
      <BEGINHMM>
      <NUMSTATES> 10
      <STATE> 2
      <MEAN> 39*/
    fscanf(fp,"~o\n");
    fscanf(fp,"<STREAMINFO> %s %s\n",buf,buf);
    fscanf(fp,"<VECSIZE> %d<NULLD><MFCC_0><DIAGC>\n",buf);
    fscanf(fp,"~h %s\n",buf);
    fscanf(fp,"<BEGINHMM>\n");
    fscanf(fp,"<NUMSTATES> %s\n",buf);
    state_total = atoi(buf);

    for(j = 2; j<=state_total - 1 ;j++)
    {
        fscanf(fp,"<STATE> %s\n",buf);
        state_N = atoi(buf);
        fscanf(fp, "<MEAN> %s\n",buf);
        mean_n = atoi(buf);
        if(j == 2) {
            phmm->mean = (double *) dmatrix(1, state_total,1,mean_n);
            for(k = 1 ;k <= state_total ;k++)
                memset(phmm->mean[k],0,sizeof(double)*mean_n);
        }
        for (i = 1; i <= mean_n; i++) 
        {
            fscanf(fp, "%lf", &(phmm->mean[j][i]));
            //LOG(" %lf",phmm->mean[j][i]); 
        }
        fscanf(fp, "\n");
        fscanf(fp, "<VARIANCE> %s\n",buf);
        varian_n = atoi(buf);
        if(j == 2) {
            phmm->varian = (double *) dmatrix(1, state_total,1,varian_n);
            for(k = 1 ;k <= state_total;k++)
                memset(phmm->varian[k],0,sizeof(double)*varian_n);
        }
        for (i = 1; i <= mean_n; i++) { 
            fscanf(fp, "%lf", &(phmm->varian[j][i])); 
            //LOG(" %lf",phmm->varian[j][i]); 
        }
        fscanf(fp, "\n");
        if(j == 2) {
            phmm->gconst = (double *) dvector(1, state_total);
            memset(phmm->gconst,0,sizeof(double)*state_total);
        }
        fscanf(fp, "<GCONST> %lf\n",&(phmm->gconst[j]));
    }
    t_mean = phmm->mean;
    t_varian = phmm->varian;

    fscanf(fp, "<TRANSP> %d\n",&(phmm->N));
    for(i = 1 ;i <= phmm->N;i++) {
        for(j = 1 ;j <= phmm->N;j++) {
            fscanf(fp,"%lf ",&phmm->A[i][j]);
            if(phmm->A[i][j] == 0.0) {
                phmm->A[i][j] = 1e-10;
            }
        }
        fscanf(fp,"\n");
    } 
    LOG("A:\n"); 
    for(j=1;j<=6;j++) {
        for(t=1;t<=6;t++) {
            LOG("%.4lf ",phmm->A[j][t]);
        }
        LOG("\n"); 
    }
}

void FreeHMM(HMM *phmm)
{
    free_dmatrix(phmm->A, 1, phmm->N, 1, phmm->N);
    free_dmatrix(phmm->B, 1, phmm->N, 1, phmm->M);
    free_dvector(phmm->pi, 1, phmm->N);
}

/*
 ** InitHMM() This function initializes matrices A, B and vector pi with
 **	random values. Not doing so can result in the BaumWelch behaving
 **	quite wierdly.
 */ 

void InitHMM(HMM *phmm, int N, int M)
{
    int i, j, k;
    double sum;


    /* initialize random number generator */

    srand48(1000);

    phmm->M = M;

    phmm->N = N;

    phmm->A = (double **) dmatrix(1, phmm->N, 1, phmm->N);

    for (i = 1; i <= phmm->N; i++) {
        sum = 0.0;
        for (j = 1; j <= phmm->N; j++) {
            phmm->A[i][j] = drand48(); 
            sum += phmm->A[i][j];
        }
        for (j = 1; j <= phmm->N; j++) 
            phmm->A[i][j] /= sum;
    }

    phmm->B = (double **) dmatrix(1, phmm->N, 1, phmm->M);

    for (j = 1; j <= phmm->N; j++) {
        sum = 0.0;	
        for (k = 1; k <= phmm->M; k++) {
            phmm->B[j][k] = drand48();
            sum += phmm->B[j][k];
        }
        for (k = 1; k <= phmm->M; k++) 
            phmm->B[j][k] /= sum;
    }

    phmm->pi = (double *) dvector(1, phmm->N);
    sum = 0.0;
    for (i = 1; i <= phmm->N; i++) {
        phmm->pi[i] = drand48(); 
        sum += phmm->pi[i];
    }
    for (i = 1; i <= phmm->N; i++) 
        phmm->pi[i] /= sum;
}

void CopyHMM(HMM *phmm1, HMM *phmm2)
{
    int i, j, k;

    phmm2->M = phmm1->M;


    phmm2->N = phmm1->N;

    phmm2->A = (double **) dmatrix(1, phmm2->N, 1, phmm2->N);

    for (i = 1; i <= phmm2->N; i++)
        for (j = 1; j <= phmm2->N; j++)
            phmm2->A[i][j] = phmm1->A[i][j];

    phmm2->B = (double **) dmatrix(1, phmm2->N, 1, phmm2->M);
    for (j = 1; j <= phmm2->N; j++)
        for (k = 1; k <= phmm2->M; k++)
            phmm2->B[j][k] = phmm1->B[j][k];

    phmm2->pi = (double *) dvector(1, phmm2->N);
    for (i = 1; i <= phmm2->N; i++)
        phmm2->pi[i] = phmm1->pi[i]; 

}

