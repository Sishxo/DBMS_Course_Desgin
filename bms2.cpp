/* dbms1.cpp :  Database Manage System(2) -- Insert.  */

#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <string.h>
#include <iostream>
using namespace std;

#define MAX_SIZE 50		/*表中所含字段最大长度*/
#define FILE_NAME_LENGTH 15		/*文件名最大长度*/
#define FIELD_NAME_LENGTH 15	/*字段名最大长度*/

typedef struct {
    char sFieldName[FIELD_NAME_LENGTH];//字段名
    char sType[8];//类型
    int iSize;//字长
    char bKey;//该字段是否为key键
    char bNullFlag;//该字段是否允许为空
    char bValidFlag;//该字段是否有效，可用于以后对表中该字段的删除
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
    int tempi,iNum,i=0;  /*iNum is the number of the Field：字段名的数量*/
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
    while(!feof(fp)){//定位到表明匹配的表输出记录
        bFound=0;
        tempi=fread(&tempc,sizeof(char),1,fp);
        if(!tempi)break;
        fread(temptn,sizeof(char),FILE_NAME_LENGTH,fp);//读取表名
        fread(&iNum,sizeof(int),1,fp);//读取字段的数量
        fread(FieldSet,sizeof(TableMode),iNum,fp);//读取所有字段结构
        if(strcmp(tname,temptn)==0){//定位
            break;  /*break while*/
        }
    }
    fclose(fp);

    strcpy(dat,"school.dat");
    if((fp=fopen(dat,"ab+"))==NULL){
        printf("open or create file error!");
        exit(1);
    }
    fwrite("~",sizeof(char),1,fp);//分隔符
    fwrite(tname,sizeof(char),FILE_NAME_LENGTH,fp);//写入表名
    fwrite(&i,sizeof(int),1,fp);//此表内所含记录的数量
    fwrite(&iNum,sizeof(int),1,fp);//记录的字段数量
    fwrite("y",sizeof(char),1,fp);//有效位
    printf("FILE p offset:%d\n", (int)ftell(fp));
    fclose(fp);

    return 0;
}


