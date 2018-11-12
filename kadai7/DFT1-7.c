//2018年 課題5 4J02 池口恭司
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
struct complex calcSum(struct complex cm1,struct complex cm2);
struct complex calcDif(struct complex cm1,struct complex cm2);
struct complex calcPro(struct complex cm1,struct complex cm2);
struct complex calcQuo(struct complex cm1,struct complex cm2);
struct complex calcCJG(struct complex cm);
//double complex calcAbsSq(struct complex cm);
//struct complex transPlrToCts(struct complex cm);
//struct complex transCtsToPlr(struct complex cm);
void twid(struct complex *wnk,int N);

int main()
{
    struct complex *xn;
    struct complex *XK;
    double *data,*dftData;
    int mode,n,k,N;
    int i,win;
    int a,beta;
    FILE *fpIn,*fpOUT;
    char fileName[24];
    intro();
    printf("File Name to Read >>");
    scanf("%s",fileName);
    fpIn=fopen(fileName,"r");
    if(fpIn==NULL){
        printf("cannot open file[%s]",fileName);
        exit(1);
    }
    printf("File Name to Write >>");
    scanf("%s",fileName);
    fpOUT=fopen(fileName,"w");
    if(fpOUT==NULL){
        printf("cannot open file[%s]",fileName);
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
                //printf("振幅スペクトル|X%d|：%lf[dB]\n",i,sqrt(XK[i].re*XK[i].re+XK[i].im*XK[i].im));
                //printf("振幅スペクトル：%lf[dB]\n",hypot(XK[i].re,XK[i].im));
                //printf("位相スペクトルΘ%d：%lf[deg]\n",i,atan2(XK[i].im,XK[i].re));
                printf("%lf[deg]\n",atan2(XK[i].im,XK[i].re));
            }
        }
        if(mode!=1){
            for(i=0;i<N;i++){
                printf("X%d=%lf+(%lfj)\n",i,XK[i].re,XK[i].im);
                fprintf(fpOUT,"%lf\n",sqrt(pow(xn[i].re,2)+pow(xn[i].im,2)));
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
    printf("2018年 課題5 4J02 池口恭司\n");
    printf("使い方：\n");
    printf("**************************\n");
}

double Hwindow(double data,int n,int N)
{
    return (0.54-0.46*cos(2*PI*n/N))*data;
}

struct complex calcSum(struct complex cm1,struct complex cm2)
{
    complex result;
    result.re=cm1.re+cm2.re;
    result.im=cm1.im+cm2.im;
    return result;
}

struct complex calcDif(struct complex cm1,struct complex cm2)
{
    complex result;
    result.re=cm1.re-cm2.re;
    result.im=cm1.im-cm2.im;
    return result;
}

struct complex calcPro(struct complex cm1,struct complex cm2)
{
    complex result;
    result.re=cm1.re*cm2.re-cm1.im*cm2.im;
    result.im=cm1.im*cm2.re+cm1.re*cm2.im;
    return result;
}

struct complex calcQuo(struct complex cm1,struct complex cm2)
{
    complex result;
    result.re=(cm1.re*cm2.re+cm1.im*cm2.im)/(cm2.re*cm2.re+cm2.im*cm2.im);
    result.im=(cm1.im*cm2.re-cm1.re*cm2.im)/(cm2.re*cm2.re+cm2.im*cm2.im);
    return result;
}

struct complex calcCJG(struct complex cm)
{
    complex result;
    result.re=cm.re;
    result.im=cm.im * (-1);
    return result;
}

void twid(struct complex *wnk,int N) //
{
    int n;
    for(n=0;n<N;n++){
        wnk[n].re=cos(2*PI*n*k/N);
        wnk[n].im=sin(2*PI*n*k/N);
    }
}

void bitReverse(int bitOrder[]) //ビットを逆順にする
{
    int eltValue=sizeof(bitOrder)/sizeof(int);
    int i,value;
    for(value=0;eltValue>>value;value++);
    for(i=0;i<eltValue;i++){
        bitOrder[i]=bitReverse(bitOrder[i],value);
    }
}

int bitReverse_forOne(int order,int value)
{
    int *num,i;
    num=(int *)malloc(sizeof(int)*value);
    for(i=value-1;i>=0;i--){
        if((order&1)==1){ //0ビット目が1だったとき
            num[i]=1;
        }else{
            num[i]=0;
        }
        order=order>>1;
    }
    return order;
}