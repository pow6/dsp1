//2018�N �ۑ�5 4J02 �r�����i
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#define PI M_PI
struct complex{
    double re;
    double im;
};
void intro(void);
void FFT(struct complex *xn,struct complex *XK,int N);
void IFFT(struct complex *xn,struct complex *XK,int N);
void DFT(int N,int k,struct complex *xn,struct complex *XK);
struct complex calcSum(struct complex cm1,struct complex cm2);
struct complex calcDif(struct complex cm1,struct complex cm2);
struct complex calcPro(struct complex cm1,struct complex cm2);
struct complex calcQuo(struct complex cm1,struct complex cm2);
struct complex calcCJG(struct complex cm);
//double complex calcAbsSq(struct complex cm);
//struct complex transPlrToCts(struct complex cm);
//struct complex transCtsToPlr(struct complex cm);
void twid(struct complex *wnk,int N);
void arrayReverse(struct complex *bitArray,int N);
void bitReverse_makeOrder(int eltValue, int *bitOrder); 
int countBit(int value);


int main()
{
    clock_t stDFT,edDFT,stFFT,edFFT;
    struct complex *xn;
    struct complex *XK;
    double *data,*dftData;
    int mode,n,k,N;
    int i,a;
    FILE *fpIn,*fpOUT;
    char fileName[24]="HattenData.txt";
    fpIn=fopen(fileName,"r");
    if(fpIn==NULL){
        printf("cannot open file[%s]",fileName);
        exit(1);
    }
    printf("�f�[�^��(2^N)  N=");
    scanf("%d",&N);
    N=(int)pow(2,N);
    printf("�f�[�^�� N=%d",N);
    xn=(struct complex*)malloc(sizeof(struct complex)*N);
    XK=(struct complex*)malloc(sizeof(struct complex)*N);
    for(i=0;i<N;i++){
        xn[i].re=0;
        xn[i].im=0;
        XK[i].re=0;
        XK[i].im=0;
    }
    for(i=0;i<N;i++){       
        fscanf(fpIn,"%lf",&xn[i].re);
    }
    fclose(fpIn);
    //DFT
    stDFT=clock();
//    for(i=0;i<N;i++){
//        DFT(N,i,xn,XK);
//    }
    edDFT=clock();
    //FFT
    stFFT=clock();
    FFT(xn,XK,N);
    edFFT=clock();
    printf("�����ɂ����������ԁF\n");
    printf("DFT:%fms  FFT:%fms\n",(double)(edDFT-stDFT)/CLOCKS_PER_SEC*1000,(double)(edFFT-stFFT)/CLOCKS_PER_SEC*1000);
}


//FFT�����s����
void FFT(struct complex *xn,struct complex *XK,int N)
{
    int i,j,z;
    int numOfArray=sizeof(xn)/sizeof(struct complex);
    numOfArray=N;
    int shift,now,next;                        //�v�Z���o�^�t���C�ł��炷��
    int numOfBit=countBit(numOfArray);  //�i���i2^r��r�i�j
    int numOfCalc=numOfArray/2;         //��i������̃o�^�t���C���Z��
    struct complex *wnk,tmp;                //��]�q�̃|�C���^
    wnk=(struct complex*)malloc(sizeof(struct complex)*N);
    twid(wnk,numOfArray);               //��]�q�̍쐬
    arrayReverse(xn,N);                   //���̎��_��xn�̓r�b�g�t���ɕ���
    now=numOfArray/2;
    for(i=0,shift=1;i<numOfBit;i++){            //�i�����̌J��Ԃ�����
        for(j=0;j<shift;j++){       //1�i������̃o�^�t���C���Z�����̌J��Ԃ�������������
            for(z=0;z<now;z++){       //�����܂ł�1�i��
                tmp=calcPro(xn[shift*2*z+j+shift],wnk[j*now]);
                xn[shift*2*z+j+shift]=calcDif(xn[shift*2*z+j],tmp);      //��i�v�Z
                xn[shift*2*z+j]=calcSum(xn[shift*2*z+j],tmp);    //���i�v�Z
            }
        }
        now=now>>1;
        shift=shift<<1;                 //1�i�i�ނ��ƂɃo�^�t���C���Z�ł��炷�ʂ͂Q�{�ɂȂ�
    }
    for(i=0;i<numOfArray;i++){
        XK[i]=xn[i];
    }
}
//IFFT�����s����(FFT��n�Ŋ��鏈����ǉ�����)
void IFFT(struct complex *xn,struct complex *XK,int N)
{
    int i,j,z;
    int numOfArray=sizeof(xn)/sizeof(struct complex);
    numOfArray=N;
    int shift,now,next;                        //�v�Z���o�^�t���C�ł��炷��
    int numOfBit=countBit(numOfArray);  //�i���i2^r��r�i�j
    int numOfCalc=numOfArray/2;         //��i������̃o�^�t���C���Z��
    struct complex *wnk,tmp;                //��]�q�̃|�C���^
    wnk=(struct complex*)malloc(sizeof(struct complex)*N);
    twid(wnk,numOfArray);               //��]�q�̍쐬
    for(i=0;i<numOfArray;i++){
        wnk[i]=calcCJG(wnk[i]);
    }
    now=numOfArray/2;
    for(i=0,shift=1;i<numOfBit;i++){            //�i�����̌J��Ԃ�����
        for(j=0;j<shift;j++){       //1�i������̃o�^�t���C���Z�����̌J��Ԃ�������������
            for(z=0;z<now;z++){       //�����܂ł�1�i��
                tmp=calcPro(xn[shift*2*z+j+shift],wnk[j*now]);
                xn[shift*2*z+j+shift]=calcDif(xn[shift*2*z+j],tmp);      //��i�v�Z
                xn[shift*2*z+j]=calcSum(xn[shift*2*z+j],tmp);    //���i�v�Z
            }
        }
        now=now>>1;
        shift=shift<<1;                 //1�i�i�ނ��ƂɃo�^�t���C���Z�ł��炷�ʂ͂Q�{�ɂȂ�
    }
    for(i=0;i<numOfArray;i++){
        XK[i].re=xn[i].re/N;
        XK[i].im=xn[i].im/N;
    }
}

