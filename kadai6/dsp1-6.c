#include <stdio.h>
#include <stdlib.h>
#include <string.h>

FILE *readFP(char fileName[]);
FILE *writeFP(char fileName[]);
FILE *readBinaryFP(char fileName[]);
FILE *writeBinaryFP(char fileName[]);
void readChunkRIFF(FILE *fp);
void readChunkFMT(FILE *fp);
void readDatas(FILE *fpIN,FILE *fpOUT,double *pcm_data);
void textToWave(FILE *fpIN,FILE *fpOUT,double *pcm_data);
void printChunkRIFF(void);
void printChunkFMT(void);
void printForDrill(void);

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
    FILE *r,*rb,*w,*wb;
    double *pcm_data=NULL;
    rb = readBinaryFP("ara.wav");
    w = writeFP("ara.txt");
    readDatas(rb,w,pcm_data);
    fclose(rb);
    fclose(w);
    r = readFP("ara.txt");
    wb = writeBinaryFP("araOUT.wav");
    printForDrill();
    textToWave(r,wb,pcm_data);
    fclose(r);
    fclose(wb);
    printForDrill();
}
/********************************/

/************* Functions *************/
FILE *readFP(char fileName[]){
    FILE *fp;
    fp = fopen(fileName,"r");
    if(fp==NULL){
        printf("Error : Cannot Read File[%s]\n",fileName);
        exit(1);
    }
    return fp;
}

FILE *writeFP(char fileName[]){
    FILE *fp;
    fp = fopen(fileName,"w");
    if(fp==NULL){
        printf("Error : Cannot Write File[%s]\n",fileName);
        exit(1);
    }
    return fp;
}
FILE *readBinaryFP(char fileName[]){
    FILE *fp;
    fp = fopen(fileName,"rb");
    if(fp==NULL){
        printf("Error : Cannot Read File[%s]\n",fileName);
        exit(1);
    }
    return fp;
}

FILE *writeBinaryFP(char fileName[]){
    FILE *fp;
    fp = fopen(fileName,"wb");
    if(fp==NULL){
        printf("Error : Cannot Write File[%s]\n",fileName);
        exit(1);
    }
    return fp;
}

//WAVE からデータを読み取り，TEXTに出力
void readDatas(FILE *fpIN,FILE *fpOUT,double *pcm_data){
    char buff[5];
    short short_data;
    unsigned long i;
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
	pcm_data=(double *)calloc(fact.sample,sizeof(double));
	for(i=0;i<fact.sample;i++){
		fread(&short_data,sizeof(short),1,fpIN);
		pcm_data[i]=short_data;
	}
	for(i=0;i<fact.sample;i++){
        fprintf(fpOUT,"%f\n",pcm_data[i]);
    }
    free(pcm_data);
    pcm_data=NULL;
}

void textToWave(FILE *fpIN,FILE *fpOUT,double *pcm_data){
    short short_data;
    char buff[256];
    unsigned long i;
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

}


/************* Display Functions *************/
void printChunkRIFF(){
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
void printChunkFMT(){
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

void printForDrill(){
    printf("読み取り結果\n");
    printf("ファイルサイズ(-8バイト)：%lu\n",riff.size);
    printf("チャンネル数：%u\n",fmt.channel);
    printf("サンプリング周波数：%lu[Hz]\n",fmt.fs);
    printf("量子化ビット数：%u[bit]\n",fmt.bit);
    printf("データ数：%lu[sample]\n",fact.sample);
    printf("録音時間：%lf[s]\n",(double)(1.0/fmt.fs)*(double)fact.sample/fmt.fs);  
}