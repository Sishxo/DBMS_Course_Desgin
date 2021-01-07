#include <iostream>
#include <string>
#include <vector>
#include <cstdio>
#include <string>
#include <sstream>
#include <iomanip>

#include <direct.h>
#include <io.h>

#include <fstream>

using namespace std;

const int inf = 0x3f3f3f3f;

struct Tables {
	string name;
	string pathName;
	vector<string>colName;//������
	vector<string>type;
	vector<int>size;
	FILE* fp;
};

class myDBMS {
	vector<Tables*>tab;
	bool open;
public:
	string cmd;
	string prePath;//���ݿ���ļ�·��

	myDBMS() {
		cmd = "";
		prePath = "";
		open = false;
		//����ɾ��,����ɾ��,��ɾ�Ĳ�
		cout << "�밴���¹��������������(����ִ�Сд)" << endl;
		cout << "�½����ݿ�: create database ���ݿ�����" << endl;
		cout << "ɾ�����ݿ�: drop database ���ݿ�����" << endl;
		cout << "�����ݿ�: open database ���ݿ�����" << endl;
		cout << "�ر����ݿ�: close database ���ݿ�����" << endl;
		cout << "����±�  : create table  ����" << endl;
		cout << "            (" << endl;
		cout << "            ��1(�ֶ�)�� ��������1(��С:��Ϊ1��ʡ��)," << endl;
		cout << "            ��2(�ֶ�)�� ��������2(��С:��Ϊ1��ʡ��)," << endl;
		cout << "            ��          ��" << endl;
		cout << "            )" << endl;
		cout << "ɾ���ɱ�  : drop table ����" << endl;
		cout << "�鿴ȫ��  : query all from ����" << endl;
		cout << "��ѯ��ֵ  : query ���� from ���� where ���� = ֵ(ֵ��Ϊall,��ȫ����Χ)" << endl;
		cout << "����      : insert into ����(field1,field2,��) values(value1,value2,��)" << endl;
		cout << "�޸�      : update ���� = ��ֵ from ���� where ���� = ֵ(ֵ��Ϊall,��ȫ����Χ)" << endl;
		cout << "ɾ��      : delete from ���� where ���� = ֵ(ֵ��Ϊall,��ȫ����Χ)" << endl;
		cout << "�˳�����  : exit" << endl;
	}
	~myDBMS() {
		for (int i = 0; i < tab.size(); i++) {
			if (tab[i]->fp != NULL)
				fclose(tab[i]->fp);
			delete tab[i];
		}
		tab.resize(0);
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

	int posIsNos(string);//��������
};

void myDBMS::transfer() {
	for (int i = 0; i < cmd.size(); i++)
		cmd[i] = tolower(cmd[i]);
}

void myDBMS::openDataBase(string dataBaseName) {
	if (open) {
		cout << "���ȹرյ�ǰ�򿪵����ݿ�" << endl;
		return;
	}
	string pathName ="C:\\Users\\19146\\Desktop\\���ݿ�" + dataBaseName;
	if (0 != access(pathName.c_str(), 0))printf("�����ݿⲻ����");
	else {
		prePath = pathName + "\\";
		cout << "�����ݿ�ɹ�" << endl;
		open = true;
	}
}

void myDBMS::closeDataBase() {//������������ͬ
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

//C:\Users\TKwang\source\repos\Project1
void myDBMS::myCreateDataBase(string dataBaseName) {//�������ݿ�
	string pathName = "C:\\Users\\19146\\Desktop\\���ݿ�" + dataBaseName;
	if (0 != access(pathName.c_str(), 0)) {
		if (0 == mkdir(pathName.c_str()))//����0��ʾ�����ɹ�,-1��ʾʧ��
			cout << "�����ɹ�" << endl;
		else
			cout << "����ʧ��" << endl;
		return;
	}
	cout << "�����ݿ��Ѵ���" << endl;
}

void myDBMS::myDropDataBase(string dataBaseName) {//ɾ�����ݿ�
	string pathName = "C:\\Users\\19146\\Desktop\\���ݿ�" + dataBaseName;
	if (0 == access(pathName.c_str(), 0)) {
		pathName = "rd " + pathName;
		if (0 == system(pathName.c_str()))
			cout << "ɾ�����ݿ�" << dataBaseName << "�ɹ�" << endl;
		else
			cout << "ɾ�����ݿ�" << dataBaseName << "ʧ��" << endl;
		return;
	}
	cout << "���ݿ�" << dataBaseName << "������" << endl;
}

void myDBMS::myCreateTable(string tableName) {//������
	vector<string>colName;
	vector<string>type;
	vector<int>size;
	string tmp;
	getchar();
	getline(cin, tmp);
	getline(cin, tmp);
	while (tmp != ")") {
		stringstream ss(tmp);
		string x;
		ss >> x; colName.push_back(x);
		ss >> x;
		int pos = x.find('(');
		if (pos == string::npos) {
			type.push_back(x);
			size.push_back(1);
		}
		else {
			type.push_back(x.substr(0, pos));
			int num = 0;
			for (int i = pos + 1; i < x.length() - 1; i++)
				num = num * 10 + x[i] - '0';
			size.push_back(num);
		}
		getline(cin, tmp);
	}
	tableName += ".txt";
	string pathName = prePath + tableName;
	//cout << pathName.c_str() << endl;
	if (0 != access(pathName.c_str(), 0)) {
		Tables* ptr = new Tables;
		ptr->name = tableName;
		ptr->pathName = pathName;
		ptr->colName = colName;
		ptr->type = type;
		ptr->size = size;
		ptr->fp = fopen(pathName.c_str(), "w");
		string wrin;
		wrin.clear();
		for (int i = 0; i < colName.size(); i++)
			wrin += colName[i] + ";" + type[i] + ";" + to_string(size[i]) + ";";
		wrin += "\n";
		fprintf(ptr->fp,wrin.c_str());
		fclose(ptr->fp);
		tab.push_back(ptr);
		cout << "�����±�ɹ�!" << endl;
		return;
	}
	cout << "�ñ��Ѿ�����!" << endl;
}

void myDBMS::myDropTable(string tableName) {//ɾ����
	tableName += ".txt";
	string pathName = prePath + tableName;
	if (0 != access(pathName.c_str(), 0))
		cout << "�ñ�����!" << endl;
	else {
		for (int i = 0; i < tab.size(); i++)
			if (tab[i]->name == tableName) {
				if (tab[i]->fp != NULL)
					fclose(tab[i]->fp);
				delete tab[i];
				tab.erase(tab.begin() + i);
			}
		remove(pathName.c_str());
		cout << "ɾ���ɹ�!" << endl;
	}
}

int myDBMS::posIsNos(string tableName) {
	string pathName = prePath + tableName + ".txt";
	if (0 != access(pathName.c_str(), 0)) {
		cout << "�ñ�����!" << endl;
		return -1;
	}
	FILE* tempfptr = fopen(pathName.c_str(), "r");
	char contant[100];
	fscanf(tempfptr, "%s", contant);
	string tmp = contant;
	for (int i = 0; i < tmp.size(); i++)
		if (tmp[i] == ';')
			tmp[i] = ' ';
	stringstream check(tmp);
	string x, y, z;
	Tables* nxt = new Tables;
	nxt->name = tableName;
	while (check >> x) {
		check >> y >> z;
		nxt->colName.push_back(x);
		nxt->type.push_back(y);
		nxt->size.push_back(atoi(z.c_str()));
	}
	nxt->pathName = pathName;
	tab.push_back(nxt);
	fclose(tempfptr);
	return tab.size() - 1;
}

void myDBMS::myInsert(string tableName,string value) {//��������
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
	stringstream ss(value);
	string tmp2; ss >> tmp2;
	tab[pos]->fp = fopen(tab[pos]->pathName.c_str(), "a");
	for (int i = 0; i < tab[pos]->type.size(); i++) {
		string tmp = tab[pos]->type[i];
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
	fprintf(tab[pos]->fp, "%c", '\n');
	fclose(tab[pos]->fp);
	cout << "����ɹ�!" << endl;
}

void myDBMS::myDelete(string tableName,string isWhere) {//ɾ������
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
		if (tab[pos]->colName[i] == typeName) {
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
	bool flag = true;
	if (toValue.length() == 3 && tolower(toValue[0]) == 'a' && tolower(toValue[1]) == 'l' && tolower(toValue[2]) == '1')
		flag = false;
	if (flag) {
		while (!feof(tab[pos]->fp)) {
			memset(sentence, 0, sizeof(sentence));
			fgets(sentence, 1024, tab[pos]->fp);
			stringstream myTmp(sentence);
			//cout << "sentence " << sentence << endl;
			string x;
			for (int i = 0; i <= wherePos; i++)
				myTmp >> x;
			if (x == toValue) continue;
			fputs(sentence, tmpfptr);
		}
	}
	fclose(tmpfptr);
	fclose(tab[pos]->fp);
	remove(tab[pos]->pathName.c_str());
	if (0 == rename(pathName2.c_str(), tab[pos]->pathName.c_str()))
		cout << "ɾ���ɹ�!" << endl;
	else
		cout << "ɾ��ʧ��!" << endl;
}

void myDBMS::myUpdate(string tableName, string toColName,string newValue, string isWhere) {//��������
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
						if (i == updataPos)
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
				if (i != tab[pos]->type.size() - 1)
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

void myDBMS::myQuery(string toColName, string tableName, string isWhere = "") {//��ѯ����
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

	char contant[1024];
	//fscanf(tab[pos]->fp, "%s", contant);
	fgets(contant, sizeof(contant), tab[pos]->fp);
	if (isWhere == "") {//ȫ���
		int len = strlen(contant);
		for (int i = 0; i < len; i++)
			if (contant[i] == ';')
				contant[i] = ' ';
		stringstream ss(contant);
		string x;
		for (int i = 0; i < tab[pos]->size.size(); i++) {//�����colNames
			int width = 15;
			if (tab[pos]->size[i] != 1)
				width = tab[pos]->size[i];
			ss >> x;
			cout << left << setw(width) << x;
			ss >> x;
			ss >> x;
		}
		cout << endl;
		//fgets(contant, sizeof(contant), tab[pos]->fp);
		while(!feof(tab[pos]->fp)) {
			memset(contant, 0, sizeof(contant));
			fgets(contant, sizeof(contant), tab[pos]->fp);
			if (strlen(contant) == 0)break;
			stringstream out(contant);
			string x;
			for (int i = 0; i < tab[pos]->type.size(); i++) {
				out >> x;
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
			}
			cout << endl;
		}
	}
	else {
		stringstream ss(isWhere);
		int wherePos = inf, aimPos = inf;
		string typeName, toValue, whr, deng;
		ss >> whr >> typeName >> deng >> toValue;
		for (int i = 0; i < tab[pos]->colName.size(); i++) //�ҵ��涨��ΧcolName�±�
			if (tab[pos]->colName[i] == typeName) {
				wherePos = i;
				break;
			}
		for (int i = 0; i < tab[pos]->colName.size(); i++) //�ҵ��涨��ΧcolName�±�
			if (tab[pos]->colName[i] == toColName) {
				aimPos = i;
				break;
			}
		while (!feof(tab[pos]->fp)) {
			memset(contant, 0, sizeof(contant));
			fgets(contant, sizeof(contant), tab[pos]->fp);
			stringstream myTmp(contant);
			string x, check, out;
			for (int i = 0; i < tab[pos]->colName.size(); i++) {
				myTmp >> x;
				if (i == wherePos)check = x;
				if (i == aimPos)out = x;
			}
			if (check == toValue) {
				int width = 15;
				if (tab[pos]->size[aimPos] != 1)
					width = tab[pos]->size[aimPos];
				string tmp = tab[pos]->type[aimPos];
				if (tmp == "int")
					cout << left << setw(width) << atoi(out.c_str()) << endl;
				else if (tmp == "float")
					cout << left << setw(width) << atof(out.c_str()) << endl;
				else if (tmp == "double")
					cout << left << setw(width) << atof(out.c_str()) << endl;
				else if (tmp == "char")
					cout << left << setw(width) << out << endl;
			}
		}
	}
	fclose(tab[pos]->fp);
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
			string value, name;
			cin >> db.cmd >> name >> value;
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
		else if (db.cmd == "query") {
			//"�鿴ȫ��  : query all from ����"
			//"��ѯ��ֵ  : query ���� from ���� where ���� = ֵ(ֵ��Ϊall,��ȫ����Χ)"
			string toColName, tableName;
			cin >> toColName >> db.cmd >> tableName;
			if (db.cmd.length() == 3 && tolower(db.cmd[0]) == 'a' && tolower(db.cmd[1]) == 'l' && tolower(db.cmd[2]) == 'l') {
				db.myQuery(toColName, tableName);
			}
			else {
				string isWhere;
				getline(cin,isWhere);
				db.myQuery(toColName, tableName, isWhere);
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
