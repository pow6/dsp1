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

int main(){
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
    printf("1.『File Name to Read >>』と表示されるので，読み込むデータのファイル名を入力する\n");
    printf("2.『Mode DFT:1 IDFT:2』と表示されるので，実行したい関数を選択する。DFTは１，IDFTは２を入力\n");
    printf("3.『Amount of Data』と表示されるので，点数（N）を入力する\n");
    printf("4-1a.『Hannig Window On:1 Off:Other>>』と表示されるので，ハミング窓を使うなら１，使わないならそれ以外を入力\n");
    printf("4-1b.DFTの場合『Whitch Data to Print』『Amp.&Phase Spectrum:1 Data of DFT:2』と表示されるので，\n位相・振幅スペクトルを出力するなら１，DFT結果をそのまま表示するなら，２を入力\n");
    printf("4-1c.両方とも出力する場合は，1,2以外を入力\n");
    printf("4-2.IDFTの場合，データファイルの保存形式は，『実数,虚数』のようにカンマで区切り，jは省く\n");
    printf("5.指定したデータが表示される\n");
    printf("**************************\n");
}

double Hwindow(double data,int n,int N)
{
    return (0.54-0.46*cos(2*PI*n/N))*data;
}
