#include <iostream>
#include <string>
#include <vector>
#include <cstdio>
#include <string.h>
#include <sstream>
#include <iomanip>
#include<windows.h>

#include <direct.h>
#include <io.h>

#include <fstream>
#define MAX_SIZE 1000000

using namespace std;

const int inf = 0x3f3f3f3f;//int�����������ֵ

struct Tables {
	string name;//����
	string pathName;//�洢·������
	vector<string>colName;//������
	vector<string>type;//����
	vector<string>key;//����
	vector<int>size;//��С
	FILE* fp;//�ļ�ָ��
};

class myDBMS {
	vector<Tables*>tab;
	bool open;//�Ƿ��
public:
	string cmd;//sql���
	string prePath;//���ݿ���ļ�·��

	myDBMS() {
		cmd = "";//��ʼ��
		prePath = "";
		open = false;//��ʼ�����ݿ�δ��

		//����ɾ��,����ɾ��,��ɾ�Ĳ�
		cout << "�밴���¹��������������(����ִ�Сд)" << endl;
		cout << "�½����ݿ�: create database ���ݿ�����" << endl;
		cout << "ɾ�����ݿ�: drop database ���ݿ�����" << endl;
		cout << "�����ݿ�: open database ���ݿ�����" << endl;
		cout << "�ر����ݿ�: close database ���ݿ�����" << endl;
		cout << "����±�  : create table  ����" << endl;
		cout << "            (" << endl;
		cout << "            ��1(�ֶ�)�� ��������1(��С:��Ϊ1��ʡ��) primarykey(�����ǿ�ʡ��)"  << endl;
		cout << "            ��2(�ֶ�)�� ��������2(��С:��Ϊ1��ʡ��) primarykey" << endl;
		cout << "            ��          ��" << endl;
		cout << "            )" << endl;
		cout << "ɾ���ɱ�  : drop table ����" << endl;
		cout << "�鿴ȫ��  : select all from ����" << endl;
		cout << "�򵥲�ѯ  : select ���� from ���� where ���� = ֵ(ֵ��Ϊall,��ȫ����Χ)" << endl;
		cout << "            select all from ����1������2 where ����1.����1 = ����2.����2" << endl;
		cout << "����      : insert into ����(field1,field2,��) values(value1,value2,��)" << endl;
		cout << "�޸�      : update ���� = ��ֵ from ���� where ���� = ֵ(ֵ��Ϊall,��ȫ����Χ)" << endl;
		cout << "ɾ��      : delete from ���� where ���� = ֵ(ֵ��Ϊall,��ȫ����Χ)" << endl;
		cout << "�˳�����  : exit" << endl;
	}
	//���������ʱ��������������
	~myDBMS() {
		for (int i = 0; i < tab.size(); i++) {//������Դ�ʱ���رմ򿪵ı�delete��
			if (tab[i]->fp != NULL)
				fclose(tab[i]->fp);
			delete tab[i];
		}
		tab.resize(0);//��������������СΪ0
		cmd = "";
		prePath = "";
	}

	void transfer();//תΪСд
	void openDataBase(string);//�����ݿ�
	void closeDataBase();//�ر����ݿ�
	void myCreateDataBase(string); //�������ݿ�
	void myDropDataBase(string); //ɾ�����ݿ�
	void myCreateTable(string); //������
	void myDropTable(string); //ɾ����
	void myInsert(string,string); //��������
	void myInsertwithCol(string,string,string); //��������(������)
	void myDelete(string,string); //ɾ������
	void myUpdate(string, string, string, string); //��������
	void myQuery(string, string, string); //��ѯ����
	void AddColumn(string);//���ӱ��е���
	void DropColumn(string);//ɾ�����е���
	void connect(string,string,string,string,string);
	void productImplement(vector<vector<string>> dimvalue,vector<vector<string>> &res,int,vector<string> tmp);
	string get_toColName(string);//��ȡtoColName


	int posIsNos(string);//��������
};

//תΪСд
void myDBMS::transfer() {//��sql����е���ĸȫ��ת��ΪСд
	for (int i = 0; i < cmd.size(); i++)
		cmd[i] = tolower(cmd[i]);
}

//�����ݿ⣬����Ϊ���ݿ���
void myDBMS::openDataBase(string dataBaseName) {
	if (open) {//����������ݿ��
		cout << "���ȹرյ�ǰ�򿪵����ݿ�" << endl;
		return;
	}
	string pathName = "E:\\CS346\\DBMS_Course_Desgin\\DB\\" + dataBaseName;//ƴ�Ӵ洢·��
	//������ݿ��Ƿ����
	if (0 != access(pathName.c_str(), 0))printf("�����ݿⲻ����\n");//c_str:��C++�е�stringת����C���ַ���,access���������ж�Ŀ���ļ��Ƿ����
	else {
		prePath = pathName + "\\";
		cout << "�����ݿ�ɹ�" << endl;
		open = true;
	}
}

//�ر����ݿ⣬������������ͬ
void myDBMS::closeDataBase() {
	for (int i = 0; i < tab.size(); i++) {
		if (tab[i]->fp != NULL)
			fclose(tab[i]->fp);
		delete tab[i];
	}
	tab.resize(0);
	cmd = "";
	prePath = "";
	open = false;
}

//�������ݿ�
void myDBMS::myCreateDataBase(string dataBaseName) {
	string pathName = "E:\\CS346\\DBMS_Course_Desgin\\DB\\" + dataBaseName;
	//���ж��Ƿ��и����ݿ����
	if (0 != access(pathName.c_str(), 0)) {
        //����0��ʾ�����ɹ�,-1��ʾʧ��,mkdir(path,mode)����Ŀ¼,modeĬ��0777������ȫ�ַ��ʣ�
		if (0 == mkdir(pathName.c_str()))
			cout << "�����ɹ�" << endl;
		else
			cout << "����ʧ��" << endl;
		return;
	}
	cout << "�����ݿ��Ѵ���" << endl;
}

