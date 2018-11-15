//2018�N �ۑ�5 4J02 �r�����i
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#define PI M_PI
struct complex{
    double re;
    double im;
};
void DFT(int N,int k,int a,int beta,struct complex *xn,struct complex *XK,int win);
void intro(void);
double Hwindow(double data,int n,int N);

int main(){
    struct complex *xn;
    struct complex *XK;
    double *data,*dftData;
    int mode,n,k,N;
    int i,win;
    int a,beta;
    FILE *fpIn;
    char filenameIn[24];
    intro();
    printf("File Name to Read >>");
    gets(filenameIn);
    fpIn=fopen(filenameIn,"r");
    if(fpIn==NULL){
        printf("cannot open file[%s]",filenameIn);
        exit(1);
    }
    do{    
        printf("Mode DFT:1 IDFT:2 >>");
        scanf("%d",&mode);
        getchar();
    }while(mode!=1&&mode!=2);
    printf("Amount of Data >>");
    scanf("%d",&N);
    a=(mode==1)?1:(-1);   //for function data[DFT]
    beta=(mode==1)?1:N; //for function data[DFT]
    xn=(struct complex*)malloc(sizeof(struct complex)*N);
    XK=(struct complex*)malloc(sizeof(struct complex)*N);
    for(i=0;i<N;i++){
        xn[i].re=0;
        xn[i].im=0;
        XK[i].re=0;
        XK[i].im=0;
    }
    win=0;
    if(mode==1){
        //DFT
        for(i=0;i<N;i++){       
            fscanf(fpIn,"%lf",&xn[i].re);
        }
        printf("Hannig Window On:1 Off:Other>>");
        scanf("%d",&win);
        for(i=0;i<N;i++){
            DFT(N,i,a,beta,xn,XK,win);
        }
        printf("Whitch Data to Print\n[Amp.&Phase Spectrum:1] [Data of DFT:2] [BOTH:Other Number]>>");
        scanf("%d",&mode);
        if(mode!=2){
            for(i=0;i<N;i++){
                printf("�U���X�y�N�g��|X%d|�F%lf[dB]\n",i,sqrt(XK[i].re*XK[i].re+XK[i].im*XK[i].im));
                //printf("�U���X�y�N�g���F%lf[dB]\n",hypot(XK[i].re,XK[i].im));
                printf("�ʑ��X�y�N�g����%d�F%lf[deg]\n",i,atan2(XK[i].im,XK[i].re));
            }
        }
        if(mode!=1){
            for(i=0;i<N;i++){
                printf("X%d=%lf+(%lfj)\n",i,XK[i].re,XK[i].im);
            }
        }
    }else{
        //IDFT
        for(i=0;i<N;i++){
            fscanf(fpIn,"%lf,%lf",&xn[i].re,&xn[i].im);
        }
        for(i=0;i<N;i++){
            DFT(N,i,a,beta,xn,XK,win);
        }
        for(i=0;i<N;i++){
            printf("X%d=%lf+(%lfj)\n",i,XK[i].re,XK[i].im);
        }
    }
    fclose(fpIn);
}

void DFT(int N,int k,int a,int beta,struct complex *xn,struct complex *XK,int win)
{
    int n;
    XK[k].re=0;
    XK[k].im=0;
    for(n=0;n<N;n++){
        XK[k].re+=xn[n].re*cos(2*PI*n*k/N)+a*xn[n].im*sin(2*PI*n*k/N);
        XK[k].im+=xn[n].im*cos(2*PI*n*k/N)-a*xn[n].re*sin(2*PI*n*k/N);
        if(win==1){
            XK[k].re=Hwindow(XK[k].re,n,N);
            XK[k].im=Hwindow(XK[k].im,n,N);
        }
    }
    XK[k].re=XK[k].re/beta;
    XK[k].im=XK[k].im/beta;
}

void intro(void)
{
    printf("2018�N �ۑ�5 4J02 �r�����i\n");
    printf("�g�����F\n");
    printf("1.�wFile Name to Read >>�x�ƕ\�������̂ŁC�ǂݍ��ރf�[�^�̃t�@�C��������͂���\n");
    printf("2.�wMode DFT:1 IDFT:2�x�ƕ\�������̂ŁC���s�������֐���I������BDFT�͂P�CIDFT�͂Q�����\n");
    printf("3.�wAmount of Data�x�ƕ\�������̂ŁC�_���iN�j����͂���\n");
    printf("4-1a.�wHannig Window On:1 Off:Other>>�x�ƕ\�������̂ŁC�n�~���O�����g���Ȃ�P�C�g��Ȃ��Ȃ炻��ȊO�����\n");
    printf("4-1b.DFT�̏ꍇ�wWhitch Data to Print�x�wAmp.&Phase Spectrum:1 Data of DFT:2�x�ƕ\�������̂ŁC\n�ʑ��E�U���X�y�N�g�����o�͂���Ȃ�P�CDFT���ʂ����̂܂ܕ\������Ȃ�C�Q�����\n");
    printf("4-1c.�����Ƃ��o�͂���ꍇ�́C1,2�ȊO�����\n");
    printf("4-2.IDFT�̏ꍇ�C�f�[�^�t�@�C���̕ۑ��`���́C�w����,�����x�̂悤�ɃJ���}�ŋ�؂�Cj�͏Ȃ�\n");
    printf("5.�w�肵���f�[�^���\�������\n");
    printf("**************************\n");
}

double Hwindow(double data,int n,int N)
{
    return (0.54-0.46*cos(2*PI*n/N))*data;
}
