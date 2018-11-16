//2018�N �ۑ�5 4J02 �r�����i
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#define PI M_PI
struct complex{
    double re;
    double im;
};
void intro(void);
void FFT(struct complex *xn,struct complex *XK,int N);
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
    FFT(xn,XK,N);
    for(i=0;i<N;i++){
        printf("X%d=%lf+(%lfj)\n",i,xn[i].re,XK[i].im);
    }
   fclose(fpIn);
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

printf("��]�s��\n");
for(i=0;i<N;i++){
printf("%lf   ",wnk[i].re);
printf("%lf\n",wnk[i].im);
}
printf("�ǂݎ�����s��\n");
for(i=0;i<N;i++){
printf("%lf   ",xn[i].re);
printf("%lf\n",xn[i].im);
}
    arrayReverse(xn,N);                   //���̎��_��xn�̓r�b�g�t���ɕ���
printf("��]��");
for(i=0;i<N;i++){
printf("%lf   ",xn[i].re);
printf("%lf\n",xn[i].im);
}
    now=numOfArray/2;
    for(i=0,shift=1;i<numOfBit;i++){            //�i�����̌J��Ԃ�����
        for(j=0;j<shift;j++){       //1�i������̃o�^�t���C���Z�����̌J��Ԃ�������������
            for(z=0;z<now;z++){       //�����܂ł�1�i��
            //printf("******\n");
            //printf("[%d]=[%d]+[%d]*[%d]\n",now+z,j,j+shift,((i*shift)%((numOfArray-1))*j)%(numOfCalc));
            //printf("[%d]=[%d]-[%d]*[%d]\n",now+z+shift,j,j+shift,((i*shift)%((numOfArray-1))*j)%(numOfCalc));
                tmp=calcPro(xn[shift*2*z+j+shift],wnk[j*now]);
                xn[shift*2*z+j+shift]=calcDif(xn[shift*2*z+j],tmp);      //��i�v�Z
                xn[shift*2*z+j]=calcSum(xn[shift*2*z+j],tmp);    //���i�v�Z
            //    XK[now+z]=calcSum(xn[j],calcPro(xn[j+shift],wnk[(((i*shift)%(numOfArray-1))*j)%(numOfCalc)]));      //��i�v�Z
            //    XK[now+shift+z]=calcSum(xn[j],calcPro(xn[j+shift],wnk[(((i*shift)%numOfArray-1)*j)%numOfCalc]));    //���i�v�Z
                //XK[now+shift+z]=calcSum(xn[j],xn[j+shift]*wnk[(((i*shift)%7)*j)%4]);    //���i�v�Z
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


void intro(void)
{
    printf("2018�N �ۑ�5 4J02 �r�����i\n");
    printf("�g�����F\n");
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
for(i=0;i<N;i++){
printf("bitOrder:%d bitArray:%d %d\n",bitOrder[i],bitArray[i].re,bitArray[i].im);
}
    for(i=0;i<eltValue;i++){
        tmpArray[bitOrder[i]]=bitArray[i];
    }
    for(i=0;i<eltValue;i++){
        bitArray[i]=tmpArray[i];
    }
for(i=0;i<N;i++){
printf("order:%d bitArray:%d %d\n",i,bitArray[i].re,bitArray[i].im);
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