#include <iostream>
#include <string>
#include <vector>
#include <cstdio>
#include <string.h>
#include <sstream>
#include <iomanip>

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
		cout << "            ��1(�ֶ�)�� ��������1(��С:��Ϊ1��ʡ��)" << endl;
		cout << "            ��2(�ֶ�)�� ��������2(��С:��Ϊ1��ʡ��)" << endl;
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
	void myDelete(string,string); //ɾ������
	void myUpdate(string, string, string, string); //��������
	void myQuery(string, string, string); //��ѯ����
	void connect(string,string,string,string);
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
	string pathName = "C:\\Users\\stell\\Desktop\\���ݿ�ԭ��γ�����ĵ�202012\\DBMS\\" + dataBaseName;//ƴ�Ӵ洢·��
	//������ݿ��Ƿ����
	if (0 != access(pathName.c_str(), 0))printf("�����ݿⲻ����\n");//c_str:��C++�е�stringת����C���ַ���
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
	string pathName = "C:\\Users\\stell\\Desktop\\���ݿ�ԭ��γ�����ĵ�202012\\DBMS\\" + dataBaseName;
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
	string pathName = "C:\\Users\\stell\\Desktop\\���ݿ�ԭ��γ�����ĵ�202012\\DBMS\\" + dataBaseName;
	if (0 == access(pathName.c_str(), 0)) {//�����ѯ�������ݿ�
		pathName = "rd " + pathName;//rd:ɾ��Ŀ¼
		if (0 == system(pathName.c_str()))
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
	vector<int>size;//��С
	string tmp;
	getchar();//����
	//getline:��ȡ���У�����ǰ����Ƕ��Ŀո񣬲�����洢���ַ���������
	getline(cin, tmp);//������һ��
	getline(cin, tmp);//���������ֶ���
	while (tmp != ")") {//��û�ж������һ��
		stringstream ss(tmp);//�����ݴ��ݸ�һ��stringstream ����
		string x;
		ss >> x; colName.push_back(x);//ͨ����stringstream������ֵ����һ��string���󲢼ӵ�vector�������
		ss >> x;//�ٴθ�ֵ
		//ʶ��(��С)
		int pos = x.find('(');
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
		getline(cin, tmp);//��ȡ��һ��
	}
	tableName += ".txt";
	string pathName = prePath + tableName;//ƴ�����ݿ���ļ�·���ͱ���
	//cout << pathName.c_str() << endl;
	if (0 != access(pathName.c_str(), 0)) {//���û�������
		Tables* ptr = new Tables;//�½�һ����Ķ�̬�ռ�
		ptr->name = tableName;
		ptr->pathName = pathName;
		ptr->colName = colName;
		ptr->type = type;
		ptr->size = size;
		ptr->fp = fopen(pathName.c_str(), "w");
		string wrin;
		wrin.clear();
		//���ñ�Ľṹ����һ����
		for (int i = 0; i < colName.size(); i++)
            //�� ����;����;��С; ����ʽ�����ļ�
			wrin += colName[i] + ";" + type[i] + ";" + to_string(size[i]) + ";";
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
				tab.erase(tab.begin() + i);
			}
		remove(pathName.c_str());//ɾ�����Ӧ���ļ�
		cout << "ɾ���ɹ�!" << endl;
	}
}

