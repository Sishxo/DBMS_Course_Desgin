/* dbms1.cpp :  Database Manage System(2) -- Insert.  */

#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <string.h>
#include <iostream>
using namespace std;

#define MAX_SIZE 50		/*���������ֶ���󳤶�*/
#define FILE_NAME_LENGTH 15		/*�ļ�����󳤶�*/
#define FIELD_NAME_LENGTH 15	/*�ֶ�����󳤶�*/

typedef struct {
    char sFieldName[FIELD_NAME_LENGTH];//�ֶ���
    char sType[8];//����
    int iSize;//�ֳ�
    char bKey;//���ֶ��Ƿ�Ϊkey��
    char bNullFlag;//���ֶ��Ƿ�����Ϊ��
    char bValidFlag;//���ֶ��Ƿ���Ч���������Ժ�Ա��и��ֶε�ɾ��
}TableMode,*PTableMode;

int type(char *s){
    int i,j;
    char ch,*tmp;
    tmp=s;
    ch=*tmp;
    i=j=0;
    while(ch!='\0'){
        if(ch>='0'&& ch<='9'){
            j++;
        }
        tmp=tmp+1;
        i++;
        ch=*tmp;
    }
    if(i==j)return 1;
    else return 0;
}

int range(char *s){
    char ch,*tmp=s;
    int cnt;
    if(type(s)==1){
        if(s<"-2147483648"&&s>"2147483647")return -1;
        else return 0;
    }else{
        ch=*s;
        cnt=0;
        while(ch!='\0'){
            cnt++;
            s=s+1;
            ch=*s;
        }
        return cnt;
    }
}

int main(){
    TableMode tempTM;
    TableMode FieldSet[MAX_SIZE];
    FILE *fp;
    int tempi,iNum,i=0;  /*iNum is the number of the Field���ֶ���������*/
    char tempc;
    char dbf[FILE_NAME_LENGTH],tname[FILE_NAME_LENGTH],temptn[FILE_NAME_LENGTH],dat[FILE_NAME_LENGTH];
    int bFound;  /*found the table */
    char flag;

    strcpy(dbf,"school.dbf");
    if((fp=fopen(dbf,"rb")) == NULL ){
       printf("\nFile Not Exist or Bad File!\n" );
    }
    cout<<"tname:";
    cin>>tname;
    while(!feof(fp)){//��λ������ƥ��ı������¼
        bFound=0;
        tempi=fread(&tempc,sizeof(char),1,fp);
        if(!tempi)break;
        fread(temptn,sizeof(char),FILE_NAME_LENGTH,fp);//��ȡ����
        fread(&iNum,sizeof(int),1,fp);//��ȡ�ֶε�����
        fread(FieldSet,sizeof(TableMode),iNum,fp);//��ȡ�����ֶνṹ
        if(strcmp(tname,temptn)==0){//��λ
            break;  /*break while*/
        }
    }
    fclose(fp);

    strcpy(dat,"school.dat");
    if((fp=fopen(dat,"ab+"))==NULL){
        printf("open or create file error!");
        exit(1);
    }
    fwrite("~",sizeof(char),1,fp);//�ָ���
    fwrite(tname,sizeof(char),FILE_NAME_LENGTH,fp);//д�����
    fwrite(&i,sizeof(int),1,fp);//�˱���������¼������
    fwrite(&iNum,sizeof(int),1,fp);//��¼���ֶ�����
    fwrite("y",sizeof(char),1,fp);//��Чλ
    printf("FILE p offset:%d\n", (int)ftell(fp));
    fclose(fp);

    return 0;
}


