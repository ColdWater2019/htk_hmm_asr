//#define MFCC_DEBUG
#ifdef MFCC_DEBUG
#define MFCC_LOG(format,args...) printf(format,##args)
#else
#define MFCC_LOG(format,args...)
#endif
typedef struct _TWavHeader
{
    int rId;              //标志符（RIFF）
    int rLen;             //数据大小,包括数据头的大小和音频文件的大小
    int wId;              //格式类型（"WAVE"）
    int fId;              //"fmt"
    int fLen;             //Sizeof(WAVEFORMATEX)
    short wFormatTag;     //编码格式，包括WAVE_FORMAT_PCM，WAVEFORMAT_ADPCM等
    short nChannels;      //声道数，单声道为1，双声道为2
    int nSamplesPerSec;   //采样频率
    int nAvgBytesPerSec;  //每秒的数据量
    short nBlockAlign;    //块对齐
    short wBitsPerSample; //WAVE文件的采样大小
    int dId;              //"data"
    int wSampleLength;    //音频数据的大小
}TWavHeader;

/* -------------------- MFCC Related Operations -------------------- */

typedef struct{
   int frameSize;       /* speech frameSize */
   int numChans;        /* number of channels */
   long sampPeriod;     /* sample period */
   int fftN;            /* fft size */
   int klo, khi;        /* lopass to hipass cut-off fft indices */
   float fres;          /* scaled fft resolution */
   float* cf;           /* array[1..pOrder+1] of centre freqs */
   int* loChan;         /* array[1..fftN/2] of loChan index */
   float* loWt;         /* array[1..fftN/2] of loChan weighting */
   float* hamWin;       /* array[1..N] generate precomputed Hamming window */
}FBankInfo;

void PreEmphasise (FBankInfo fb,float *s, float k);
void GenHamWindow (FBankInfo fb,int frameSize);
void Ham (FBankInfo fb,float *s,int frameSize);
void initfft(FBankInfo fb,float *s,float *fftbuf);
void Realft (float *s,int fftN);
void FFT(float *s, int fftN,int invert);
void initMel(FBankInfo *fb,int numChan);
float Mel(int k,float fres);
void Wave2FBank(FBankInfo fbInfo,float* fbank,float *s);
void FBank2MFCC(float* fbank,int numChan,int n,float *c);
void CptEn(FBankInfo fb,float *s);
void WeightCepstrum (int start, int count, int cepLiftering,float *c);
static void GenCepWin (float *cw,int cepLiftering, int count);
float FBank2C0(float* fbank,int );

void MFCC_init (FBankInfo *fb,int sampleRate,int bitNum,int frame_len_ms,int numChan);
void MFCC_process(FBankInfo fb,float *data,float *out); 
