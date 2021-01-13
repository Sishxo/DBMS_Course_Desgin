/**
��������
�������=
���ţ�()
�ؼ��֣� create drop open close database
         all table from select where insert into values
         update delete primarykey and or
         �������� int char
���֣�
���������������������ݿ�����Ԫ��ֵ���ַ���/���֣�

����sql server�����ִ�Сд������
**/

#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

using namespace std;

string KEYWORD[20]={"create", "drop", "open", "close", "database","and", "or",       //�ؼ���
                    "all", "table", "from", "select", "where", "insert", "into",
                    "values","update", "delete", "primarykey", "int", "char",};

char SEPARATER[2]={'(',')'};    //�ָ���
char OPERATOR[1]={'='};     //�����
char FILTER[4]={' ','\t','\r','\n'};                    //���˷�
const int IDENTIFIER=100;         //��ʶ��ֵ
const int CONSTANT=101;           //����ֵ
const int FILTER_VALUE=102;       //�����ַ�ֵ


/**�ж��Ƿ�Ϊ�ؼ���**/

bool IsKeyword(string word){
    //����д��ΪСд
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

/**�ж��Ƿ�Ϊ�ָ���**/

bool IsSeparater(char ch){
    for(int i=0;i<2;i++){
        if(SEPARATER[i]==ch){
            return true;
        }
    }
    return false;
}

/**�ж��Ƿ�Ϊ�����**/

bool IsOperator(char ch){
    for(int i=0;i<1;i++){
        if(OPERATOR[i]==ch){
            return true;
        }
    }
    return false;
}

/**�ж��Ƿ�Ϊ���˷�**/

bool IsFilter(char ch){
    for(int i=0;i<4;i++){
        if(FILTER[i]==ch){
            return true;
        }
    }
    return false;
}

/**�ж��Ƿ�Ϊ����**/

bool IsDigit(char ch){
    if(ch>='0' && ch<='9') return true;
    return false;
}

/**����ÿ���ֵ�ֵ**/

template <class T>

int value(T *a,int n,T str){
	for(int i=0;i<n;i++){
		if(a[i]==str) return i+1;
	}
	return -1;
}

/**�ʷ�����**/

void analyse(FILE * fpin){
    char ch=' ';
    string arr="";
    while((ch=fgetc(fpin))!=EOF){
        arr="";
        if(IsFilter(ch)){}              //�ж��Ƿ�Ϊ���˷�
        else if(isalpha(ch)||ch=='_'){      //�ж��Ƿ�Ϊ�ؼ��ֻ��ʶ��
            while(isalpha(ch)||ch=='_'||IsDigit(ch)){
                arr += ch;
                ch=fgetc(fpin);
            }
            if(IsKeyword(arr)){
                printf("%3d    ",value(KEYWORD,20,arr));
				cout<<arr<<"  �ؼ���"<<endl;
			}
			else{
                printf("%3d    ",IDENTIFIER);
                cout<<arr<<"  ��ʶ��"<<endl;
			}
        }
        else if(IsDigit(ch)){           //�ж��Ƿ�Ϊ����
            while(IsDigit(ch)||(ch=='.'&&IsDigit(fgetc(fpin)))){
                arr += ch;
                ch=fgetc(fpin);
            }
            printf("%3d    ",CONSTANT);
            cout<<arr<<"  ����"<<endl;
        }
        else switch(ch){
        case '=':
            {
                arr += ch;
                printf("%3d    ",value(OPERATOR,1,*arr.data()));
                cout<<arr<<"  �����"<<endl;
                break;
            }
        case '(':
        case ')':
            {
              arr += ch;
              printf("%3d    ",value(SEPARATER,2,*arr.data()));
              cout<<arr<<"  �ָ���"<<endl;
              break;
            }
        default :cout<<"\""<<ch<<"\":�޷�ʶ����ַ���"<<endl;
        }
    }
}

//C:\Users\2000514\Desktop\text.txt

int main(){
    char inFile[40];
    FILE *fpin;
    cout<<"������Դ�ļ���������·���ͺ�׺��:";
    while(true){
        cin>>inFile;
        if((fpin=fopen(inFile,"r"))!=NULL)
            break;
        else{
            cout<<"�ļ�������"<<endl;
            cout<<"������Դ�ļ���������·���ͺ�׺��:";
        }
    }
    cout<<"------�ʷ���������------"<<endl;
    analyse(fpin);
    return 0;
}
