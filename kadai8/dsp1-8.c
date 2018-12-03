//2018年 課題6 4J02 池口恭司
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

FILE *readFP(char fileName[]);
FILE *writeFP(char fileName[]);
FILE *readBinaryFP(char fileName[]);
FILE *writeBinaryFP(char fileName[]);
void readChunkRIFF(FILE *fp);
void readChunkFMT(FILE *fp);
void readDataFromWave(FILE *fpIN,FILE *fpOUT,FILE *fpFL);
void readDataFromText(FILE *fpIN,FILE *fpOUT,FILE *fpFL);
void textToWave(FILE *fpIN,FILE *fpOUT);
void intro(void);
void printChunkRIFF(void);
void printChunkFMT(void);
void printForDrill(void);
void filtering(double *rawData,double *h,int rawDataSize,int hFilterSize,FILE *fpOUT);


//RIFFチャンク、WAVEフォームタイプ構造体
typedef struct{
    char id[4];             //"RIFF"
    unsigned long size;     //ファイルサイズ-8バイト
    char form[4];           //"WAVE"
}riff_chunk;

//fmtチャンク構造体
typedef struct{
    char id[4];                 //"fmt "スペース含めて4文字
    unsigned long size;         //fmt領域のサイズ
    unsigned short format_id;   //フォーマットID(PCM:1)
    unsigned short channel;     //チャンネル数（モノラル:1 ステレオ:2）
    unsigned long fs;           //サンプリング周波数
    unsigned long byte_sec;     //1秒あたりのバイト数（fs * byte_samp）/平均データ速度
    unsigned short byte_samp;   //1要素あたりのバイト数（channel * (bit/8)）
    unsigned short bit;         //量子化ビット数(8 or 16)
}fmt_chunk;

//dataチャンク構造体
typedef struct{
    char id[4];
    unsigned long size;
}data_chunk;

//factチャンク構造体
typedef struct{
	char id[4];			// "fact"
	unsigned long size;		// factデータ領域のサイズ (4)
	unsigned long sample;		// 全データ数
} fact_chunk;

riff_chunk riff;
fmt_chunk fmt;
fact_chunk fact;
data_chunk data;

/************* main *************/
void main(){
    FILE *rData,*rFilt,*w;
    FILE *read,*write;
    int mode;
    char fileName[24],fileNameTxt[24];
    intro();
    printf("モード選択");
    printf("読み取りファイル形式：wav->1 text->2\n");
    scanf("%d",&mode);
    printf("フィルタリングファイル名.txt：");
    scanf("%s",fileName);
    rFilt=readFP(fileName);
    printf("データファイル名.拡張子(txt or wav)：");
    scanf("%s",fileName);
    printf("出力txtファイル名.txt：");
    scanf("%s",fileNameTxt);
    w=writeFP(fileNameTxt);
    if(mode==1){
        rData=readBinaryFP(fileName);
        readDataFromWave(rData,w,rFilt);
    }else if(mode==2){
        rData=readFP(fileName);
        readDataFromText(rData,w,rFilt);
    }else{
        printf("モード選択が正しくされていません\n");
        exit(0);
    }
    read=readFP(fileNameTxt);
    printf("出力wavファイル名.wav：");
    scanf("%s",fileName);
    write=writeBinaryFP(fileName);
    textToWave(read,write);
}
/********************************/

/************* Functions *************/
FILE *readFP(char fileName[])
{
    FILE *fp;
    fp = fopen(fileName,"r");
    if(fp==NULL){
        printf("Error : Cannot Read File[%s]\n",fileName);
        exit(1);
    }
    return fp;
}

FILE *writeFP(char fileName[])
{
    FILE *fp;
    fp = fopen(fileName,"w");
    if(fp==NULL){
        printf("Error : Cannot Write File[%s]\n",fileName);
        exit(1);
    }
    return fp;
}
FILE *readBinaryFP(char fileName[])
{
    FILE *fp;
    fp = fopen(fileName,"rb");
    if(fp==NULL){
        printf("Error : Cannot Read File[%s]\n",fileName);
        exit(1);
    }
    return fp;
}

FILE *writeBinaryFP(char fileName[])
{
    FILE *fp;
    fp = fopen(fileName,"wb");
    if(fp==NULL){
        printf("Error : Cannot Write File[%s]\n",fileName);
        exit(1);
    }
    return fp;
}