//ɾ�����ݿ�
void myDBMS::myDropDataBase(string dataBaseName) {
	string pathName = "E:\\CS346\\DBMS_Course_Desgin\\DB\\" + dataBaseName;
	if (0 == access(pathName.c_str(), 0)) {//�����ѯ�������ݿ�
		pathName = "rd " + pathName;//rd:ɾ��Ŀ¼
		if (0 == system(pathName.c_str()))//system(),����һ��dos�������rd����Ϊɾ��ָ�����ֵΪ0��-1
			cout << "ɾ�����ݿ�" << dataBaseName << "�ɹ�" << endl;
		else
			cout << "ɾ�����ݿ�" << dataBaseName << "ʧ��" << endl;
		return;
	}
	cout << "���ݿ�" << dataBaseName << "������" << endl;
}

//������,����:����
/*
create table ����
(
��1(�ֶ�)�� ��������1(��С:��Ϊ1��ʡ��),
��2(�ֶ�)�� ��������2(��С:��Ϊ1��ʡ��),
��          ��
)
*/
void myDBMS::myCreateTable(string tableName) {
	vector<string>colName;//����
	vector<string>type;//����
	vector<string>key;//����
	vector<int>size;//��С
	string tmp;
	getchar();//��ȥ�س�
	//getline:��ȡ���У�����ǰ����Ƕ��Ŀո񣬲�����洢���ַ���������
	getline(cin, tmp);//������һ��
	getline(cin, tmp);//���������ֶ���,�Ƿ�����
	while (tmp != ")") {//��û�ж������һ��
		stringstream ss(tmp);//�����ݴ��ݸ�һ��stringstream ����
		string x;
		ss >> x; 
		colName.push_back(x);//ͨ����stringstream������ֵ����һ��string���󲢼ӵ�vector�������
		ss >> x;//�ٴθ�ֵ
		//ʶ��(��С)
		int pos = x.find('(');//find���ص���������Ѱ�ҵ������б�ע���ȵ�������
		if (pos == string::npos) {//find()���Ҳ���ָ��ֵ������»᷵��string::npos
			type.push_back(x);//û��(��ֱ�Ӱ�x�ӵ�type�������
			size.push_back(1);//��Ĭ��ֵ1�ӵ�size�ĺ���
		}
		else {//�����ע�˴�С
			type.push_back(x.substr(0, pos));//�ѵ���֮ǰ�����ݼ���type
			int num = 0;
			//��string��ʽ������ת��Ϊint��
			for (int i = pos + 1; i < x.length() - 1; i++)
				num = num * 10 + x[i] - '0';
			size.push_back(num);//�Ѵ�С����size�����
		}
		string iskey;
		ss>>iskey;
		if(iskey=="primarykey"){
			key.push_back("isKey");
		}
		else{
			key.push_back("notKey");
		}
		getline(cin, tmp);//��ȡ��һ��
	}
	tableName += ".txt";//�����Ϊ.TXT�ļ�
	string pathName = prePath + tableName;//ƴ�����ݿ���ļ�·���ͱ���
	//cout << pathName.c_str() << endl;
	if (0 != access(pathName.c_str(), 0)) {//���û�������
		Tables* ptr = new Tables;//�½�һ����Ķ�̬�ռ�
		ptr->name = tableName;
		ptr->pathName = pathName;
		ptr->colName = colName;
		ptr->type = type;
		ptr->size = size;
		ptr->key =key;
		ptr->fp = fopen(pathName.c_str(), "w");
		string wrin;
		wrin.clear();
		//���ñ�Ľṹ����һ����
		for (int i = 0; i < colName.size(); i++)
			if(key[i]=="isKey"){
				wrin += colName[i] + ";" + type[i] + ";" + to_string(size[i]) + ";"+"isKey"+";";
			}else{
            //�� ����;����;��С; ����ʽ�����ļ�
			wrin += colName[i] + ";" + type[i] + ";" + to_string(size[i]) + ";"+"notKey"+";";
			}
		wrin += "\n";//����
		fprintf(ptr->fp,wrin.c_str());//д���ļ�
		fclose(ptr->fp);//�ر��ļ�
		tab.push_back(ptr);//��table�����м�������±�
		cout << "�����±�ɹ�!" << endl;
		return;
	}
	cout << "�ñ��Ѿ�����!" << endl;
}

//ɾ��������:����
void myDBMS::myDropTable(string tableName) {
	tableName += ".txt";
	string pathName = prePath + tableName;
	if (0 != access(pathName.c_str(), 0))
		cout << "�ñ�����!" << endl;
	else {
		for (int i = 0; i < tab.size(); i++)
			if (tab[i]->name == tableName) {//�ҵ���Ӧ�ı�
			    //������ž͹ر��ļ�Ȼ��delete��
				if (tab[i]->fp != NULL)
					fclose(tab[i]->fp);
				delete tab[i];
                //��tab������ɾ���ñ�
				tab.erase(tab.begin() + i);//ɾ����λ��tables������λ��ǰ��
			}
		remove(pathName.c_str());//ɾ�����Ӧ���ļ�
		cout << "ɾ���ɹ�!" << endl;
	}
}

//alter table table_name
//add column_name datatype(size)
void myDBMS::AddColumn(string tableName){
	if(!open){
		cout << "��ѡ�����ݿ�!" << endl;
		return;
	}
	
	int pos=inf;
	for (int i = 0; i < tab.size(); i++)
		if (tab[i]->name == tableName) {
			pos=i; 
			break;
		}
	if(pos==inf){
		pos=posIsNos(tableName);
	}
	if(pos==-1) return;
	string tmp,x;
	getline(cin,tmp);
	stringstream ss(tmp);
	ss>>x;
	tab[pos]->colName.push_back(x);
	ss>>x;
	int pos2=x.find('(');
	if(pos2 == string::npos){
		tab[pos]->type.push_back(x);
		tab[pos]->size.push_back(1);
	}
	else{
		tab[pos]->type.push_back(x.substr(0,pos2));
		int num = 0;
			//��string��ʽ������ת��Ϊint��
		for (int i = pos + 1; i < x.length() - 1; i++)
			num = num * 10 + x[i] - '0';
		tab[pos]->size.push_back(num);//�Ѵ�С����size�����
	}
	tab[pos]->key.push_back("notKey");
	tableName +=".txt";
	string pathName=prePath+tableName;
	//cout<<"break"<<endl;
	char content[1024];
	tab[pos]->fp =fopen(pathName.c_str(),"r");
	if(0==access(pathName.c_str(),0)){
		string wrin;
		wrin.clear();
		for (int i = 0; i < tab[pos]->colName.size(); i++)
			if(tab[pos]->key[i]=="isKey"){
				wrin += tab[pos]->colName[i] +";"+ tab[pos]->type[i] + ";" + to_string(tab[pos]->size[i]) + ";"+"isKey"+";";
			}else{
            //�� ����;����;��С; ����ʽ�����ļ�
			wrin += tab[pos]->colName[i] + ";" + tab[pos]->type[i] + ";" + to_string(tab[pos]->size[i]) + ";"+"notKey"+";";
			}
		//cout<<wrin<<endl;
		wrin +="\n";
		fgets(content,1024,tab[pos]->fp);
		while(!feof(tab[pos]->fp)){
			memset(content,0,sizeof(content));
			fgets(content,1024,tab[pos]->fp);
			wrin +=content;
		}
		//wrin +="\n";
		fclose(tab[pos]->fp);
		tab[pos]->fp =fopen(pathName.c_str(),"w");
		fprintf(tab[pos]->fp,wrin.c_str());
		fclose(tab[pos]->fp);
		cout<<"����гɹ���"<<endl;
		return;
	}
	cout<<"�ñ�����";
}

