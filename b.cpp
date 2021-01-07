#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <string.h>
#include <iostream>
using namespace std;

#define MAX_SIZE 100		/*���������ֶ���󳤶�*/
#define FILE_NAME_LENGTH 15		/*�ļ�����󳤶�*/
#define FIELD_NAME_LENGTH 15	/*�ֶ�����󳤶�*/
#define MAX_SQL 1000 /*SQL�����󳤶�*/

typedef struct {
  char sFieldName[FIELD_NAME_LENGTH];//�ֶ���
  char sType[8];//����
  int iSize;//�ֳ�
  char bKey;//���ֶ��Ƿ�Ϊkey��
  char bNullFlag;//���ֶ��Ƿ�����Ϊ��
  char bValidFlag;//���ֶ��Ƿ���Ч���������Ժ�Ա��и��ֶε�ɾ��
} TableMode,*PTableMode;

int get_int(char *sql,int *j){
    char num[10];
    int a=0;
    while(sql[*j]!=','){
        if(sql[*j]>='0'&&sql[*j]<='9'){
            num[a]=sql[*j];
            a++;
        }
        *j=*j+1;
    }
    return atoi(num);
}
void * get_char(char *sql,int *j,char *ch){
    int i=0;
    sql=sql+*j;
    while(*sql!='\''){
        *j=*j+1;
        sql++;
    }
    sql++;
    while(ch[i]!='\0'){
        ch[i]='\0';
        i++;
    }
    i=0;
    while(*sql!='\''){
        ch[i]=*sql;
        *j=*j+1;
        sql++;
        i++;
    }
}
int main(){
    TableMode tempTM;
    TableMode FieldSet[MAX_SIZE];
    FILE *fp1,*fp2;
    int tempi,iNum,tempj;  /*iNum is the number of the Field���ֶ���������*/
    char tempc;
    char dbf[FILE_NAME_LENGTH],tname[FILE_NAME_LENGTH]={},temptn[FILE_NAME_LENGTH],dat[FILE_NAME_LENGTH],sql[MAX_SQL];
    char values[6]={};
    int bFound;  /*found the table */
    int i,j,k;
    char flag;
    int irecord;
    int n;char ch[MAX_SIZE]={};
    int place=0;

    cin>>tname;

    strcpy(dbf,"school.dbf");
    fp1=fopen(dbf,"rb");
    if(fp1 == NULL ){
       printf("\nFile Not Exist or Bad File!\n" );
       exit(1);
    }
    while(!feof(fp1)){//�Ҷ�Ӧ��Ľṹ
        bFound=0;
        tempi=fread(&tempc,sizeof(char),1,fp1);//��¼���Ĵ���
        if(!tempi)break;
        fread(temptn,sizeof(char),FILE_NAME_LENGTH,fp1);//��ȡ����
        fread(&iNum,sizeof(int),1,fp1);//��ȡ�ֶε�����
        fread(FieldSet,sizeof(TableMode),iNum,fp1);//��ȡ�����ֶνṹ
        if(strcmp(temptn,tname)==0){//��λ
            bFound=1;
            break;
        }
    }
    if(bFound==1){//����ҵ���
        strcpy(dat,"school.dat");
        if((fp2=fopen(dat,"rb")) == NULL ){//��dat
            printf("\nFile Not Exist or Bad File!\n" );
        }
        while(!feof(fp2)){//��λ��������
            tempi=fread(&tempc,sizeof(char),1,fp2);
            if(!tempi)break;
            fread(temptn,sizeof(char),FILE_NAME_LENGTH,fp2);//��ȡ����
            fread(&irecord,sizeof(int),1,fp2);//�˱���������¼������
            fread(&iNum,sizeof(int),1,fp2);//��ȡ�ֶε�����
            fread(&flag,sizeof(char),1,fp2);//����ʶλ
            printf("FILE p offset:%d\n", (int)ftell(fp2));
            cout<<irecord<<endl;
            if(strcmp(temptn,tname)==0){
                printf("  No   ");
                for(i=0;i<iNum;i++){
                    printf("%-15s",FieldSet[i].sFieldName);
                }
            }
            cout<<endl;
            fseek(fp2,25+place,SEEK_SET);
            printf("FILE p offset:%d\n", (int)ftell(fp2));
            //cout<<(int)ftell(fp2);
            for(i=1;i<irecord+1;i++){//������ĩβ
                printf("  %d    ",i);
                for(j=0;j<iNum;j++){
                    //cout<<FieldSet[j].sType<<endl;
                    if(strcmp(FieldSet[j].sType,"int")==0){
                        fread(&n,sizeof(int),1,fp2);
                        printf("%-15d",n);
                        printf("FILE p offset:%d\n", (int)ftell(fp2));
                    }else{
                        fread(ch,sizeof(char),FieldSet[j].iSize,fp2);
                        printf("%-15s",ch);
                        printf("FILE p offset:%d\n", (int)ftell(fp2));
                    }
                }
                cout<<endl;
            }
            fseek(fp2,0,SEEK_END);
            printf("FILE p offset:%d\n", (int)ftell(fp2));
            if(strcmp(temptn,tname)==0)
                break;
        }

    }else{
        cout<<"Not Found"<<endl;
        exit(1);
    }
    fclose(fp2);
    fclose(fp1);
}
