/*
**      Author: Nyx Zheng, zyh@rock-chips.com
**      Date:   19 December 2016
**      File:   hmmutils.c
**      Purpose: utilities for reading, writing HMM stuff. 
**      Organization: Rockchip 
**
**      $Id: viterbi.c,v 1.4 2016/12/15 15:51:26 Nyx Zheng $
*/

#include <math.h>
#include <stdio.h>
#include "hmm.h"
#include "nrutil.h"
static char rcsid[] = "$Id: viterbi.c,v 1.2 1998/02/19 16:31:35 kanungo Exp kanungo $";
#define MAXFLOAT      3.40282347e+38F
#define PI 3.1415926
double computMaxB(HMM *phmm,float **O,int T,int frame,int state) {
    int t;
    float prob_t = 1.0,mean_t = 0.0,coef = 1.0;
    float gconst_t = 0.0,var_t = 0.0,tmp = 0.0,x = 0.0;
    if(state == 6) return 0.0;//
    tmp += phmm->gconst[state];
    for(t = 1;t <= T ; t++ ) {//t is vector number
        mean_t = phmm->mean[state][t]; 
        var_t = phmm->varian[state][t];
        x = O[frame][t]-mean_t;
        tmp += x * x / var_t;
        coef+=(-log(sqrt(2*PI*var_t)));
    }
    tmp *= -0.5; 
    prob_t = tmp + coef;
    //LOG("prob_t:%lf coef:%lf\n",prob_t,coef); 
    //prob_t = 1/((var_t)) * exp(-2*fabs(O[frame][t]-mean_t)/((var_t)*(var_t))); 
    return prob_t;
}
void Viterbi_init(HMM *phmm)
{
    int i,j;
    LOG("logA:\n");
    for(i = 1; i<= phmm->N ;i++) {
        for(j = 1; j<= phmm->N ;j++) {
            if(phmm->A[i][j] > 0.000001)
                phmm->A[i][j] = log(phmm->A[i][j]);
            else
                phmm->A[i][j] = -1e30;
            //LOG("%lf ", phmm->A[i][j]);
        }
        //LOG("\n");
    }

}
void Viterbi(HMM *phmm, int Frame, float **O, double **delta, int **psi, 
        int *q, double *pprob,int *last_state)
{
    int 	i, j;	/* state indices */
    int  	t,frame,state = *last_state;/* time index */

    int	    maxvalind;
    double	maxval, val;
    double  prob_t = 1.0,mean_t = 0.0,var_t = 0.0,maxBval_t = 0.0,log_A_t = -1e10;
    int T = 13;
    /* 1. Initialization  */
    //LOG("Viterbi phmm->N:%d frame:%d:\n",phmm->N,Frame);

    /*for(i = 1;i <= Frame;i++) {
        for(j = 1;j<=phmm->N;j++) {
            delta[i][j] = -1e30;//maxBval_t;//log(phmm->pi[i]) + maxBval_t;
            psi[i][j] = 1;
        }
    }*/
    if(Frame == 1) {
        for (i = 2; i < phmm->N; i++) {
            maxBval_t = computMaxB(phmm,O,T,1,i);
            delta[1][i] = maxBval_t;//log(phmm->pi[i]) + maxBval_t;
            psi[1][i] = 2;
        }
        *last_state = 2;
    } else {
    /* 2. Recursion */
    //第一帧状态为 2 ；
    //maxvalind = 2;
    //state = 2;
    //for (frame = 2; frame <= Frame; frame++) {
        maxval = -1e30;
        for (j = 2; j <= phmm->N; j++) {
            for (i = 2; i <= phmm->N;i++) {
                log_A_t = phmm->A[state][i];
                val = delta[Frame-1][i] + log_A_t;
                if (val > maxval) {
                    maxval = val;	
                    maxvalind = i;
                }
            }
            psi[Frame][j] = maxvalind;
            delta[Frame][j] = maxval+ computMaxB(phmm,O,T,Frame,j);
        }
        *last_state = maxvalind;
    //}
    } 
    /* 3. Termination */
    *pprob = -1e10;
    q[Frame] = 1;
    for (i = 2; i <= phmm->N; i++) {
        //LOG("delta[Frame][%d]:%lf\n",i,delta[Frame][i]);
        if (delta[Frame][i] > *pprob) {
            *pprob = delta[Frame][i];	
            q[Frame] = i;
        }
    }
    
    /* 4. Path (state sequence) backtracking */
    for (frame = Frame - 1; frame >= 1; frame--) {
        q[frame] = psi[frame+1][q[frame+1]];
        //LOG("%d ",q[frame]);
    }
    //LOG("\n");
    //LOG("pprob:%lf\n",*pprob);
}

void ViterbiLog(HMM *phmm, int T, int *O, double **delta, int **psi,
        int *q, double *pprob)
{
    int     i, j;   /* state indices */
    int     t;      /* time index */

    int     maxvalind;
    double  maxval, val;
    double  **biot;

    /* 0. Preprocessing */

    for (i = 1; i <= phmm->N; i++) 
        phmm->pi[i] = log(phmm->pi[i]);
    for (i = 1; i <= phmm->N; i++) 
        for (j = 1; j <= phmm->N; j++) {
            phmm->A[i][j] = log(phmm->A[i][j]);
        }

    biot = dmatrix(1, phmm->N, 1, T);
    for (i = 1; i <= phmm->N; i++) 
        for (t = 1; t <= T; t++) {
            biot[i][t] = log(phmm->B[i][O[t]]);
        }

    /* 1. Initialization  */

    for (i = 1; i <= phmm->N; i++) {
        delta[1][i] = phmm->pi[i] + biot[i][1];
        psi[1][i] = 0;
    }

    /* 2. Recursion */

    for (t = 2; t <= T; t++) {
        for (j = 1; j <= phmm->N; j++) {
            maxval = -MAXFLOAT;
            maxvalind = 1;
            for (i = 1; i <= phmm->N; i++) {
                val = delta[t-1][i] + (phmm->A[i][j]);
                if (val > maxval) {
                    maxval = val;
                    maxvalind = i;
                }
            }

            delta[t][j] = maxval + biot[j][t];
            psi[t][j] = maxvalind;

        }
    }

    /* 3. Termination */

    *pprob = -MAXFLOAT;
    q[T] = 1;
    for (i = 1; i <= phmm->N; i++) {
        if (delta[T][i] > *pprob) {
            *pprob = delta[T][i];
            q[T] = i;
        }
    }

    *pprob = *pprob;
    LOG("pprob:%lf\n",*pprob); 

    /* 4. Path (state sequence) backtracking */

    for (t = T - 1; t >= 1; t--)
        q[t] = psi[t+1][q[t+1]];

}


