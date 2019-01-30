#include <stdio.h> 
#include <stdlib.h>
#include <time.h>
#include <math.h>

double gauss(void);	//白色ガウス性雑音発生関数
double colored(void);	//有色信号発生関数(1次IIRフィルタ利用)
void intro(void);

#define fileRead(fileName,fileStream) fileStream=fopen(fileName,"r");if(fileStream==NULL){printf("cannat read file[%s]",fileName);exit(1);}
#define fileWrite(fileName,fileStream) fileStream=fopen(fileName,"w");if(fileStream==NULL){printf("cannat write file[%s]",fileName);exit(1);}


//メイン　
int main()
{
    double *rawxn;      //生データ
	double *xn;			//入力信号ベクトル
	double *hn;			//推定システム
	double *wn;			//未知システム

	double d,y,e,xnorm;
	int i,j,sample;

    FILE *fp;
    int dataSize,filtSize;
    double tmp;
    char fileName[24];

	srand((unsigned)time(NULL));

    intro();
	//入力データが入ったファイルをオープン
    printf("入力データファイル名を入力してください(txt)：");
    scanf("%s",fileName);
    fileRead(fileName,fp);
    dataSize=0;
    while(fscanf(fp,"%lf",tmp) != EOF)dataSize++;
    rawxn = (double *)malloc(dataSize*sizeof(double));
    rewind(fp);
    while(fscanf(fp,"%lf",rawxn[i]) != EOF)i++;
    fclose(fp);

	//インパルス応答をファイルから読み込む．数もカウントnとする
    printf("インパルス応答(未知システム)ファイル名を入力してください(txt)：");
    scanf("%s",fileName);
    fileRead(fileName,fp);
    filtSize=0;
    while(fscanf(fp,"%lf",tmp) != EOF)filtSize++;
    wn = (double *)malloc(filtSize*sizeof(double));
    rewind(fp);
    while(fscanf(fp,"%lf",wn[i]) != EOF)i++;
    fclose(fp);

	//保存用ファイルのオープン
    printf("保存先ファイル名を入力してください(csv)：");
    scanf("%s",fileName);
    fileWrite(fileName,fp); //開いたまま

    //フィルタ係数用の配列を確保
    hn = (double *)calloc(filtSize,sizeof(double));

    fprintf(fp,"番号,誤差10log10(e^2),白色信号");
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
        for(i=0;i<filtSize;i++){
            hn[i]+=step*xn[i]*e/(xnorm+0.000001);//フィルタ係数の更新
        }

		//アルゴリズムの評価に使うeの2乗を計算
        e = e * e;        

		//評価に使うデータ(eなど)をファイルへ1つずつ保存
        fprintf(fp,"%d,%lf,%lf",j,10*log10(e),x[0]);

	//サンプルループここまで
    }
}

double gauss(void){
    

}
double colored(void){

}

void intro(void){

}