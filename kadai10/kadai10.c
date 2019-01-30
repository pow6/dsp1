//4J02�r�����i

#include <stdio.h> 
#include <stdlib.h>
#include <time.h>
#include <math.h>

double gauss(void);	//���F�K�E�X���G�������֐�
double colored(void);	//�L�F�M�������֐�(1��IIR�t�B���^���p)
void intro(void);

//���C��
int main()
{
    double *rawxn;      //���f�[�^
	double *xn;			//���͐M���x�N�g��
	double *hn;			//����V�X�e��
	double *wn;			//���m�V�X�e��

	double d,y,e,xnorm,error;
	int i,j;

    FILE *fpA,*fpB,*fpC;
    int dataSize,filtSize;
    double tmp,step;

	srand((unsigned)time(NULL));

    //intro();
	//���̓f�[�^���������t�@�C�����I�[�v��
    dataSize = 12000;
    fpA = fopen("ara11_s.txt","r");
    if(fpA == NULL){
        printf("cannot open datafile\n");
        return -1;
    }

    rawxn = (double *)malloc(dataSize*sizeof(double));
    fseek(fpA,0L,SEEK_SET);
    for(i=0;i<dataSize;i++){
        fscanf(fpA,"%lf",&rawxn[i]);
    }
    fclose(fpA);

	//�C���p���X�������t�@�C������ǂݍ��ށD�����J�E���gn�Ƃ���
    fpB = fopen("w_imp50.txt","r");
    if(fpB==NULL){
        printf("cannot open impulsefile\n");
        return -1;
    }

    filtSize=50;
    wn = (double *)malloc(filtSize*sizeof(double));
    xn = (double *)calloc(filtSize,sizeof(double));
    fseek(fpB,0L,SEEK_SET);
    for(i=0;i<filtSize;i++){
        fscanf(fpB,"%lf\n",&wn[i]);
    }
    fclose(fpB);

	//�ۑ��p�t�@�C���̃I�[�v��
    fpC = fopen("result.csv","w");
    fseek(fpB,0L,SEEK_SET);
    if(fpC==NULL){
        printf("cannot open writefile\n");
        return -1;
    }
    
    //�t�B���^�W���p�̔z����m��
    hn = (double *)calloc(filtSize,sizeof(double));

    fprintf(fpC,"�ԍ�,���M��,�ϑ��M��d(n),�^���G�R�[y(n),�덷,�덷10log10(e^2)\n");
    printf("�ԍ�,���M��,�ϑ��M��d(n),�^���G�R�[y(n),�덷,�덷10log10(e^2)\n");
	//��������O���t�̉����ɑ�������C�T���v����̌J��Ԃ�
    step = 1.0;

    for(j=0;j<dataSize;j++){
		//xn�̍X�V
    	for(i=filtSize-1;i>0;i--)xn[i]=xn[i-1];//���̓x�N�g���𐶐�
		//xn[0] = rawxn[j];
        xn[0]=gauss();
		//xn[0]=colored();
	
    	//���]�M��d�̌v�Z�ifir�t�B���^�ɂ��t�B���^�����O�j d = xn��wn�̓���
        d = 0;
        for(i=0;i<filtSize;i++){
            d += xn[i] * wn[i];
            //if(j==100)printf("%d %lf %lf %lf\n",i,xn[i],wn[i],d);
        }

		//�t�B���^����̏o�͐M��y�̌v�Z�i���@��d�Ɠ��l�j
        y = 0;
        for(i=0;i<filtSize;i++){
            y += xn[i] * hn[i];
            //if(j==5)printf("%d %lf %lf %lf\n",i,xn[i],hn[i],y);
        }


		//�덷�M��e�̌v�Z
        if(d-y==0);
        else e = d - y;
        //���͐M���x�N�g��xn�̃m�����̓��||xn||^2�̌v�Z(xn���m�̓���)
        xnorm = 0;
        for(i=0;i<filtSize;i++){
            xnorm += xn[i] * xn[i];
        }

		//�t�B���^�W����hn�̌v�Z
        for(i=0;i<filtSize;i++){
            hn[i]= hn[i] + step*xn[i]*e/(xnorm+0.000001);//�t�B���^�W���̍X�V
        }

		//�A���S���Y���̕]���Ɏg��e��2����v�Z
        error = e * e;        

		//�]���Ɏg���f�[�^(e�Ȃ�)���t�@�C����1���ۑ�
        fprintf(fpC,"%d,%lf,%lf,%lf,%lf,%lf\n",j,xn[0],d,y,e,10*log10(error));
        printf("%d,%lf,%lf,%lf,%lf,%lf\n",j,xn[0],d,y,e,10*log10(error));
    }

}

double gauss(void)//���K����
{
    double x1,x2,gauss,sd=1.0;//sd�͕��U
    double pi=3.14159265358979;

    x1=(double)rand()/(RAND_MAX);//0-1�̈�l�����P
	while(x1 == 0.0){
		x1=(double)rand()/(RAND_MAX);
	}
    x2=(double)rand()/(RAND_MAX);//0-1�̈�l�����Q
    gauss=sqrt(-2.0*log(x1))*cos(2.0*pi*x2)*sqrt(sd);
    return gauss;
}

double colored(void)
{
	double color;
	static double oldcolor=0.0;
	color = gauss() + 0.95*oldcolor;
	oldcolor = color;
	return color;
}

void intro(void){
    printf("4J02 �r�����i �ۑ�10\n");
}
