//2018�N �ۑ�6 4J02 �r�����i
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


//RIFF�`�����N�AWAVE�t�H�[���^�C�v�\����
typedef struct{
    char id[4];             //"RIFF"
    unsigned long size;     //�t�@�C���T�C�Y-8�o�C�g
    char form[4];           //"WAVE"
}riff_chunk;

//fmt�`�����N�\����
typedef struct{
    char id[4];                 //"fmt "�X�y�[�X�܂߂�4����
    unsigned long size;         //fmt�̈�̃T�C�Y
    unsigned short format_id;   //�t�H�[�}�b�gID(PCM:1)
    unsigned short channel;     //�`�����l�����i���m����:1 �X�e���I:2�j
    unsigned long fs;           //�T���v�����O���g��
    unsigned long byte_sec;     //1�b������̃o�C�g���ifs * byte_samp�j/���σf�[�^���x
    unsigned short byte_samp;   //1�v�f������̃o�C�g���ichannel * (bit/8)�j
    unsigned short bit;         //�ʎq���r�b�g��(8 or 16)
}fmt_chunk;

//data�`�����N�\����
typedef struct{
    char id[4];
    unsigned long size;
}data_chunk;

//fact�`�����N�\����
typedef struct{
	char id[4];			// "fact"
	unsigned long size;		// fact�f�[�^�̈�̃T�C�Y (4)
	unsigned long sample;		// �S�f�[�^��
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
    printf("���[�h�I��");
    printf("�ǂݎ��t�@�C���`���Fwav->1 text->2\n");
    scanf("%d",&mode);
    printf("�t�B���^�����O�t�@�C����.txt�F");
    scanf("%s",fileName);
    rFilt=readFP(fileName);
    printf("�f�[�^�t�@�C����.�g���q(txt or wav)�F");
    scanf("%s",fileName);
    printf("�o��txt�t�@�C����.txt�F");
    scanf("%s",fileNameTxt);
    w=writeFP(fileNameTxt);
    if(mode==1){
        rData=readBinaryFP(fileName);
        readDataFromWave(rData,w,rFilt);
    }else if(mode==2){
        rData=readFP(fileName);
        readDataFromText(rData,w,rFilt);
    }else{
        printf("���[�h�I��������������Ă��܂���\n");
        exit(0);
    }
    read=readFP(fileNameTxt);
    printf("�o��wav�t�@�C����.wav�F");
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
//���f�[�^�ǂݎ��
	fseek(fpIN,0,SEEK_SET);			//�t�@�C���ǂݍ��݈ʒu���t�@�C���̐擪�o�C�g��
	fread(&riff, sizeof(riff_chunk),1,fpIN);//RIFF�`�����NWAVE�t�H�[����ǂݍ���
	fread(&fmt, sizeof(fmt_chunk),1,fpIN);	//fmt�`�����N��ǂݍ���
	fseek(fpIN,20+fmt.size,SEEK_SET);	//�t�@�C���ǂݍ��݈ʒu�����̃`�����N�̐擪��
	fread(buff, sizeof(char),4,fpIN);	//�`�����NID��ǂݍ���fact��data���`�F�b�N
	fseek(fpIN,20+fmt.size,SEEK_SET);	//�t�@�C���ǂݍ��݈ʒu�����̃`�����N�̐擪�֍ăZ�b�g
	buff[4]='\0';
	if(strcmp(buff,"fact")==0){				//ID��fact�Ȃ�
		fread(&fact, sizeof(fact_chunk),1,fpIN);	//fmt�`�����N��ǂݍ���
	}
	fread(&data, sizeof(data_chunk),1,fpIN);//data�`�����N��ǂݍ���
	fact.sample = data.size/(fmt.bit/8);	// �S�T���v����
	rawData=(double *)calloc(fact.sample,sizeof(double));
	for(i=0;i<fact.sample;i++){
		fread(&short_data,sizeof(short),1,fpIN);
		rawData[i]=short_data;
	}
    printf("�ǂݎ�����f�[�^��[%ld��]\n",fact.sample);
/*	for(i=0;i<fact.sample;i++){
        printf("%lf\n",rawData[i]);
    }
*/    fclose(fpIN);
    sizeOfRaw=fact.sample;
//�t�B���^�f�[�^�ǂݎ��
    while(fgets(buff,256,fpFL)!=NULL)sizeOfFil++;
	if((h=(double *)calloc(sizeOfFil,sizeof(double)))==NULL){
        printf("�z�񂪊m�ۂł��܂���ł���\n");
        exit(0);
    }
    fseek(fpFL,0,SEEK_SET);
    printf("�f�W�^���t�B���^�̌W���f�[�^[%d��]\n",sizeOfFil);
    for(j=0;j<sizeOfFil;j++){
        fscanf(fpFL,"%lf",&h[j]);
//        printf("%lf ",h[j]);
    }
//    printf("\n");
    fclose(fpFL);
//�f�[�^����
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
        printf("�z�񂪊m�ۂł��܂���ł���\n");
        exit(0);
    }
    fseek(fpIN,0,SEEK_SET);
    printf("�ǂݎ�����f�[�^[%d��]\n",sizeOfRaw);
    for(i=0;i<sizeOfRaw;i++){
        fscanf(fpIN,"%lf",&rawData[i]);
        printf("%lf\n",rawData[i]);
    }
    fclose(fpIN);
//�t�B���^�f�[�^�ǂݎ��
    while(fgets(buff,256,fpFL)!=NULL)sizeOfFil++;
	if((h=(double *)calloc(sizeOfFil,sizeof(double)))==NULL){
        printf("�z�񂪊m�ۂł��܂���ł���\n");
        exit(0);
    }
    fseek(fpFL,0,SEEK_SET);
    printf("�f�W�^���t�B���^�̌W���f�[�^[%d��]�F",sizeOfFil);
    for(j=0;j<sizeOfFil;j++){
        fscanf(fpFL,"%lf",&h[j]);
        printf("%lf ",h[j]);
    }
    printf("\n");
    fclose(fpFL);
//�f�[�^����
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
        printf("�z�񂪊m�ۂł��܂���ł���\n");
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
    printf("2018�N �ۑ�8 4J02 �r�����i\n");
    printf("�g�����F\n");
    printf("�P�D�ǂݎ��t�@�C���̌`����I������ytxt/wav�z\n");
    printf("�Q�D�t�B���^�����O�W����ۑ������t�@�C�����C�f�[�^��ۑ������t�@�C�����y�сC�o�͂���t�@�C��������͂���\n");
    printf("���t�B���^�����O�W���́C�t�@�C�����璼�ړǂݎ�邽�߁C�x����G�R�[�Ȃǂ̐ݒ�͓ǂݎ��t�@�C���̕��Őݒ���s��\n");
    printf("���{�v���O�����ł́Ctxt�t�@�C���o�͌�Cwav�t�@�C���o�͂��s��\n");
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
    //printf("rawDataSize[%d��] hFilterSize[%d��]\n",rawDataSize,hFilterSize);
    if((inputData=calloc(hFilterSize,sizeof(double)))==NULL){
        printf("�z�񂪊m�ۂł��܂���ł���\n");
        exit(0);
    }
    if((result=calloc(rawDataSize,sizeof(double)))==NULL){
        printf("�z�񂪊m�ۂł��܂���ł���\n");
        exit(0);
    }
    printf("�t�B���^��̃f�[�^[%d��]\n",rawDataSize);
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

