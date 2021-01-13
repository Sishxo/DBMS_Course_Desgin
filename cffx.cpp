/**
分析对象：
运算符：=
符号：()
关键字： create drop open close database
         all table from select where insert into values
         update delete primarykey and or
         数据类型 int char
数字：
其他：列名、表名、数据库名、元素值（字符串/数字）

秉承sql server不区分大小写的特性
**/

#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

using namespace std;

string KEYWORD[20]={"create", "drop", "open", "close", "database","and", "or",       //关键字
                    "all", "table", "from", "select", "where", "insert", "into",
                    "values","update", "delete", "primarykey", "int", "char",};

char SEPARATER[2]={'(',')'};    //分隔符
char OPERATOR[1]={'='};     //运算符
char FILTER[4]={' ','\t','\r','\n'};                    //过滤符
const int IDENTIFIER=100;         //标识符值
const int CONSTANT=101;           //常数值
const int FILTER_VALUE=102;       //过滤字符值


/**判断是否为关键字**/

bool IsKeyword(string word){
    //将大写改为小写
    int n=word.length();
	for(int i=0;i<n;i++){
		if(word[i]<='Z'&&word[i]>='A'){
			word[i]=word[i]+32;
		}
	}

    for(int i=0;i<20;i++){
        if(KEYWORD[i]==word){
            return true;
        }
    }
    return false;
}

/**判断是否为分隔符**/

bool IsSeparater(char ch){
    for(int i=0;i<2;i++){
        if(SEPARATER[i]==ch){
            return true;
        }
    }
    return false;
}

/**判断是否为运算符**/

bool IsOperator(char ch){
    for(int i=0;i<1;i++){
        if(OPERATOR[i]==ch){
            return true;
        }
    }
    return false;
}

/**判断是否为过滤符**/

bool IsFilter(char ch){
    for(int i=0;i<4;i++){
        if(FILTER[i]==ch){
            return true;
        }
    }
    return false;
}

/**判断是否为数字**/

bool IsDigit(char ch){
    if(ch>='0' && ch<='9') return true;
    return false;
}

/**返回每个字的值**/

template <class T>

int value(T *a,int n,T str){
	for(int i=0;i<n;i++){
		if(a[i]==str) return i+1;
	}
	return -1;
}

/**词法分析**/

void analyse(FILE * fpin){
    char ch=' ';
    string arr="";
    while((ch=fgetc(fpin))!=EOF){
        arr="";
        if(IsFilter(ch)){}              //判断是否为过滤符
        else if(isalpha(ch)||ch=='_'){      //判断是否为关键字或标识符
            while(isalpha(ch)||ch=='_'||IsDigit(ch)){
                arr += ch;
                ch=fgetc(fpin);
            }
            if(IsKeyword(arr)){
                printf("%3d    ",value(KEYWORD,20,arr));
				cout<<arr<<"  关键字"<<endl;
			}
			else{
                printf("%3d    ",IDENTIFIER);
                cout<<arr<<"  标识符"<<endl;
			}
        }
        else if(IsDigit(ch)){           //判断是否为数字
            while(IsDigit(ch)||(ch=='.'&&IsDigit(fgetc(fpin)))){
                arr += ch;
                ch=fgetc(fpin);
            }
            printf("%3d    ",CONSTANT);
            cout<<arr<<"  数字"<<endl;
        }
        else switch(ch){
        case '=':
            {
                arr += ch;
                printf("%3d    ",value(OPERATOR,1,*arr.data()));
                cout<<arr<<"  运算符"<<endl;
                break;
            }
        case '(':
        case ')':
            {
              arr += ch;
              printf("%3d    ",value(SEPARATER,2,*arr.data()));
              cout<<arr<<"  分隔符"<<endl;
              break;
            }
        default :cout<<"\""<<ch<<"\":无法识别的字符！"<<endl;
        }
    }
}

//C:\Users\2000514\Desktop\text.txt

int main(){
    char inFile[40];
    FILE *fpin;
    cout<<"请输入源文件名（包括路径和后缀）:";
    while(true){
        cin>>inFile;
        if((fpin=fopen(inFile,"r"))!=NULL)
            break;
        else{
            cout<<"文件名错误！"<<endl;
            cout<<"请输入源文件名（包括路径和后缀）:";
        }
    }
    cout<<"------词法分析如下------"<<endl;
    analyse(fpin);
    return 0;
}