//alter table table_name
//drop column column_name
void myDBMS::DropColumn(string tableName){
	if(!open){
		cout << "��ѡ�����ݿ�!" << endl;
		return;
	}
	
	int pos=inf;
	for (int i = 0; i < tab.size(); i++)
		if (tab[i]->name == tableName) {
			pos=i; 
			break;
		}
	if(pos==inf){
		pos=posIsNos(tableName);
	}
	if(pos==-1) return;
	string tmp,x;
	getline(cin,tmp);
	stringstream ss(tmp);
	ss>>x;
	ss>>x;
	int pos2;
	tableName +=".txt";
	string pathName=prePath+tableName;
	//cout<<"break"<<endl;
	char content[1024];
	tab[pos]->fp =fopen(pathName.c_str(),"r");
	if(0==access(pathName.c_str(),0)){
		string value;
		string wrin;
		wrin.clear();
			for(int i=0;i<tab[pos]->colName.size();i++){
				if(x!=tab[pos]->colName[i]){
					if(tab[pos]->key[i]=="isKey"){
						wrin += tab[pos]->colName[i] +";"+ tab[pos]->type[i] + ";" + to_string(tab[pos]->size[i]) + ";"+"isKey"+";";
					}else{
            		//�� ����;����;��С; ����ʽ�����ļ�
						wrin += tab[pos]->colName[i] + ";" + tab[pos]->type[i] + ";" + to_string(tab[pos]->size[i]) + ";"+"notKey"+";";
					}		
				}else{
					pos2=i;
				}
			}
		//cout<<wrin<<endl;
		fgets(content,1024,tab[pos]->fp);
		while(!feof(tab[pos]->fp)){
			memset(content,0,sizeof(content));
			fgets(content,1024,tab[pos]->fp);
			value=content;
			//cout<<value<<endl;
			stringstream valuess(value);
			wrin +="\n";
			for(int i=0;i<tab[pos]->colName.size()-2;i++){
				if(i!=pos2){
					valuess>>tmp;
					wrin +=tmp + " ";
				}
			}
			if(tab[pos]->colName.size()-2!=pos2){
				valuess>>tmp;
				wrin +=tmp;
			}
			//cout<<wrin<<endl;
		}
		wrin +="\n";
		fclose(tab[pos]->fp);
		tab[pos]->fp =fopen(pathName.c_str(),"w");
		fprintf(tab[pos]->fp,wrin.c_str());
		fclose(tab[pos]->fp);
		cout<<"ɾ���гɹ���"<<endl;
		return;
	}
	cout<<"�ñ�����";
}

//��������,����������,���ڶ�һ��txt���ļ�������tab������
int myDBMS::posIsNos(string tableName) {
	string pathName = prePath + tableName + ".txt";
	if (0 != access(pathName.c_str(), 0)) {
		cout << "�ñ�����!" << endl;
		return -1;
	}
	//�Զ�����ʽ�򿪱�
	FILE* tempfptr = fopen(pathName.c_str(), "r");
	char contant[1024];
	fscanf(tempfptr, "%s", contant);
	string tmp = contant;
	//��;��Ϊ�� ��
	for (int i = 0; i < tmp.size(); i++)
		if (tmp[i] == ';')
			tmp[i] = ' ';//���Ĵ��룬����Ĺ�ϵģʽ����;�ָ�����Ϊ�ո�
	stringstream check(tmp);
	string x, y, z, q;//x:���� y:���� z:��С q:�������
	Tables* nxt = new Tables;//�½�һ����̬��nxt
	nxt->name = tableName;//��ֵ����
	while (check >> x) {
		check >> y >> z>> q;
		nxt->colName.push_back(x);
		nxt->type.push_back(y);
		nxt->size.push_back(atoi(z.c_str()));//atoi():��stringת��Ϊint
		if(q=="isKey"){
			nxt->key.push_back("isKey");
		}
		else{
			nxt->key.push_back("notKey");
		}
	}
	nxt->pathName = pathName;
	tab.push_back(nxt);//
	fclose(tempfptr);//�ر��ļ�
	return tab.size() - 1;
}

int check_type(string x){
    int cnt=0;
    for(int i=0;i<x.size();i++){
        if(x[i]>='0'&&x[i]<='9')cnt++;
    }
    if(cnt==x.size())return 0;
    else if(cnt==x.size()-1 && x.find('.') && x[0]!='.' && x[x.size()-1]!='.')return 1;
    else return 2;
}

