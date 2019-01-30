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

	double d,y,e,xnorm;
	int i,j,sample;

    FILE *fpA,*fpB,*fpC;
    int dataSize,filtSize;
    double tmp,step;

	srand((unsigned)time(NULL));

    //intro();
	//入力データが入ったファイルをオープン
    fpA = fopen("ara11_s.txt","r");
    if(fpA == NULL){
        printf("ファイルオープンエラー\n");
        return -1;
    }

    dataSize = 10000;
    rawxn = (double *)malloc(dataSize*sizeof(double));
    fseek(fpA,0L,SEEK_SET);
    for(i=0;i<dataSize;i++){
        fscanf(fpA,"%lf",&rawxn[i]);
    }
    fclose(fpA);

	//インパルス応答をファイルから読み込む．数もカウントnとする
    fpB = fopen("w_imp50.txt","r");
    if(fpB==NULL){
        printf("ファイルオープンエラー\n");
        return -1;
    }

    filtSize=50;
    wn = (double *)malloc(filtSize*sizeof(double));
    fseek(fpB,0L,SEEK_SET);
    for(i=0;i<filtSize;i++){
        fscanf(fpB,"%lf\n",&wn[i]);
        printf("filtSize : %d  ---- %lf\n",i,wn[i]);
    }
    fclose(fpB);

	//保存用ファイルのオープン
    fpC = fopen("result.csv","w");
    fseek(fpB,0L,SEEK_SET);
    if(fpC==NULL){
        printf("ファイルオープンエラー\n");
        return -1;
    }
    
    //フィルタ係数用の配列を確保
    hn = (double *)calloc(filtSize,sizeof(double));

    fprintf(fpC,"番号,誤差10log10(e^2),白色信号");
    printf("番号,誤差10log10(e^2),白色信号");
	//ここからグラフの横軸に相当する，サンプル回の繰り返し
    for(j=0;j<dataSize;j++){
		//xnの更新
		//ファイルから読むものについては，1サンプルごとに読み込み
        for(i=filtSize-1;i>0;i--) xn[i]=xn[i-1];//入力ベクトルを生成
		xn[0]=gauss();
		
		//所望信号dの計算（firフィルタによるフィルタリング） d = xnとwnの内積
        d = 0;
        for(i=0;i<filtSize;i++){
            d += xn[i] * wn[i];
        }

		//フィルタからの出力信号yの計算（方法はdと同様）
        y = 0;
        for(i=0;i<filtSize;i++){
            y += xn[i] * hn[i];
        }


		//誤差信号eの計算
        e = d - y;

		//入力信号ベクトルxnのノルムの二乗||xn||^2の計算(xn同士の内積)
        xnorm = 0;
        for(i=0;i<filtSize;i++){
            y += xn[i] * xn[i];
        }

		//フィルタ係数のhnの計算
        step = 1/xnorm;
        for(i=0;i<filtSize;i++){
            hn[i]+=step*xn[i]*e/(xnorm+0.000001);//フィルタ係数の更新
        }

		//アルゴリズムの評価に使うeの2乗を計算
        e = e * e;        

		//評価に使うデータ(eなど)をファイルへ1つずつ保存
        fprintf(fpC,"%d,%lf,%lf",j,10*log10(e),xn[0]);
        printf("%d,%lf,%lf",j,10*log10(e),xn[0]);
	//サンプルループここまで
    }

}

////////////////////////////////////////////////////////////////////
//白色ガウス性雑音作成関数
double gauss(void)//正規乱数（ボックスミュラー法を利用）
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
////////////////////////////////////////////////////////////////////
//１次IIRフィルタによる有色信号作成関数
double colored(void)
{
	double color;
	static double oldcolor=0.0;
	color = gauss() + 0.95*oldcolor;
	oldcolor = color;
	return color;
}

void intro(void){

}