//DFT�����s����
void DFT(int N,int k,struct complex *xn,struct complex *XK)
{
    int n;
    XK[k].re=0;
    XK[k].im=0;
    for(n=0;n<N;n++){
        XK[k].re+=xn[n].re*cos(2*PI*n*k/N)+xn[n].im*sin(2*PI*n*k/N);
        XK[k].im+=xn[n].im*cos(2*PI*n*k/N)-xn[n].re*sin(2*PI*n*k/N);
    }
}

void intro(void)
{
    printf("2018�N �ۑ�5 4J02 �r�����i\n");
    printf("�g�����F\n");
    printf("**************************\n");
    printf("1.�wFile Name to Read >>�x�ǂݍ��ރf�[�^�̃t�@�C��������͂���\n");
    printf("2.�wFile Name to Write >>�x�������ރf�[�^�̃t�@�C��������͂���\n");
    printf("3.�wMode FFT:1 IFFT:2�x���s�������֐���I������BFFT�͂P�CIFFT�͂Q�����\n");
    printf("4.�wAmount of Data�x�_���iN�j����͂���\n");
    printf("5.�w�肵���f�[�^���t�@�C���ɏo�͂����\n");
    printf("**************************\n");
}

//���f���̘a�̌v�Z
struct complex calcSum(struct complex cm1,struct complex cm2)
{
    struct complex result;
    result.re=cm1.re+cm2.re;
    result.im=cm1.im+cm2.im;
    return result;
}

//���f���̍��̌v�Z
struct complex calcDif(struct complex cm1,struct complex cm2)
{
    struct complex result;
    result.re=cm1.re-cm2.re;
    result.im=cm1.im-cm2.im;
    return result;
}

//���f���̐ς̌v�Z
struct complex calcPro(struct complex cm1,struct complex cm2)
{
    struct complex result;
    result.re=cm1.re*cm2.re-cm1.im*cm2.im;
    result.im=cm1.im*cm2.re+cm1.re*cm2.im;
    return result;
}

//���f���̏��̌v�Z
struct complex calcQuo(struct complex cm1,struct complex cm2)
{
    struct complex result;
    result.re=(cm1.re*cm2.re+cm1.im*cm2.im)/(cm2.re*cm2.re+cm2.im*cm2.im);
    result.im=(cm1.im*cm2.re-cm1.re*cm2.im)/(cm2.re*cm2.re+cm2.im*cm2.im);
    return result;
}

//���v���f���̍쐬
struct complex calcCJG(struct complex cm)
{
    struct complex result;
    result.re=cm.re;
    result.im=cm.im * (-1);
    return result;
}

//��]�s��̍쐬
void twid(struct complex *wnk,int N)
{
    int n;
    for(n=0;n<N;n++){
        wnk[n].re=cos(-2*PI*n/N);
        wnk[n].im=sin(-2*PI*n/N);
    }
}

 //���Ƃ̍s����r�b�g�t���ɕ��ёւ���
void arrayReverse(struct complex *bitArray,int N)
{
    int eltValue=sizeof(bitArray)/sizeof(struct complex);
    eltValue=N;
    int i,value;
    struct complex *tmpArray;
    int *bitOrder;
    tmpArray=(struct complex*)malloc(sizeof(struct complex)*eltValue);
    bitOrder=(int *)malloc(sizeof(int)*N);
    for(i=0;i<N;i++){
        bitOrder[i]=0;
    }
    bitReverse_makeOrder(eltValue,bitOrder);
    for(i=0;i<eltValue;i++){
        tmpArray[bitOrder[i]]=bitArray[i];
    }
    for(i=0;i<eltValue;i++){
        bitArray[i]=tmpArray[i];
    }
}

//�r�b�g�t���s����쐬����
void bitReverse_makeOrder(int eltValue, int *bitOrder) 
{
    int i,j,tmp;
    int numOfBit=countBit(eltValue);
    for(j=0;j<eltValue;j++){
        tmp=j;
        for(i=0;i<numOfBit;i++){
            bitOrder[j]=bitOrder[j]<<1;
            if((tmp&1)==1){ //0�r�b�g�ڂ�1�������Ƃ�
                bitOrder[j]+=1;
            }else{
                bitOrder[j]+=0;
            }
            tmp=tmp>>1;
        }
    }
}

//�r�b�g����Ԃ�
int countBit(int value)
{    
    int result=0;
    do{
        result++;
        value=value/2;
    }while(value!=0);
    return result-1;
}