//��������,����:����,
//insert into ����(field1,field2,��) values(value1,value2,��)
//insert into student(Sno,Sname) values(100,wang)
void myDBMS::myInsert(string tableName,string value) {
	if (!open) {//���û�����ݿ��
		cout << "��ѡ�����ݿ�!" << endl;
		return;
	}
	int pos = inf;
	for (int i = 0; i < tab.size(); i++)//��λ���������е�λ��
		if (tab[i]->name == tableName) {
			pos = i; break;
		}
	if (pos == inf)//���û���ҵ���
		pos = posIsNos(tableName);
	if (pos == -1)return;

	tab[pos]->fp = fopen(tab[pos]->pathName.c_str(), "r");//�򿪱�
	char contant[1024];
	fgets(contant, 1024, tab[pos]->fp);

    int keyPos;
    bool error=false;
	for(int i=0;i<tab[pos]->size.size();i++){
        //cout<<tab[pos]->key[i]<<endl;
        if(tab[pos]->key[i]=="isKey"){
            keyPos = i;
            break;
        }
	}
	//cout<<keyPos<<endl;
	stringstream vl(value);
	string v;
	for(int i=0;i<=keyPos;i++){
        vl>>v;
    }
    //cout<<keyPos;
	
	while (!feof(tab[pos]->fp)) {
        memset(contant, 0, sizeof(contant));//sentence��ʼ��Ϊ0
        fgets(contant, 1024, tab[pos]->fp);
		//cout<<contant<<endl;
        stringstream ss(contant);
        string x;
        int i=0;
        ss>>x;
        while(i!=keyPos){
            ss>>x;
            i++;
        }
        //cout<<v<<x<<endl;
        if(v==x){
            error=true;
            break;
        }
	}
	fclose(tab[pos]->fp);
	//cout<<error<<endl;
	if(error){
        cout<<"����ʧ��"<<endl;
	}else{
        stringstream ss(value);
        for (int i = 0; i < tab[pos]->type.size(); i++) {
            string tmp = tab[pos]->type[i];//�����͸���tmp
            //cout << "tmp:" << tmp << endl;
            string x;
            ss>>x;
            //cout<<"x:"<<x<<endl;
            int type = check_type(x);
            //cout<<"type:"<<type<<endl;
            if (tmp == "int" && type==0) {
                continue;
            }
            else if (tmp == "float" && (type==1 || type==0)) {
                continue;
            }
            else if (tmp == "double" && (type==1 || type==0)) {
                continue;
            }
            else if (tmp == "char") {
                if(x.size()>tab[pos]->size[i]){
                    error=true;
                    break;
                }else{
                    continue;
                }
            }
            else{
                error=true;
            }

        }
        if(!error){
            stringstream va(value);
            tab[pos]->fp = fopen(tab[pos]->pathName.c_str(), "a");//�򿪱�
            for (int i = 0; i < tab[pos]->type.size(); i++) {
                string tmp = tab[pos]->type[i];//�����͸���tmp
                //cout << "tmp:" << tmp << endl;
                if (tmp == "int") {
                    int x; va >> x;
                    fprintf(tab[pos]->fp, "%d", x);
                }
                else if (tmp == "float") {
                    float x; va >> x;
                    fprintf(tab[pos]->fp, "%f", x);
                }
                else if (tmp == "double") {
                    double x; va >> x;
                    fprintf(tab[pos]->fp, "%f", x);
                }
                else if (tmp == "char") {
                    if (tab[pos]->size[i] == 1) {
                        char x; va >> x;
                        fprintf(tab[pos]->fp, "%c", x);
                    }
                    else {
                        int cnt = tab[pos]->size[i];
                        char* x = new char[cnt];
                        va >> x;
                        fprintf(tab[pos]->fp, "%s", x);
                        delete x;
                    }
                }
                if (i != tab[pos]->type.size() - 1)
                    fprintf(tab[pos]->fp, "%c", ' ');
            }
            fprintf(tab[pos]->fp, "%c", '\n');//����
            cout<<"����ɹ�"<<endl;
        }else{
			
            cout<<"����ʧ��"<<endl;
        }
        fclose(tab[pos]->fp);
	}
}
void myDBMS::myInsertwithCol(string tableName,string fieldname,string value) {
	vector<string>fieldlist;
	vector<string>valuelist;
	int flag;
	if (!open) {//���û�����ݿ��
		cout << "��ѡ�����ݿ�!" << endl;
		return;
	}
	int pos = inf;
	for (int i = 0; i < tab.size(); i++)//��λ���������е�λ��
		if (tab[i]->name == tableName) {
			pos = i; break;
		}
	if (pos == inf)//���û���ҵ���
		pos = posIsNos(tableName);
	if (pos == -1)return;
	stringstream ss(value);
	stringstream ssfield(fieldname);
	string singlefield,singlevalue;
	while(ssfield >> singlefield){
		fieldlist.push_back(singlefield);
		ss >> singlevalue;
		valuelist.push_back(singlevalue);
	}
	//cout<<fieldlist[2]<<endl;
	/*for(int i=0;i<fieldlist.size();i++){
		cout<<fieldlist[i]<<endl;
		cout<<valuelist[i]<<endl;
	}*/
	tab[pos]->fp = fopen(tab[pos]->pathName.c_str(), "a");//�򿪱�
	for (int i = 0; i < tab[pos]->type.size(); i++) {
		string tmp = tab[pos]->type[i];//�����͸���tmp
		//cout << "tmp:" << tmp << endl;
		flag=0;
		for(int j=0;j<fieldlist.size();j++){
			if(tab[pos]->colName[i]==fieldlist[j]){
				//cout<<valuelist[j]<<endl;
				flag=1;
				//cout<<"break"<<endl;
				if(tmp=="int"){
					int x=std::stoi(valuelist[j]);
					//cout<<x<<endl;
					fprintf(tab[pos]->fp,"%d",x);
				}
				else if(tmp=="float"){
					float x=std::stof(valuelist[j]);
					fprintf(tab[pos]->fp,"%f",x);
				}
				else if(tmp=="double"){
					double x=std::stold(valuelist[j]);
					fprintf(tab[pos]->fp,"%f",x);
				}
				else if (tmp == "char") {
						int cnt = tab[pos]->size[i];
						char* x = new char[cnt];
						strcpy(x,valuelist[j].c_str());
						fprintf(tab[pos]->fp, "%s", x);
						delete x;
				}
			}
		}
		if(flag==0){
			fprintf(tab[pos]->fp,"null");
		}
		if (i != tab[pos]->type.size() - 1){
			fprintf(tab[pos]->fp, "%c", ' ');
		}
	}
	fprintf(tab[pos]->fp, "%c", '\n');//����
	fclose(tab[pos]->fp);
	cout << "����ɹ�!" << endl;
}
//ɾ������
//delete from ���� where ���� = ֵ(ֵ��Ϊall,��ȫ����Χ)
void myDBMS::myDelete(string tableName,string isWhere) {
	if (!open) {
		cout << "��ѡ�����ݿ�!" << endl;
		return;
	}
	int pos = inf;
	for (int i = 0; i < tab.size(); i++)
		if (tab[i]->name == tableName) {
			pos = i; break;
		}
	if (pos == inf)
		pos = posIsNos(tableName);
	if (pos == -1)return;
	stringstream ss(isWhere);
	int wherePos = inf;
	string typeName, toValue, whr, deng;
	ss >> whr >> typeName >> deng >> toValue;
	for (int i = 0; i < tab[pos]->colName.size(); i++) {
		//cout << " " << tab[pos]->colName[i] << endl;
		if (tab[pos]->colName[i] == typeName) {//�������
			wherePos = i;
			break;
		}
	}
	string pathName2 = prePath + "tmp.txt";
	FILE* tmpfptr = fopen(pathName2.c_str(), "w");
	char sentence[1024];
	tab[pos]->fp = fopen(tab[pos]->pathName.c_str(), "r");
	fgets(sentence, 1024, tab[pos]->fp);
	fputs(sentence, tmpfptr);
	//�ж��Ƿ�Ϊall
	bool flag = true;
	bool aru = false;
	if (toValue.length() == 3 && tolower(toValue[0]) == 'a' && tolower(toValue[1]) == 'l' && tolower(toValue[2]) == 'l')
		flag = false;
    //����˼·���½�һ����tmp.txt�������ų�����where��������ļ�¼��Ȼ���tmp.txt���������Ƴ�ԭ�����ļ�
	if (flag) {
		while (!feof(tab[pos]->fp)) {
			memset(sentence, 0, sizeof(sentence));//sentence��ʼ��Ϊ0
			fgets(sentence, 1024, tab[pos]->fp);
			stringstream myTmp(sentence);
			//cout << "sentence " << sentence << endl;
			string x;
			for (int i = 0; i <= wherePos; i++)
				myTmp >> x;
			if (x == toValue){
                aru = true;
                continue;
			}
			fputs(sentence, tmpfptr);
		}
	}
	fclose(tmpfptr);
	fclose(tab[pos]->fp);
	remove(tab[pos]->pathName.c_str());
	if(aru==false)
        cout << "Ӱ��0����¼" <<endl;
    if (0 == rename(pathName2.c_str(), tab[pos]->pathName.c_str()))
        cout << "ɾ���ɹ�!" << endl;
    else
        cout << "ɾ��ʧ��!" << endl;
}

