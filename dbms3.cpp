#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <string.h>
#include <iostream>
using namespace std;

#define MAX_SIZE 50		/*表中所含字段最大长度*/
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

int found(char *s,char *sql){
    int i=0,cnt=0,flag=0,num=0;
    char *tmp=s;
    while(*tmp!='\0'){
        num++;
        tmp++;
    }
    while(*sql!='\0'){
        if(*s==*sql){
            cnt++;
            flag=1;
            s++;
        }else{
            flag=0;
        }
        if(flag=0) cnt=0;
        if(cnt==num) break;
        sql++;i++;
    }
    return i;
}

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
    int i,j;
    char flag;
    int irecord;
    int n;char ch[MAX_SIZE]={};
    int place=0;

    cin.getline(sql,1000);//获取sql语句
    i=12;j=0;
    while(sql[i]!=' '){//获取表名
        tname[j]=sql[i];
        j++;
        i++;
    }

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
        if((fp2=fopen(dat,"r+b")) == NULL ){//打开dat
            printf("\nFile Not Exist or Bad File!\n" );
        }
        while(!feof(fp2)){//定位到表的最后
            tempi=fread(&tempc,sizeof(char),1,fp2);
            if(!tempi)break;
            fread(temptn,sizeof(char),FILE_NAME_LENGTH,fp2);//读取表名
            fread(&irecord,sizeof(int),1,fp2);//此表内所含记录的数量
            if(strcmp(temptn,tname)==0){
                fseek(fp2,-sizeof(int),SEEK_CUR);
                i=irecord+1;
                fwrite(&i,sizeof(int),1,fp2);
                fseek(fp2,-sizeof(int),SEEK_CUR);
                int nn;
                fread(&nn,sizeof(int),1,fp2);
            }
            fread(&iNum,sizeof(int),1,fp2);//读取字段的数量
            fread(&flag,sizeof(char),1,fp2);//读标识位
            printf("FILE p offset:%d\n", (int)ftell(fp2));
            if(strcmp(temptn,tname)==0){
                for(i=0;i<iNum;i++){
                    printf("%-15s",FieldSet[i].sFieldName);
                }
            }
            fseek(fp2,place+25,SEEK_SET);
            cout<<endl;
            cout<<"irecord:"<<irecord<<endl;
            printf("FILE p offset:%d\n", (int)ftell(fp2));
            for(i=1;i<irecord+1;i++){//读到表末尾
                for(j=0;j<iNum;j++){
                    if(strcmp(FieldSet[j].sType,"int")==0){
                        fread(&n,sizeof(int),1,fp2);
                        //printf("FILE p offset:%d\n", (int)ftell(fp2));
                        printf("%-15d",n);
                    }else{
                        fread(ch,sizeof(char),FieldSet[j].iSize,fp2);
                        //printf("FILE p offset:%d\n", (int)ftell(fp2));
                        printf("%-15s",ch);
                    }
                }
                cout<<endl;
            }
            printf("FILE p offset:%d\n", (int)ftell(fp2));
            if(strcmp(temptn,tname)==0){
                j=found("(",sql);
                for(i=0;i<iNum;i++){
                    //cout<<"j:"<<j<<endl;
                    if(strcmp(FieldSet[i].sType,"int")==0){
                        n=get_int(sql,&j);
                        printf("%-15d",n);
                        cout<<fwrite(&n,sizeof(int),1,fp2);
                    }else{
                        get_char(sql,&j,ch);
                        printf("%-15s",ch);
                        cout<<fwrite(ch,sizeof(char),FieldSet[i].iSize,fp2);
                    }
                }
                printf("FILE p offset:%d\n", (int)ftell(fp2));
                break;
            }
            place=(int)ftell(fp2);
            cout<<"place:"<<place;
        }

    }
    fclose(fp2);
    fclose(fp1);

//insert into student values(100,'wang')
//insert into student values(200,'li')
//insert into student values()

    return 0;
}

