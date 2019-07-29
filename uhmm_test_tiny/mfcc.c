#include <math.h>
#include <stdio.h>
#include "mfcc.h"
//全局变量
#define PI   3.14159265358979
#define TPI  6.28318530717959

/* EXPORT->PreEmphasise: pre-emphasise signal in s */
void PreEmphasise (FBankInfo fb, float *s, float k)
{
    int i;
    float preE;

    preE = k;
    for (i=fb.frameSize;i>=2;i--) {
        s[i] -= s[i-1]*preE;
        //MFCC_LOG("s[i]:%lf preE:%lf",s[i],preE);
    }
    //MFCC_LOG("\n");
    s[1] *= 1.0-preE;
}

/* GenHamWindow: generate precomputed Hamming window function */
void GenHamWindow (FBankInfo fb,int frameSize)
{
    int i;
    float a;

    a = TPI / (frameSize - 1);
    //MFCC_LOG("frameSize:%d\n",frameSize);
    for (i=1;i<=frameSize;i++) {
        fb.hamWin[i] = 0.54 - 0.46 * cos(a*(i-1));
        //MFCC_LOG("%lf ",hamWin[i]);
    }
    //MFCC_LOG("\n");
}

/* EXPORT->Ham: Apply Hamming Window to Speech frame s */
void Ham (FBankInfo fb,float *s,int frameSize)
{
    int i;
    GenHamWindow(fb,frameSize);
    for (i=1;i<=frameSize;i++) {
        s[i] *= fb.hamWin[i];
    //    MFCC_LOG("%lf ",s[i]);
    }
    //while(1);
}

/* EXPORT-> FFT: apply fft/invfft to complex s */
void FFT(float *s, int fftN,int invert)
{
    int ii,jj,n,nn,limit,m,j,inc,i;
    double wx,wr,wpr,wpi,wi,theta;
    double xre,xri,x;

    n=fftN;
    nn=n / 2; j = 1;
    for (ii=1;ii<=nn;ii++) {
        i = 2 * ii - 1;
        if (j>i) {
            xre = s[j]; xri = s[j + 1];
            s[j] = s[i];  s[j + 1] = s[i + 1];
            s[i] = xre; s[i + 1] = xri;
        }
        m = n / 2;
        while (m >= 2  && j > m) {
            j -= m; m /= 2;
        }
        j += m;
    };
    limit = 2;
    while (limit < n) {
        inc = 2 * limit; theta = TPI / limit;
        if (invert) theta = -theta;
        x = sin(0.5 * theta);
        wpr = -2.0 * x * x; wpi = sin(theta); 
        wr = 1.0; wi = 0.0;
        for (ii=1; ii<=limit/2; ii++) {
            m = 2 * ii - 1;
            for (jj = 0; jj<=(n - m) / inc;jj++) {
                i = m + jj * inc;
                j = i + limit;
                xre = wr * s[j] - wi * s[j + 1];
                xri = wr * s[j + 1] + wi * s[j];
                s[j] = s[i] - xre; s[j + 1] = s[i + 1] - xri;
                s[i] = s[i] + xre; s[i + 1] = s[i + 1] + xri;
            }
            wx = wr;
            wr = wr * wpr - wi * wpi + wr;
            wi = wi * wpr + wx * wpi + wi;
        }
        limit = inc;
    }
    if (invert)
        for (i = 1;i<=n;i++) 
            s[i] = s[i] / nn;

}

/* EXPORT-> Realft: apply fft to real s */
void Realft (float *s,int fftN)
{
    int n, n2, i, i1, i2, i3, i4;
    double xr1, xi1, xr2, xi2, wrs, wis;
    double yr, yi, yr2, yi2, yr0, theta, x;

    n=fftN/ 2; n2 = n/2;
    theta = PI / n;
    FFT(s,fftN,0);
    x = sin(0.5 * theta);
    yr2 = -2.0 * x * x;
    yi2 = sin(theta); yr = 1.0 + yr2; yi = yi2;
    for (i=2; i<=n2; i++) {
        i1 = i + i - 1;      i2 = i1 + 1;
        i3 = n + n + 3 - i2; i4 = i3 + 1;
        wrs = yr; wis = yi;
        xr1 = (s[i1] + s[i3])/2.0; xi1 = (s[i2] - s[i4])/2.0;
        xr2 = (s[i2] + s[i4])/2.0; xi2 = (s[i3] - s[i1])/2.0;
        s[i1] = xr1 + wrs * xr2 - wis * xi2;
        s[i2] = xi1 + wrs * xi2 + wis * xr2;
        s[i3] = xr1 - wrs * xr2 + wis * xi2;
        s[i4] = -xi1 + wrs * xi2 + wis * xr2;
        yr0 = yr;
        yr = yr * yr2 - yi  * yi2 + yr;
        yi = yi * yr2 + yr0 * yi2 + yi;
    }
    xr1 = s[1];
    s[1] = xr1 + s[2];
    s[2] = 0.0;
       //MFCC_LOG("Realft:\n"); 
    for(i = 1;i<=n;i++) {
       //MFCC_LOG("%lf ",s[i]); 
    }
       //MFCC_LOG("\n");
}