//��������
//update ���� = ��ֵ from ���� where ���� = ֵ(ֵ��Ϊall,��ȫ����Χ)
void myDBMS::myUpdate(string tableName, string toColName,string newValue, string isWhere) {
	if (!open) {
		cout << "��ѡ�����ݿ�!" << endl;
		return;
	}
	int pos = inf;
	for (int i = 0; i < tab.size(); i++)
		if (tab[i]->name == tableName) {
			pos = i; break;
		}
	if (pos == inf)
		pos = posIsNos(tableName);
	if (pos == -1)return;
	stringstream ss(isWhere);
	int wherePos = inf, updataPos = inf;
	string typeName, toValue, whr, deng;
	ss >> whr >> typeName >> deng >> toValue;
	for (int i = 0; i < tab[pos]->colName.size(); i++) //�ҵ���Χ��Ӧ��colName�±�
		if (tab[pos]->colName[i] == typeName) {
			wherePos = i;
			break;
		}
	for (int i = 0; i < tab[pos]->colName.size(); i++) //�ҵ���Ҫ�޸ĵ�colName�±�
		if (tab[pos]->colName[i] == toColName) {
			updataPos = i;
			break;
		}
	string pathName2 = prePath + "tmp.txt";
	FILE* tmpfptr = fopen(pathName2.c_str(), "w");
	char sentence[1024];
	tab[pos]->fp = fopen(tab[pos]->pathName.c_str(), "r");
	fgets(sentence, 1024, tab[pos]->fp);
	fputs(sentence, tmpfptr);
	//��ԭ�ļ������ݷŵ����ļ�
	bool flag = true;
	if (toValue.length() == 3 && tolower(toValue[0]) == 'a' && tolower(toValue[1]) == 'l' && tolower(toValue[2]) == 'l')
		flag = false;
	if (!flag) {//ȫ���޸�
		while (!feof(tab[pos]->fp)) {
			for (int i = 0; i < tab[pos]->type.size(); i++) {
				string tmp = tab[pos]->type[i];
				//cout << "tmp:" << tmp << endl;
				if (tmp == "int") {
					int x; fscanf(tab[pos]->fp, "%d", &x);
					if (i == updataPos)
						x = atoi(newValue.c_str());
					fprintf(tmpfptr, "%d", x);
				}
				else if (tmp == "float") {
					float x; fscanf(tab[pos]->fp, "%f", &x);
					if (i == updataPos)
						x = atof(newValue.c_str());
					fprintf(tmpfptr, "%f", x);
				}
				else if (tmp == "double") {
					double x; fscanf(tab[pos]->fp, "%lf", &x);
					if (i == updataPos)
						x = atof(newValue.c_str());
					fprintf(tmpfptr, "%f ", x);
				}
				else if (tmp == "char") {
					if (tab[pos]->size[i] == 1) {
						char x; fscanf(tab[pos]->fp, "%c", &x);
						if (i == updataPos)//�����Ҫ�޸ĵ���
							x = newValue[0];
						fprintf(tmpfptr, "%c", x);
					}
					else {
						int cnt = tab[pos]->size[i];
						char* x = new char[cnt];
						fscanf(tab[pos]->fp, "%s", x);
						if (i == updataPos) {
							strcpy(x, newValue.c_str());
							x[newValue.length()] = NULL;
						}
						fprintf(tmpfptr, "%s", x);
						delete x;
					}
				}
				if (i != tab[pos]->type.size() - 1)//����������һ������ո�
					fprintf(tmpfptr, "%c", ' ');
			}
			fprintf(tmpfptr, "%c", '\n');
		}
	}
	else {//��Χ���޸�
		while (!feof(tab[pos]->fp)) {
			memset(sentence, 0, sizeof(sentence));
			fgets(sentence, 1024, tab[pos]->fp);
			stringstream myTmp(sentence);
			string x;
			for (int i = 0; i <= wherePos; i++)
				myTmp >> x;
			if (x == toValue) {
				stringstream myTmp2(sentence);
				string input;
				input.clear();
				for (int i = 0; i < tab[pos]->colName.size(); i++) {
					myTmp2 >> x;
					if (i == updataPos)
						input += newValue;
					else
						input += x;
					if (i != tab[pos]->colName.size() - 1)
						input += " ";
					else
						input += "\n";
				}
				//cout << "input = " << input;
				fputs(input.c_str(), tmpfptr);
			}
			else
				fputs(sentence, tmpfptr);
		}
	}
	fclose(tmpfptr);
	fclose(tab[pos]->fp);
	remove(tab[pos]->pathName.c_str());
	if (0 == rename(pathName2.c_str(), tab[pos]->pathName.c_str()))
		cout << "���³ɹ�!" << endl;
	else
		cout << "����ʧ��!" << endl;
}

