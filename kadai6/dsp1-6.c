#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void printChunkRIFF(void);
void printChunkFMT(void);

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

riff_chunk riff;
fmt_chunk fmt;

void main(){
    FILE *fp;
    char fname[]="ara11.wav";
    fp = fopen(fname,"r");
    if(fp==NULL){
        printf("cannot open %s",fname);
        exit(1);
    }

    fseek(fp,0,SEEK_SET);   //ストリームをファイルの先頭に持ってくる
    fread(&riff.id,1,4,fp);
    fread(&riff.size,4,1,fp);
    fread(&riff.form,1,4,fp);
    fread(&fmt.id,1,4,fp);
    fread(&fmt.size,4,1,fp);
    fread(&fmt.format_id,2,1,fp);
    fread(&fmt.channel,2,1,fp);
    fread(&fmt.fs,4,1,fp);
    fread(&fmt.byte_sec,4,1,fp);
    fread(&fmt.byte_samp,2,1,fp);
    fread(&fmt.bit,2,1,fp);
    
    /*
    printf("filesize : %d\n",(int)riff.size);
    printf("channel : %d\n",(int)fmt.channel);
    printf("fs : %d\n",(int)fmt.fs);
    printf("bit : %d\n",(int)fmt.bit);
    */
    printChunkRIFF();
    printChunkFMT();
}

void printChunkRIFF(){
    char id[5],form[5];
    sprintf(id,"%s",riff.id);
    sprintf(form,"%s",riff.form);
    printf("----- RIFF -----\n");
    printf("ID:%s\n", riff.id);
    printf("FileSize:%d\n",(int)riff.size);
    printf("form:%s\n",riff.form);
}
void printChunkFMT(){
    char id[5];
    sprintf(id,"%s",fmt.id);
    printf("----- FMT -----\n");
    printf("ID:%s\n",fmt.id);
    printf("FMTSize:%d\n",(int)fmt.size);
    printf("formatID:%d\n",(int)fmt.format_id);
    printf("channel:%d\n",(int)fmt.channel);
    printf("SamplingHz:%d\n",(int)fmt.fs);
    printf("byte/sec:%d\n",(int)fmt.byte_sec);
    printf("byte/ele:%d\n",(int)fmt.byte_samp);
    printf("bit:%d\n",(int)fmt.bit);
}