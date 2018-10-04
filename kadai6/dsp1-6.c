#include <stdio.h>
#include <stdlib.h>
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
    fread(&riff.id,sizeof(riff.id[0]),sizeof(riff.id),fp);
    fread(&riff.size,sizeof(riff.size),1,fp);
    fread(&riff.form,sizeof(riff.form[0]),sizeof(riff.form),fp);
    fread(&fmt.id,sizeof(fmt.id[0]),sizeof(fmt.id),fp);
    fread(&fmt.size,sizeof(fmt.size),1,fp);
    fread(&fmt.format_id,sizeof(fmt.format_id),1,fp);
    fread(&fmt.channel,sizeof(fmt.channel),1,fp);
    fread(&fmt.fs,sizeof(fmt.fs),1,fp);
    fread(&fmt.byte_sec,sizeof(fmt.byte_sec),1,fp);
    fread(&fmt.byte_samp,sizeof(fmt.byte_samp),1,fp);
    fread(&fmt.bit,sizeof(fmt.bit),1,fp);
    printf("filesize : %d\n",(int)riff.size);
    printf("channel : %d\n",(int)fmt.channel);
    printf("fs : %d\n",(int)fmt.fs);
    printf("bit : %d\n",(int)fmt.bit);

    
}