//��ѯ����
void myDBMS::myQuery(string toColName, string tableName, string isWhere = "") {
	if (!open) {
		cout << "��ѡ�����ݿ�!" << endl;
		return;
	}
	int pos = inf;
	for (int i = 0; i < tab.size(); i++)
		if (tab[i]->name == tableName) {
			pos = i; break;
		}
	if (pos == inf)
		pos = posIsNos(tableName);
	if (pos == -1)return;

	tab[pos]->fp = fopen(tab[pos]->pathName.c_str(), "r");
	char contant[MAX_SIZE];
	fgets(contant, sizeof(contant), tab[pos]->fp);
	stringstream ss(contant);
	string x;
	int keyPos = 0;

	if (isWhere == "") {//ȫ���
	    //cout<<toColName<<endl;
		int len = strlen(contant);
		for (int i = 0; i < len; i++)
			if (contant[i] == ';')
				contant[i] = ' ';
		stringstream tmp(toColName);
		string x,y,type[tab[pos]->size.size()];
		int sign[20],j,num=0,size[tab[pos]->size.size()];
		while(tmp>>y){
            //cout<<"y:"<<y<<endl;
            stringstream ss(contant);
            for (int v = 0; v < tab[pos]->size.size(); v++) {//�����colNames
                ss >> x;
                //cout<<x<<y<<endl;
                if(x == y){
                    //ȷ�����������ʾ�Ŀ��
                    int width = 15;
                    if (tab[pos]->size[v] != 1)
                    width = tab[pos]->size[v];
                    cout << left << setw(width) << x;//����벢���ÿ�����
                    sign[num] = v;
                    size[num] = tab[pos]->size[v];
                    type[num] = tab[pos]->type[v];
                    //cout<<"sign:"<<*sign<<' ';
                    num++;
                    break;
                }
                ss >> x;//����
                ss >> x;//��С
                ss >> x;//key
            }
		}
		cout << endl;
		//cout<<*ptr;
		//fgets(contant, sizeof(contant), tab[pos]->fp);
		while(!feof(tab[pos]->fp)) {
			memset(contant, 0, sizeof(contant));
			fgets(contant, sizeof(contant), tab[pos]->fp);
			if (strlen(contant) == 0){
                break;
			}
			string x;
			int i,k;
			for(k=0;k<num;k++) {
                stringstream out(contant);
                //cout<<"x:"<<x<<endl;
                //cout<<sign[k]<<endl;
                for(i=0;i<tab[pos]->size.size();i++){
                    out >> x;
                    if(i==sign[k]){
                        int width = 15;
                        if (tab[pos]->size[i] != 1)
                            width = tab[pos]->size[i];
                        string tmp = tab[pos]->type[i];
                        if (tmp == "int")
                            cout << left << setw(width) << atoi(x.c_str());
                        else if (tmp == "float" || tmp == "double")
                            cout << left << setw(width) << atof(x.c_str());
                        else if (tmp == "char")
                            cout << left << setw(width) << x;
                        break;
                    }
                }
			}
			cout << endl;
		}
	}
	else {//�������
		stringstream ss(isWhere);
		int aimPos = inf;
		int pcolName[20];
		int j=0;
		string typeName, toValue, whr, deng;
		ss >> whr >> typeName >> deng >> toValue;
		stringstream s(toColName);
		string z;

		while(s>>z){
            //cout<<"z:"<<z<<endl;
            for (int i = 0; i < tab[pos]->colName.size(); i++){
                //cout<<"colName:"<<tab[pos]->colName[i]<<endl;
                if (tab[pos]->colName[i] == z) {
                    int width = 15;
                    if (tab[pos]->size[i] != 1)
                    width = tab[pos]->size[i];
                    cout << left << setw(width) << z;
                    pcolName[j] = i;
                    //cout<<"pcolName:"<<pcolName[j]<<endl;
                    j++;
                }
            }
		}
		cout<<endl;
		for (int i = 0; i < tab[pos]->colName.size(); i++)
			if (tab[pos]->colName[i] == typeName) {
				aimPos = i;
				//cout<<"aimPos:"<<aimPos<<endl;
				break;
			}
		while (!feof(tab[pos]->fp)) {
			memset(contant, 0, sizeof(contant));
			fgets(contant, sizeof(contant), tab[pos]->fp);
			stringstream myTmp(contant);
			string x, check, out;//out:�����Ҫ�������
			j = 0;
			for (int i = 0; i < tab[pos]->colName.size(); i++) {
				myTmp >> x;
				if (i == pcolName[j]){
                    out = out+x+" ";
                    //cout<<out<<endl;
                    j++;
				}
				if (i == aimPos)check = x;
			}
			j = 0;
			stringstream ss(out);
			if (check == toValue) {
                while(ss>>x){
                    int width = 15;
                    if (tab[pos]->size[pcolName[j]] != 1)
                        width = tab[pos]->size[pcolName[j]];
                    string tmp = tab[pos]->type[pcolName[j]];
                    j++;
                    if (tmp == "int")
                        cout << left << setw(width) << atoi(x.c_str()) ;
                    else if (tmp == "float")
                        cout << left << setw(width) << atof(x.c_str()) ;
                    else if (tmp == "double")
                        cout << left << setw(width) << atof(x.c_str()) ;
                    else if (tmp == "char")
                        cout << left << setw(width) << x ;
                }
                cout<<endl;
			}
		}
	}
	fclose(tab[pos]->fp);
}
//select all from student,sc where student.Sno = sc.Sno
//��ȡ��������
string myDBMS::get_toColName(string tableName){
    if (!open) {
		cout << "��ѡ�����ݿ�!" << endl;
        exit(1);
	}
	int pos = inf;
	for (int i = 0; i < tab.size(); i++)
		if (tab[i]->name == tableName) {
			pos = i; break;
		}
	if (pos == inf)
		pos = posIsNos(tableName);
	if (pos == -1)return "";

	tab[pos]->fp = fopen(tab[pos]->pathName.c_str(), "r");
	char contant[1024];
	fgets(contant, sizeof(contant), tab[pos]->fp);
    fclose(tab[pos]->fp);
    int len = strlen(contant);
    for (int i = 0; i < len; i++)
        if (contant[i] == ';')
            contant[i] = ' ';
    stringstream tmp(contant);
    string x,toColName,y=",";
    for(int i=0;i<tab[pos]->size.size(); i++){
        tmp>>x;
        toColName=toColName+x;
        toColName=toColName+y;
        tmp>>x;
        tmp>>x;
        tmp>>x;
    }
    len = toColName.size();
    toColName = toColName.substr(0,len-1);
    return toColName;
}