void readDataFromWave(FILE *fpIN,FILE *fpOUT,FILE *fpFL)
{
    char buff[5];
    short short_data;
    double *rawData,*h;
    unsigned long i;
    int sizeOfRaw=0,sizeOfFil=0,j;
    int dataSize;
//生データ読み取り
	fseek(fpIN,0,SEEK_SET);			//ファイル読み込み位置をファイルの先頭バイトへ
	fread(&riff, sizeof(riff_chunk),1,fpIN);//RIFFチャンクWAVEフォームを読み込む
	fread(&fmt, sizeof(fmt_chunk),1,fpIN);	//fmtチャンクを読み込む
	fseek(fpIN,20+fmt.size,SEEK_SET);	//ファイル読み込み位置を次のチャンクの先頭へ
	fread(buff, sizeof(char),4,fpIN);	//チャンクIDを読み込みfactかdataかチェック
	fseek(fpIN,20+fmt.size,SEEK_SET);	//ファイル読み込み位置を次のチャンクの先頭へ再セット
	buff[4]='\0';
	if(strcmp(buff,"fact")==0){				//IDがfactなら
		fread(&fact, sizeof(fact_chunk),1,fpIN);	//fmtチャンクを読み込む
	}
	fread(&data, sizeof(data_chunk),1,fpIN);//dataチャンクを読み込む
	fact.sample = data.size/(fmt.bit/8);	// 全サンプル数
	rawData=(double *)calloc(fact.sample,sizeof(double));
	for(i=0;i<fact.sample;i++){
		fread(&short_data,sizeof(short),1,fpIN);
		rawData[i]=short_data;
	}
    printf("読み取ったデータ数[%ld個]\n",fact.sample);
/*	for(i=0;i<fact.sample;i++){
        printf("%lf\n",rawData[i]);
    }
*/    fclose(fpIN);
    sizeOfRaw=fact.sample;
//フィルタデータ読み取り
    while(fgets(buff,256,fpFL)!=NULL)sizeOfFil++;
	if((h=(double *)calloc(sizeOfFil,sizeof(double)))==NULL){
        printf("配列が確保できませんでした\n");
        exit(0);
    }
    fseek(fpFL,0,SEEK_SET);
    printf("デジタルフィルタの係数データ[%d個]\n",sizeOfFil);
    for(j=0;j<sizeOfFil;j++){
        fscanf(fpFL,"%lf",&h[j]);
//        printf("%lf ",h[j]);
    }
//    printf("\n");
    fclose(fpFL);
//データ処理
    filtering(rawData,h,sizeOfRaw,sizeOfFil,fpOUT);   
    fclose(fpOUT); 
}

void readDataFromText(FILE *fpIN,FILE *fpOUT,FILE *fpFL)
{
    int sizeOfRaw=0,sizeOfFil=0,j,i;
    double *rawData,*h,*result;
    char buff[256];
    while(fgets(buff,256,fpIN)!=NULL)sizeOfRaw++;
	if((rawData=(double *)calloc(sizeOfRaw,sizeof(double)))==NULL){
        printf("配列が確保できませんでした\n");
        exit(0);
    }
    fseek(fpIN,0,SEEK_SET);
    printf("読み取ったデータ[%d個]\n",sizeOfRaw);
    for(i=0;i<sizeOfRaw;i++){
        fscanf(fpIN,"%lf",&rawData[i]);
        printf("%lf\n",rawData[i]);
    }
    fclose(fpIN);
//フィルタデータ読み取り
    while(fgets(buff,256,fpFL)!=NULL)sizeOfFil++;
	if((h=(double *)calloc(sizeOfFil,sizeof(double)))==NULL){
        printf("配列が確保できませんでした\n");
        exit(0);
    }
    fseek(fpFL,0,SEEK_SET);
    printf("デジタルフィルタの係数データ[%d個]：",sizeOfFil);
    for(j=0;j<sizeOfFil;j++){
        fscanf(fpFL,"%lf",&h[j]);
        printf("%lf ",h[j]);
    }
    printf("\n");
    fclose(fpFL);
//データ処理
    filtering(rawData,h,sizeOfRaw,sizeOfFil,fpOUT);    
    fclose(fpOUT);
}