//��������,����������
int myDBMS::posIsNos(string tableName) {
	string pathName = prePath + tableName + ".txt";
	if (0 != access(pathName.c_str(), 0)) {
		cout << "�ñ�����!" << endl;
		return -1;
	}
	//�Զ�����ʽ�򿪱�
	FILE* tempfptr = fopen(pathName.c_str(), "r");
	char contant[100];
	fscanf(tempfptr, "%s", contant);
	string tmp = contant;
	//��;��Ϊ�� ��
	for (int i = 0; i < tmp.size(); i++)
		if (tmp[i] == ';')
			tmp[i] = ' ';
	stringstream check(tmp);
	string x, y, z;//x:���� y:���� z:��С
	Tables* nxt = new Tables;//�½�һ����̬��nxt
	nxt->name = tableName;//��ֵ����
	while (check >> x) {
		check >> y >> z;
		nxt->colName.push_back(x);
		nxt->type.push_back(y);
		nxt->size.push_back(atoi(z.c_str()));//atoi():��stringת��Ϊint
	}
	nxt->pathName = pathName;
	tab.push_back(nxt);//
	fclose(tempfptr);//�ر��ļ�
	return tab.size() - 1;
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
	stringstream ss(value);
	tab[pos]->fp = fopen(tab[pos]->pathName.c_str(), "a");//�򿪱�
	for (int i = 0; i < tab[pos]->type.size(); i++) {
		string tmp = tab[pos]->type[i];//�����͸���tmp
		//cout << "tmp:" << tmp << endl;
		if (tmp == "int") {
			int x; ss >> x;
			fprintf(tab[pos]->fp, "%d", x);
		}
		else if (tmp == "float") {
			float x; ss >> x;
			fprintf(tab[pos]->fp, "%f", x);
		}
		else if (tmp == "double") {
			double x; ss >> x;
			fprintf(tab[pos]->fp, "%f", x);
		}
		else if (tmp == "char") {
			if (tab[pos]->size[i] == 1) {
				char x; ss >> x;
				fprintf(tab[pos]->fp, "%c", x);
			}
			else {
				int cnt = tab[pos]->size[i];
				char* x = new char[cnt];
				ss >> x;
				fprintf(tab[pos]->fp, "%s", x);
				delete x;
			}
		}
		if (i != tab[pos]->type.size() - 1)
			fprintf(tab[pos]->fp, "%c", ' ');
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

	if (isWhere == "") {//ȫ���
		int len = strlen(contant);
		for (int i = 0; i < len; i++)
			if (contant[i] == ';')
				contant[i] = ' ';
		stringstream tmp(toColName);
		string x,y;
		int sign[20],j,num=0;
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
                    //cout<<"sign:"<<*sign<<' ';
                    num++;
                }
                ss >> x;//����
                ss >> x;//��С
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
			stringstream out(contant);
			string x;
			int i,n=num,j=0;
			for(i=0;i<tab[pos]->type.size(),n!=0;i++) {
                out >> x;
                //cout<<x<<" ";
                if(i==sign[j]){
                    int width = 15;
                    if (tab[pos]->size[i] != 1)
                        width = tab[pos]->size[i];
                    string tmp = tab[pos]->type[i];
                    //cout<<tmp;
                    if (tmp == "int")
                        cout << left << setw(width) << atoi(x.c_str());
                    else if (tmp == "float" || tmp == "double")
                        cout << left << setw(width) << atof(x.c_str());
                    else if (tmp == "char")
                        cout << left << setw(width) << x;
                    j++;
                    n--;
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

void myDBMS::connect(string tableName1,string tableName2,string colName1,string colName2){
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
	char contant[1024];
	int num[40];
	int n = 0;
	int v1,v2;

	tab[pos1]->fp = fopen(tab[pos1]->pathName.c_str(), "r");
	fgets(contant, sizeof(contant), tab[pos1]->fp);
	for(int i=0;i<strlen(contant);i++){
        if(contant[i]==';')contant[i]=' ';
	}
	stringstream ss1(contant);
	string str1;
	for (int v = 0; v < tab[pos1]->size.size(); v++){
        ss1 >> str1;
        if(str1 == colName1)v1=v;
        int width = 15;
        if (tab[pos1]->size[v] != 1)
            width = tab[pos1]->size[v];
        //cout<<width;
        cout << left << setw(width) << str1;//����벢���ÿ�����
        ss1 >> str1;//����
        ss1 >> str1;//��С
        num[n] = atoi(str1.c_str());
        n++;
	}
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
	for(int i=0;i<strlen(contant);i++){
        if(contant[i]==';')contant[i]=' ';
	}
	stringstream ss2(contant);
	string str2;
	for (int v = 0; v < tab[pos2]->size.size(); v++){
        ss2 >> str2;
        if(str2 == colName2)v2=v;
        int width = 15;
        if (tab[pos1]->size[v] != 1)
            width = tab[pos1]->size[v];
        //cout<<width;
        cout << left << setw(width) << str2;//����벢���ÿ�����
        ss2 >> str2;//����
        ss2 >> str2;//��С
        num[n] = atoi(str2.c_str());
        n++;
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
	cout<<endl;

	vector<vector<string>> dimvalue;
	dimvalue.push_back(t1);
	dimvalue.push_back(t2);
	vector<string> tmp;
	vector<vector<string>> res;

	int layer = 0,v[tab[pos1]->size.size()*tab[pos2]->size.size()],z=0;
	string x,x1,x2;
	productImplement(dimvalue, res, layer, tmp);
	for (int i = 0; i < res.size(); i++){
		stringstream ss1(res[i][0]);
		for(int j=0;j<v1;j++){
            ss1>>x;
		}
		x=x1;
		stringstream ss2(res[i][0]);
		for(int j=0;j<v2;j++){
            ss2>>x;
		}
		x=x2;
		if(x1==x2){
            v[z]=i;
            z++;
		}
		cout<<"";
	}
	z=0;
	//cout<<"";
	for(int i=0;i<res.size();i++){
        if(i==v[z]){
            int k=0;
            for (int j = 0; j < res[i].size(); j++){
                //cout<<res[i][j]<<endl;
                stringstream ss(res[i][j]);
                string x;
                if(num[k]<15)num[k]=15;
                while(ss>>x){
                    cout << std::left << setw(num[k]) << x ;
                    k++;
                }
            }
            cout << endl;
        }
	}
}

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
			string tmp, name, value;
			cin >> db.cmd >> name >> tmp;
			value = tmp.substr(7);
			vector<string>colName;
			int pos = name.find('(');
			for (int i = 0; i < value.length(); i++)
				if (value[i] == ',' || value[i] == '(' || value[i] == ')')
					value[i] = ' ';
			//cout << "value:" << value << endl;
			db.myInsert(name.substr(0, pos), value);
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
                db.connect(t1,t2,c1,c2);
			}else{
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
		else {
			string tmp;
			getline(cin, tmp);
			cout << "�����������,����!" << endl;
		}

	}
	return 0;
}
//select all from student,course
