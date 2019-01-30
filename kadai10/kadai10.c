//4J02池口恭司

#include <stdio.h> 
#include <stdlib.h>
#include <time.h>
#include <math.h>

double gauss(void);	//白色ガウス性雑音発生関数
double colored(void);	//有色信号発生関数(1次IIRフィルタ利用)
void intro(void);

//メイン
int main()
{
    double *rawxn;      //生データ
	double *xn;			//入力信号ベクトル
	double *hn;			//推定システム
	double *wn;			//未知システム

	double d,y,e,xnorm,error;
	int i,j;

    FILE *fpA,*fpB,*fpC;
    int dataSize,filtSize;
    double tmp,step;

	srand((unsigned)time(NULL));

    //intro();
	//入力データが入ったファイルをオープン
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

	//インパルス応答をファイルから読み込む．数もカウントnとする
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

	//保存用ファイルのオープン
    fpC = fopen("result.csv","w");
    fseek(fpB,0L,SEEK_SET);
    if(fpC==NULL){
        printf("cannot open writefile\n");
        return -1;
    }
    
    //フィルタ係数用の配列を確保
    hn = (double *)calloc(filtSize,sizeof(double));

    fprintf(fpC,"番号,元信号,観測信号d(n),疑似エコーy(n),誤差,誤差10log10(e^2)\n");
    printf("番号,元信号,観測信号d(n),疑似エコーy(n),誤差,誤差10log10(e^2)\n");
	//ここからグラフの横軸に相当する，サンプル回の繰り返し
    step = 1.0;

    for(j=0;j<dataSize;j++){
		//xnの更新
    	for(i=filtSize-1;i>0;i--)xn[i]=xn[i-1];//入力ベクトルを生成
		//xn[0] = rawxn[j];
        xn[0]=gauss();
		//xn[0]=colored();
	
    	//所望信号dの計算（firフィルタによるフィルタリング） d = xnとwnの内積
        d = 0;
        for(i=0;i<filtSize;i++){
            d += xn[i] * wn[i];
            //if(j==100)printf("%d %lf %lf %lf\n",i,xn[i],wn[i],d);
        }

		//フィルタからの出力信号yの計算（方法はdと同様）
        y = 0;
        for(i=0;i<filtSize;i++){
            y += xn[i] * hn[i];
            //if(j==5)printf("%d %lf %lf %lf\n",i,xn[i],hn[i],y);
        }


		//誤差信号eの計算
        if(d-y==0);
        else e = d - y;
        //入力信号ベクトルxnのノルムの二乗||xn||^2の計算(xn同士の内積)
        xnorm = 0;
        for(i=0;i<filtSize;i++){
            xnorm += xn[i] * xn[i];
        }

		//フィルタ係数のhnの計算
        for(i=0;i<filtSize;i++){
            hn[i]= hn[i] + step*xn[i]*e/(xnorm+0.000001);//フィルタ係数の更新
        }

		//アルゴリズムの評価に使うeの2乗を計算
        error = e * e;        

		//評価に使うデータ(eなど)をファイルへ1つずつ保存
        fprintf(fpC,"%d,%lf,%lf,%lf,%lf,%lf\n",j,xn[0],d,y,e,10*log10(error));
        printf("%d,%lf,%lf,%lf,%lf,%lf\n",j,xn[0],d,y,e,10*log10(error));
    }

}

double gauss(void)//正規乱数
{
    double x1,x2,gauss,sd=1.0;//sdは分散
    double pi=3.14159265358979;

    x1=(double)rand()/(RAND_MAX);//0-1の一様乱数１
	while(x1 == 0.0){
		x1=(double)rand()/(RAND_MAX);
	}
    x2=(double)rand()/(RAND_MAX);//0-1の一様乱数２
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
    printf("4J02 池口恭司 課題10\n");
}