void myDBMS::productImplement(vector<vector<string>> dimvalue,vector<vector<string>> &res,int layer,vector<string> tmp){
    if (layer < dimvalue.size() - 1){
		for (int i = 0; i < dimvalue[layer].size(); i++){//��ÿһ������
			vector<string> sb;//�½�һ������sb
			sb.clear();//�������

			for (int i = 0; i < tmp.size(); i++){
					sb.push_back(tmp[i]);
			}
			sb.push_back(dimvalue[layer][i]);
			productImplement(dimvalue, res, layer+1,sb);
		}
	}
	else if (layer == dimvalue.size()-1){
		for (int j = 0; j < dimvalue[layer].size();j++){
            tmp.push_back(dimvalue[layer][j]);
			res.push_back(tmp);
			tmp.pop_back();
		}
	}
}

void myDBMS::connect(string toColName,string tableName1,string tableName2,string colName1,string colName2){
    //cout<<tableName1<<tableName2<<endl;
    if (!open) {
		cout << "��ѡ�����ݿ�!" << endl;
		return;
	}
	int pos1 = inf, pos2 = inf;
	for (int i = 0; i < tab.size(); i++)
		if (tab[i]->name == tableName1) {
			pos1 = i; break;
		}
	if (pos1 == inf)
		pos1 = posIsNos(tableName1);
	if (pos1 == -1)return;

	for (int i = 0; i < tab.size(); i++)
		if (tab[i]->name == tableName2) {
			pos2 = i; break;
		}
	if (pos2 == inf)
		pos2 = posIsNos(tableName2);
	if (pos2 == -1)return;

	vector<string> t1,t2;
	char contant[102400];
	int num[40];
	int n = 0;
	int v1,v2;

	for(int i=0;i<tab[pos1]->size.size();i++){
        if(colName1==tab[pos1]->colName[i]){
            v1=i;
            break;
        }
	}
	for(int i=0;i<tab[pos2]->size.size();i++){
        if(colName2==tab[pos2]->colName[i]){
            v2=i;
            break;
        }
	}


	string pathName =prePath +"tmp.txt";
	FILE* tmpfptr = fopen(pathName.c_str(), "w");

	tab[pos1]->fp = fopen(tab[pos1]->pathName.c_str(), "r");
	fgets(contant, sizeof(contant), tab[pos1]->fp);
	string cc1(contant);

	while (!feof(tab[pos1]->fp)){
        memset(contant, 0, sizeof(contant));
        fgets(contant, sizeof(contant), tab[pos1]->fp);
        if (strlen(contant) == 0)break;
        for(int i=0;i<strlen(contant);i++){
            if(contant[i]==';')contant[i]=' ';
        }
        string x(contant);
        x=x.substr(0,strlen(contant)-1);
        x=x+" ";
        t1.push_back(x);
	}
	fclose(tab[pos1]->fp);

	tab[pos2]->fp = fopen(tab[pos2]->pathName.c_str(), "r");
	fgets(contant, sizeof(contant), tab[pos2]->fp);
	string cc2(contant);
	cc1=cc1.substr(0,cc1.size()-2)+";"+cc2;
    fprintf(tmpfptr, "%s", cc1.c_str());
    for(int i=0;i<cc1.size();i++){
        if(cc1[i]==';')cc1[i]=' ';
    }
    stringstream col(cc1);
    string c="";
    for(int i=0;i<tab[pos1]->size.size()+tab[pos2]->size.size();i++){
        col>>cc2;
        //cout<<cc2<<endl;
        c=c+cc2+" ";
        col>>cc2;col>>cc2;col>>cc2;
    }

	while (!feof(tab[pos2]->fp)){
        memset(contant, 0, sizeof(contant));
        fgets(contant, sizeof(contant), tab[pos2]->fp);
        if (strlen(contant) == 0)break;
        for(int i=0;i<strlen(contant);i++){
            if(contant[i]==';')contant[i]=' ';
        }
        string x(contant);
        x=x.substr(0,strlen(contant)-1);
        x=x+" ";
        t2.push_back(x);
	}
	fclose(tab[pos2]->fp);

	vector<vector<string>> dimvalue;
	dimvalue.push_back(t1);
	dimvalue.push_back(t2);
	vector<string> tmp;
	vector<vector<string>> res;

	int layer = 0,v[tab[pos1]->size.size()*tab[pos2]->size.size()],z=0;
	string x,x1,x2;
	productImplement(dimvalue, res, layer, tmp);
	for (int i = 0; i < res.size(); i++){
        //cout<<res[i][0]<<res[i][1]<<endl;
		stringstream ss1(res[i][0]);
		int j=0;
		ss1>>x;
        while(j<v1){
            ss1>>x;
            //cout<<x<<" ";
            j++;
        }
		x1=x;
		stringstream ss2(res[i][1]);
		j=0;
        ss2>>x;
		while(j<v2){
            ss2>>x;
            //cout<<"x:"<<x<<" ";
            j++;
        }
		x2=x;
		//cout<<x1<<x2<<endl;
		if(x1==x2){
            v[z]=i;
            z++;
		}
		//cout<<"";
	}
	z=0;
	for(int i=0;i<res.size();i++){
        if(i==v[z]){
            int k=0;
            for (int j = 0; j < res[i].size(); j++){
                //cout<<res[i][j]<<endl;
                stringstream ss(res[i][j]);
                string x;
                if(num[k]<15)num[k]=15;
                while(ss>>x){
                    fprintf(tmpfptr, "%s", x.c_str());
                    string tmp1=" ";
                    fprintf(tmpfptr, "%s", tmp1.c_str());
                    k++;
                }
            }
            string tmp2="\n";
            fprintf(tmpfptr, "%s", tmp2.c_str());
            z++;
        }
	}
	fclose(tmpfptr);
	Sleep(3*1000);
	if(toColName=="all"){
        toColName = c;
        //cout<<toColName;
	}
	myQuery(toColName, "tmp");
	//remove(pathName.c_str());
}
//select all from student,sc where student.Sno = sc.Sno