void textToWave(FILE *fpIN,FILE *fpOUT)
{
    short short_data;
    char buff[256];
    unsigned long i;
    double *pcm_data;
    fact.sample=0;
    while(fgets(buff,256,fpIN)!=NULL)fact.sample++;
	if((pcm_data=(double *)calloc(fact.sample,sizeof(double)))==NULL){
        printf("配列が確保できませんでした\n");
        exit(0);
    }
    fseek(fpIN,0,SEEK_SET);
    for(i=0;i<fact.sample;i++)fscanf(fpIN,"%lf",&pcm_data[i]);
    strcpy(riff.id,"RIFF");
    strcpy(riff.form,"WAVE");
    strcpy(fmt.id,"fmt ");
    strcpy(data.id,"data");
    fmt.size=16;
    fmt.format_id=1;
    fmt.channel=1;
    fmt.fs=11025;
    fmt.byte_sec=22050;
    fmt.byte_samp=2;
    fmt.bit=16;
    data.size=fact.sample*fmt.byte_samp;
    riff.size=44+data.size-8;
    fwrite(&riff,sizeof(riff_chunk),1,fpOUT);
    fwrite(&fmt,sizeof(fmt_chunk),1,fpOUT);
    fwrite(&data,sizeof(data_chunk),1,fpOUT);
    for(i=0;i<fact.sample;i++){
        short_data=(short)pcm_data[i];
        fwrite(&short_data,sizeof(short),1,fpOUT);
    }
    free(pcm_data);
    pcm_data=NULL;
    fclose(fpIN);
}


/************* Display Functions *************/
void intro()
{
    printf("2018年 課題8 4J02 池口恭司\n");
    printf("使い方：\n");
    printf("１．読み取るファイルの形式を選択する【txt/wav】\n");
    printf("２．フィルタリング係数を保存したファイル名，データを保存したファイル名及び，出力するファイル名を入力する\n");
    printf("※フィルタリング係数は，ファイルから直接読み取るため，遅延やエコーなどの設定は読み取るファイルの方で設定を行う\n");
    printf("※本プログラムでは，txtファイル出力後，wavファイル出力を行う\n");
    printf("**************************\n");
}

void printChunkRIFF()
{
    char id[5],form[5];
    sprintf(id,"%s",riff.id);
    sprintf(form,"%s",riff.form);
    id[4]='\0';
    form[4]='\0';
    printf("----- RIFF -----\n");
    printf("ID:%s\n", id);
    printf("FileSize:%lu\n",riff.size);
    printf("form:%s\n",form);
}

void printChunkFMT()
{
    char id[5];
    sprintf(id,"%s",fmt.id);
    id[4]='\0';
    printf("----- FMT -----\n");
    printf("ID:%s\n",id);
    printf("FMTSize:%lu\n",fmt.size);
    printf("formatID:%u\n",fmt.format_id);
    printf("channel:%u\n",fmt.channel);
    printf("SamplingHz:%lu\n",fmt.fs);
    printf("byte/sec:%lu\n",fmt.byte_sec);
    printf("byte/ele:%u\n",fmt.byte_samp);
    printf("bit:%u\n",fmt.bit);
}

/************* Kadai1-8 Additional Functions *************/
void filtering(double *rawData,double *h,int rawDataSize,int hFilterSize,FILE *fpOUT)
{
    int i,j;
    double *result;
    double *inputData;
    //printf("rawDataSize[%d個] hFilterSize[%d個]\n",rawDataSize,hFilterSize);
    if((inputData=calloc(hFilterSize,sizeof(double)))==NULL){
        printf("配列が確保できませんでした\n");
        exit(0);
    }
    if((result=calloc(rawDataSize,sizeof(double)))==NULL){
        printf("配列が確保できませんでした\n");
        exit(0);
    }
    printf("フィルタ後のデータ[%d個]\n",rawDataSize);
    for(i=0;i<rawDataSize;i++){
        for(j=hFilterSize-1;j>0;j--){
            inputData[j]=inputData[j-1];
        }
        inputData[0]=rawData[i];
        for(j=0;j<hFilterSize;j++){
            result[i]+=h[j]*inputData[j];
        }
//        printf("%lf\n",result[i]);
        fprintf(fpOUT,"%lf\n",result[i]);
    }
}

