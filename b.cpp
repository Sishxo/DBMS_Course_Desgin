#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <string.h>
#include <iostream>
using namespace std;

#define MAX_SIZE 100		/*表中所含字段最大长度*/
#define FILE_NAME_LENGTH 15		/*文件名最大长度*/
#define FIELD_NAME_LENGTH 15	/*字段名最大长度*/
#define MAX_SQL 1000 /*SQL语句最大长度*/

typedef struct {
  char sFieldName[FIELD_NAME_LENGTH];//字段名
  char sType[8];//类型
  int iSize;//字长
  char bKey;//该字段是否为key键
  char bNullFlag;//该字段是否允许为空
  char bValidFlag;//该字段是否有效，可用于以后对表中该字段的删除
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
    int tempi,iNum,tempj;  /*iNum is the number of the Field：字段名的数量*/
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
    while(!feof(fp1)){//找对应表的结构
        bFound=0;
        tempi=fread(&tempc,sizeof(char),1,fp1);//记录读的次数
        if(!tempi)break;
        fread(temptn,sizeof(char),FILE_NAME_LENGTH,fp1);//读取表名
        fread(&iNum,sizeof(int),1,fp1);//读取字段的数量
        fread(FieldSet,sizeof(TableMode),iNum,fp1);//读取所有字段结构
        if(strcmp(temptn,tname)==0){//定位
            bFound=1;
            break;
        }
    }
    if(bFound==1){//如果找到了
        strcpy(dat,"school.dat");
        if((fp2=fopen(dat,"rb")) == NULL ){//打开dat
            printf("\nFile Not Exist or Bad File!\n" );
        }
        while(!feof(fp2)){//定位到表的最后
            tempi=fread(&tempc,sizeof(char),1,fp2);
            if(!tempi)break;
            fread(temptn,sizeof(char),FILE_NAME_LENGTH,fp2);//读取表名
            fread(&irecord,sizeof(int),1,fp2);//此表内所含记录的数量
            fread(&iNum,sizeof(int),1,fp2);//读取字段的数量
            fread(&flag,sizeof(char),1,fp2);//读标识位
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
            for(i=1;i<irecord+1;i++){//读到表末尾
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