int main(void) {
	myDBMS db;
	while (cin >> db.cmd) {
		db.transfer();
		//cout << "db.cmd = " << db.cmd << endl;
		if (db.cmd == "exit")break;
		if (db.cmd == "create") {
			string name;
			cin >> db.cmd >> name;
			db.transfer();
			//cout << "db.cmd:" << db.cmd << " name:" << name << endl;
			if (db.cmd == "database")
				db.myCreateDataBase(name);
			else if (db.cmd == "table")
				db.myCreateTable(name);
			else cout << "�����������!" << endl;
		}
		else if (db.cmd == "drop") {
			string name;
			cin >> db.cmd >> name;
			db.transfer();
			if (db.cmd == "database")
				db.myDropDataBase(name);
			else if (db.cmd == "table")
				db.myDropTable(name);
			else cout << "�����������!" << endl;
		}
		else if (db.cmd == "open") {
			string name;
			cin >> db.cmd >> name;
			db.openDataBase(name);
		}
		else if (db.cmd == "close") {
			string name;
			cin >> db.cmd >> name;
			db.transfer();
			if (db.cmd == "database")
				db.closeDataBase();
			else cout << "�����������!" << endl;
		}
		else if (db.cmd == "insert") {
			//insert into ����(field1,field2,��) values(value1,value2,��)"
			string tmp, name, value,fieldname;
			cin >> db.cmd >> name >> tmp;
			//cout<<"db.cmd:"<<db.cmd<<" name:"<<name<<" tmp:"<<tmp<<endl;
			value = tmp.substr(7);//�ص�values(
			vector<string>colName;
			int pos = name.find('(');
			fieldname=name.substr(pos+1);
			//cout<<fieldname<<endl;
			for(int i=0;i<fieldname.length();i++){
				if(fieldname[i]==','||fieldname[i]=='('||fieldname[i]==')'){
					fieldname[i]=' ';
				}
			}
			for(int i = 0; i < value.length(); i++)
				if (value[i] == ',' || value[i] == '(' || value[i] == ')')
					value[i] = ' ';
			//cout<<value<<endl;
			if(pos==string::npos){
				db.myInsert(name,value);			
			}
			else{
			//cout <<"fieldname:"<<fieldname<< "value:" << value << endl;
			db.myInsertwithCol(name.substr(0, pos),fieldname,value);
			}
		}
		else if (db.cmd == "delete") {
			//delete from ���� where ��Χ
			string name;
			cin >> db.cmd >> name;
			string isWhere;
			getline(cin, isWhere);
			db.myDelete(name, isWhere);
		}
		else if (db.cmd == "update") {
			//update ���� = ��ֵ from ���� where ���� = ֵ(ֵ��Ϊall, ��ȫ����Χ)
			string toColName,deng, newValue, tableName, isWhere;
			cin >> toColName >> deng >> newValue >> db.cmd >> tableName;
			getline(cin, isWhere);
			db.myUpdate(tableName, toColName, newValue, isWhere);
		}
		else if (db.cmd == "select") {
			//"�鿴ȫ��  : select all from ����"
			//"��ѯ��ֵ  : select ���� from ���� where ���� = ֵ(ֵ��Ϊall,��ȫ����Χ)"
			string toColName, tableName;
			cin >> toColName >> db.cmd >> tableName;
			string isWhere;
            getline(cin,isWhere);
            //cout<<isWhere<<endl;
			int flag=0;//�ж��Ƿ���Ҫ�����Ӳ���
			for(int i=0;i<tableName.size();i++){
                if(tableName[i]==','){
                    tableName[i]=' ';
                    flag=1;
                    break;
                }
			}
			if(flag){
                stringstream ss(tableName);
                string t1,t2;
                ss>>t1;ss>>t2;
                for(int i=0;i<isWhere.size();i++){
                    if(isWhere[i]==',')isWhere[i]=' ';
                }
                stringstream ww(isWhere);
                string where,c1,deng,c2;
                ww>>where;ww>>c1;ww>>deng;ww>>c2;
                for(int i=0;i<c1.size();i++){
                    if(c1[i]=='.')c1[i]=' ';
                }
                for(int i=0;i<c2.size();i++){
                    if(c2[i]=='.')c2[i]=' ';
                }
                //select all from student,sc where student.Sno = sc.Sno
                stringstream cc1(c1),cc2(c2);
                cc1>>c1;cc1>>c1;
                cc2>>c2;cc2>>c2;
                for(int i=0;i<toColName.length();i++){
                    if(toColName[i] == ',')
                        toColName[i] = ' ';
                }
                db.connect(toColName,t1,t2,c1,c2);
			}
			else{
                if (toColName.length() == 3 && tolower(toColName[0]) == 'a' && tolower(toColName[1]) == 'l' && tolower(toColName[2]) == 'l') {
                    toColName = db.get_toColName(tableName);
                }
                for(int i=0;i<toColName.length();i++){
                    if(toColName[i] == ',')
                        toColName[i] = ' ';
                }
                db.myQuery(toColName, tableName, isWhere);
			}
            //cout<<toColName<<"end"<<endl;
		}
		else if(db.cmd== "alter"){
			string table_name,action;
			cin>>db.cmd>>table_name;
			db.transfer(); 
			getchar();
			if(db.cmd=="table"){
				cin>>db.cmd;
				db.transfer();
				if(db.cmd=="add"){
					db.AddColumn(table_name);
				}
				else if(db.cmd=="drop"){
					db.DropColumn(table_name);
				}
			}
		}
		else {
			string tmp;
			getline(cin, tmp);
			cout << "�����������,����!" << endl;
		}

	}
	return 0;
}
//select all from student,course