/*  fft变换时的预处理  */
void initfft(FBankInfo fb,float *s,float *fftbuf)
{
    int k;
    for (k=1; k<=fb.frameSize; k++) 
        fftbuf[k] = s[k];    /* copy to workspace */
    for (k=fb.frameSize+1; k<=fb.fftN; k++) 
        fftbuf[k] = 0.0;   /* pad with zeroes */  

}

/* Mel滤波预处理函数    */
void initMel(FBankInfo *fb,int numChan)
{
    float melk;
    float ms= 2840.037842;//2146.08;//
    //float ms=3177.54;
    int k,chan;
    int maxChan;
    int Nby2;
    fb->numChans = numChan;//三角带通滤波器组的个数为26个
    maxChan = numChan + 1;
    fb->fftN = 2;
    while(fb->frameSize > fb->fftN) fb->fftN *= 2;
    fb->klo = 2;fb->khi = fb->fftN / 2;Nby2 = fb->fftN/2;
    fb->fres=(float)1E7/(fb->sampPeriod * 700 * fb->fftN);

    MFCC_LOG("framesize:%d fres:%lf fb->fftN:%d fb->khi:%d\n",fb->frameSize,fb->fres,fb->fftN,fb->khi);
    //MFCC_LOG("nSamplesPerSec:%d\n",waveheader.nSamplesPerSec);
    ms = Mel(fb->khi+1,fb->fres);
    //MFCC_LOG("ms:%lf \n",ms);
    for (chan=1; chan <= maxChan; chan++) 
    {
        fb->cf[chan] = ((float)chan/(float)maxChan)*ms ;
    }
    for (k=1,chan=1; k<=Nby2;k++)
    {
        melk = Mel(k,fb->fres);
        if (k<fb->klo || k>fb->khi) 
            fb->loChan[k]=-1;
        else 
        {
            while (fb->cf[chan] < melk  && chan<=maxChan)
                ++chan;
            fb->loChan[k] = chan-1;
        }
        //MFCC_LOG("melk:%lf,k:%d,fres:%lf,loChan[k]:%lf\n",melk,k,fres,loChan[k]);
        //MFCC_LOG("klo:%d,khi:%d\n",klo,khi);
    }

    /* Create vector of lower channel weights */   
    for (k=1; k<=Nby2; k++) 
    {
        chan = fb->loChan[k];
        if (k<fb->klo || k>fb->khi) 
            fb->loWt[k]=0.0;
        else 
        {
            if (chan>0) 
                fb->loWt[k] = ((fb->cf[chan+1] - Mel(k,fb->fres)) / 
                        (fb->cf[chan+1] - fb->cf[chan]));
            else
                fb->loWt[k] = (fb->cf[1]-Mel(k,fb->fres))/(fb->cf[1]);
        }
    }
}

float Mel(int k,float fres)
{
    return 1127 * log(1 + (k-1)*fres);
}

