//#define MFCC_DEBUG
#ifdef MFCC_DEBUG
#define MFCC_LOG(format,args...) printf(format,##args)
#else
#define MFCC_LOG(format,args...)
#endif
typedef struct _TWavHeader
{
    int rId;              //��־����RIFF��
    int rLen;             //���ݴ�С,��������ͷ�Ĵ�С����Ƶ�ļ��Ĵ�С
    int wId;              //��ʽ���ͣ�"WAVE"��
    int fId;              //"fmt"
    int fLen;             //Sizeof(WAVEFORMATEX)
    short wFormatTag;     //�����ʽ������WAVE_FORMAT_PCM��WAVEFORMAT_ADPCM��
    short nChannels;      //��������������Ϊ1��˫����Ϊ2
    int nSamplesPerSec;   //����Ƶ��
    int nAvgBytesPerSec;  //ÿ���������
    short nBlockAlign;    //�����
    short wBitsPerSample; //WAVE�ļ��Ĳ�����С
    int dId;              //"data"
    int wSampleLength;    //��Ƶ���ݵĴ�С
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