/* EXPORT->Wave2FBank:  Perform filterbank analysis on speech s */
void Wave2FBank(FBankInfo fbInfo,float *fbank,float *s)
{
    const float melfloor = 1.0;
    int k, bin;
    float real,imag;   /* real and imag parts */
    float ek;      /* energy of k'th fft channel */

    for(k = 0;k < fbInfo.numChans+1;k++)
        fbank[k] = 0.0; 
    for (k = 2; k <= fbInfo.khi; k++) 
    {             /* fill bins */
        real = s[2*k-1]; imag = s[2*k];
        ek = sqrt(real*real + imag*imag);
        bin = fbInfo.loChan[k];
        real = fbInfo.loWt[k]*ek;
        //MFCC_LOG("ek:%lf bin:%d,t1:%lf \n",ek,bin,real);
        if (bin>0) fbank[bin] += real;
        if (bin<fbInfo.numChans) fbank[bin+1] += ek - real;
        //MFCC_LOG("fbank[%d]:%lf",bin,fbank[bin]); 
    }
    //MFCC_LOG("\n");

    /* Take logs */
    for (bin=1; bin<=fbInfo.numChans; bin++)
    { 
        real = fbank[bin];
        if (real<melfloor) real = melfloor;
        fbank[bin] = log(real);
        //MFCC_LOG("fbank[%d]:%lf ",bin,fbank[bin]);
    }
    // MFCC_LOG("\n");
    //while(1);
}


/* EXPORT->FBank2MFCC: compute first n cepstral coeff */
void FBank2MFCC(float* fbank,int numChan,int n,float *c)
{
    int j,k;
    float mfnorm,pi_factor,x;

    mfnorm = sqrt(2.0/(float)numChan);
    pi_factor = PI/(float)numChan;
    for (j=1; j<=n; j++)  {
        c[j] = 0.0; x = (float)j * pi_factor;
        for (k=1; k<=numChan; k++)
            c[j] += fbank[k] * cos(x*(k-0.5));
        c[j] *= mfnorm;
    }        
}

/* 计算每帧的能量 */ 
void CptEn(FBankInfo fb,float *s)
{
    int k=0;

    float En = 0.0;  
    for (k=1; k<=fb.frameSize; k++) 
        En += (s[k]*s[k]);
    En=log(En);

}

void WeightCepstrum (int start, int count, int cepLiftering,float *c)
{
    int i,j;
    float cepWin[13];
    //if (cepWinL != cepLiftering || count > cepWinSize)
    GenCepWin(cepWin, cepLiftering,count);
    j = start;
    for (i=1;i<=count;i++)
        c[j++] *= cepWin[i];
}

/* GenCepWin: generate a new cep liftering vector */
static void GenCepWin (float *cw,int cepLiftering, int count)
{
    int i;
    float a, Lby2;

    // if (cepWin==NULL || VectorSize(cepWin) < count)
    //   cepWin = CreateVector(&sigpHeap,count);
    a = PI/cepLiftering;
    Lby2 = cepLiftering/2.0;
    for (i=1;i<=count;i++)
        cw[i] = 1.0 + Lby2*sin(i * a);

}  

//
float FBank2C0(float *fbank,int numChan)
{
    int k;//numChan;
    float mfnorm,sum;
    mfnorm = sqrt(2.0/(float)numChan);
    sum = 0.0; 
    for (k=1; k<=numChan; k++)
        sum += fbank[k];
    //MFCC_LOG("sum:%lf mfnorm:%lf\n",sum,mfnorm);
    return sum * mfnorm;
}

void MFCC_init (FBankInfo *fb,int sampleRate,int bitNum,int frame_len_ms,int numChan) {
    fb->frameSize = sampleRate * frame_len_ms;//20ms
    fb->sampPeriod = 1E4 / sampleRate; 
    fb->cf = dvector(1,numChan + 2);//maxChan + 1
    fb->loChan = ivector(1,fb->frameSize + 1);
    fb->loWt = dvector(1,fb->frameSize + 1);
    fb->hamWin = dvector(1,fb->frameSize);
    initMel(fb,numChan);
}

void MFCC_process(FBankInfo fb,float *data,float *out) {
   float fftbuf[fb.fftN];
   float c[13]; 
   float C0; 
   float fbank [fb.numChans+1];
   int t;
   float PREEMCOEF = 0.97;
   int T = 13;
   PreEmphasise (fb,data, PREEMCOEF);
   Ham (fb,data, fb.frameSize);
   CptEn(fb,data);
   initfft(fb,data,fftbuf);
   Realft (fftbuf,fb.fftN);
   Wave2FBank(fb,fbank,fftbuf);
   FBank2MFCC(fbank,fb.numChans,12,c);
   C0 = FBank2C0(fbank,fb.numChans);
   WeightCepstrum (1, 12, 22,c);
   for(t=1;t<T;t++) {
       out[t] = c[t];
   }
   out[T] = C0;
}
