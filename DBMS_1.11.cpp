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

const int inf = 0x3f3f3f3f;//int型整数的最大值

struct Tables {
	string name;//表名
	string pathName;//存储路径名称
	vector<string>colName;//列名称
	vector<string>type;//类型
	vector<int>size;//大小
	FILE* fp;//文件指针
};

class myDBMS {
	vector<Tables*>tab;
	bool open;//是否打开
public:
	string cmd;//sql语句
	string prePath;//数据库的文件路径

	myDBMS() {
		cmd = "";//初始化
		prePath = "";
		open = false;//初始化数据库未打开

		//建库删库,建表删表,增删改查
		cout << "请按以下规则输入命令语句(命令不分大小写)" << endl;
		cout << "新建数据库: create database 数据库名字" << endl;
		cout << "删除数据库: drop database 数据库名字" << endl;
		cout << "打开数据库: open database 数据库名字" << endl;
		cout << "关闭数据库: close database 数据库名字" << endl;
		cout << "添加新表  : create table  表名" << endl;
		cout << "            (" << endl;
		cout << "            列1(字段)名 数据类型1(大小:若为1可省略)" << endl;
		cout << "            列2(字段)名 数据类型2(大小:若为1可省略)" << endl;
		cout << "            …          …" << endl;
		cout << "            )" << endl;
		cout << "删除旧表  : drop table 表名" << endl;
		cout << "查看全表  : select all from 表名" << endl;
		cout << "简单查询  : select 列名 from 表名 where 列名 = 值(值可为all,表全部范围)" << endl;
		cout << "            select all from 表名1，表名2 where 表名1.列名1 = 表名2.列名2" << endl;
		cout << "插入      : insert into 表名(field1,field2,…) values(value1,value2,…)" << endl;
		cout << "修改      : update 列名 = 新值 from 表名 where 列名 = 值(值可为all,表全部范围)" << endl;
		cout << "删除      : delete from 表名 where 列名 = 值(值可为all,表全部范围)" << endl;
		cout << "退出程序  : exit" << endl;
	}
	//撤销类对象时，调用析构函数
	~myDBMS() {
		for (int i = 0; i < tab.size(); i++) {//当表可以打开时，关闭打开的表并delete掉
			if (tab[i]->fp != NULL)
				fclose(tab[i]->fp);
			delete tab[i];
		}
		tab.resize(0);//重新设置容器大小为0
		cmd = "";
		prePath = "";
	}

	void transfer();//转为小写
	void openDataBase(string);//打开数据库
	void closeDataBase();//关闭数据库
	void myCreateDataBase(string); //建立数据库
	void myDropDataBase(string); //删除数据库
	void myCreateTable(string); //创建表
	void myDropTable(string); //删除表
	void myInsert(string,string); //插入数据
	void myDelete(string,string); //删除数据
	void myUpdate(string, string, string, string); //更新数据
	void myQuery(string, string, string); //查询数据
	void connect(string,string,string,string);
	void productImplement(vector<vector<string>> dimvalue,vector<vector<string>> &res,int,vector<string> tmp);
	string get_toColName(string);//获取toColName

	int posIsNos(string);//辅助函数
};

//转为小写
void myDBMS::transfer() {//把sql语句中的字母全部转换为小写
	for (int i = 0; i < cmd.size(); i++)
		cmd[i] = tolower(cmd[i]);
}

//打开数据库，参数为数据库名
void myDBMS::openDataBase(string dataBaseName) {
	if (open) {//如果已有数据库打开
		cout << "请先关闭当前打开的数据库" << endl;
		return;
	}
	string pathName = "C:\\Users\\stell\\Desktop\\数据库原理课程设计文档202012\\DBMS\\" + dataBaseName;//拼接存储路径
	//检查数据库是否存在
	if (0 != access(pathName.c_str(), 0))printf("该数据库不存在\n");//c_str:把C++中的string转换成C的字符串
	else {
		prePath = pathName + "\\";
		cout << "打开数据库成功" << endl;
		open = true;
	}
}

//关闭数据库，和析构函数相同
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

//建立数据库
void myDBMS::myCreateDataBase(string dataBaseName) {
	string pathName = "C:\\Users\\stell\\Desktop\\数据库原理课程设计文档202012\\DBMS\\" + dataBaseName;
	//先判断是否有该数据库存在
	if (0 != access(pathName.c_str(), 0)) {
        //返回0表示创建成功,-1表示失败,mkdir(path,mode)创建目录,mode默认0777（允许全局访问）
		if (0 == mkdir(pathName.c_str()))
			cout << "创建成功" << endl;
		else
			cout << "创建失败" << endl;
		return;
	}
	cout << "该数据库已存在" << endl;
}

//删除数据库
void myDBMS::myDropDataBase(string dataBaseName) {
	string pathName = "C:\\Users\\stell\\Desktop\\数据库原理课程设计文档202012\\DBMS\\" + dataBaseName;
	if (0 == access(pathName.c_str(), 0)) {//如果查询到该数据库
		pathName = "rd " + pathName;//rd:删除目录
		if (0 == system(pathName.c_str()))
			cout << "删除数据库" << dataBaseName << "成功" << endl;
		else
			cout << "删除数据库" << dataBaseName << "失败" << endl;
		return;
	}
	cout << "数据库" << dataBaseName << "不存在" << endl;
}

//建立表,参数:表名
/*
create table 表名
(
列1(字段)名 数据类型1(大小:若为1可省略),
列2(字段)名 数据类型2(大小:若为1可省略),
…          …
)
*/
void myDBMS::myCreateTable(string tableName) {
	vector<string>colName;//列名
	vector<string>type;//类型
	vector<int>size;//大小
	string tmp;
	getchar();//读（
	//getline:读取整行，包括前导和嵌入的空格，并将其存储在字符串对象中
	getline(cin, tmp);//读（这一行
	getline(cin, tmp);//读列名，字段名
	while (tmp != ")") {//当没有读到最后一行
		stringstream ss(tmp);//将数据传递给一个stringstream 对象
		string x;
		ss >> x; colName.push_back(x);//通过该stringstream对象将数值赋给一个string对象并加到vector的最后面
		ss >> x;//再次赋值
		//识别(大小)
		int pos = x.find('(');
		if (pos == string::npos) {//find()在找不到指定值的情况下会返回string::npos
			type.push_back(x);//没有(则直接把x加到type的最后面
			size.push_back(1);//把默认值1加到size的后面
		}
		else {//如果标注了大小
			type.push_back(x.substr(0, pos));//把到（之前的内容加入type
			int num = 0;
			//把string形式的数字转换为int型
			for (int i = pos + 1; i < x.length() - 1; i++)
				num = num * 10 + x[i] - '0';
			size.push_back(num);//把大小加入size的最后
		}
		getline(cin, tmp);//获取下一行
	}
	tableName += ".txt";
	string pathName = prePath + tableName;//拼接数据库的文件路径和表名
	//cout << pathName.c_str() << endl;
	if (0 != access(pathName.c_str(), 0)) {//如果没有这个表
		Tables* ptr = new Tables;//新建一个表的动态空间
		ptr->name = tableName;
		ptr->pathName = pathName;
		ptr->colName = colName;
		ptr->type = type;
		ptr->size = size;
		ptr->fp = fopen(pathName.c_str(), "w");
		string wrin;
		wrin.clear();
		//将该表的结构存在一行里
		for (int i = 0; i < colName.size(); i++)
            //以 列名;类型;大小; 的形式存入文件
			wrin += colName[i] + ";" + type[i] + ";" + to_string(size[i]) + ";";
		wrin += "\n";//换行
		fprintf(ptr->fp,wrin.c_str());//写入文件
		fclose(ptr->fp);//关闭文件
		tab.push_back(ptr);//在table容器中加入这个新表
		cout << "创建新表成功!" << endl;
		return;
	}
	cout << "该表已经存在!" << endl;
}

//删除表，参数:表名
void myDBMS::myDropTable(string tableName) {
	tableName += ".txt";
	string pathName = prePath + tableName;
	if (0 != access(pathName.c_str(), 0))
		cout << "该表不存在!" << endl;
	else {
		for (int i = 0; i < tab.size(); i++)
			if (tab[i]->name == tableName) {//找到相应的表
			    //如果表开着就关闭文件然后delete掉
				if (tab[i]->fp != NULL)
					fclose(tab[i]->fp);
				delete tab[i];
                //从tab容器中删除该表
				tab.erase(tab.begin() + i);
			}
		remove(pathName.c_str());//删除表对应的文件
		cout << "删除成功!" << endl;
	}
}

//辅助函数,参数：表名
int myDBMS::posIsNos(string tableName) {
	string pathName = prePath + tableName + ".txt";
	if (0 != access(pathName.c_str(), 0)) {
		cout << "该表不存在!" << endl;
		return -1;
	}
	//以读的形式打开表
	FILE* tempfptr = fopen(pathName.c_str(), "r");
	char contant[100];
	fscanf(tempfptr, "%s", contant);
	string tmp = contant;
	//把;改为‘ ’
	for (int i = 0; i < tmp.size(); i++)
		if (tmp[i] == ';')
			tmp[i] = ' ';
	stringstream check(tmp);
	string x, y, z;//x:列名 y:类型 z:大小
	Tables* nxt = new Tables;//新建一个动态表nxt
	nxt->name = tableName;//赋值表名
	while (check >> x) {
		check >> y >> z;
		nxt->colName.push_back(x);
		nxt->type.push_back(y);
		nxt->size.push_back(atoi(z.c_str()));//atoi():把string转换为int
	}
	nxt->pathName = pathName;
	tab.push_back(nxt);//
	fclose(tempfptr);//关闭文件
	return tab.size() - 1;
}

//插入数据,参数:表名,
//insert into 表名(field1,field2,…) values(value1,value2,…)
//insert into student(Sno,Sname) values(100,wang)
void myDBMS::myInsert(string tableName,string value) {
	if (!open) {//如果没有数据库打开
		cout << "无选中数据库!" << endl;
		return;
	}
	int pos = inf;
	for (int i = 0; i < tab.size(); i++)//定位表在容器中的位置
		if (tab[i]->name == tableName) {
			pos = i; break;
		}
	if (pos == inf)//如果没有找到表
		pos = posIsNos(tableName);
	if (pos == -1)return;
	stringstream ss(value);
	tab[pos]->fp = fopen(tab[pos]->pathName.c_str(), "a");//打开表
	for (int i = 0; i < tab[pos]->type.size(); i++) {
		string tmp = tab[pos]->type[i];//把类型赋给tmp
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
	fprintf(tab[pos]->fp, "%c", '\n');//换行
	fclose(tab[pos]->fp);
	cout << "插入成功!" << endl;
}

//删除数据
//delete from 表名 where 列名 = 值(值可为all,表全部范围)
void myDBMS::myDelete(string tableName,string isWhere) {
	if (!open) {
		cout << "无选中数据库!" << endl;
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
		if (tab[pos]->colName[i] == typeName) {//列名相等
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
	//判断是否为all
	bool flag = true;
	bool aru = false;
	if (toValue.length() == 3 && tolower(toValue[0]) == 'a' && tolower(toValue[1]) == 'l' && tolower(toValue[2]) == 'l')
		flag = false;
    //总体思路是新建一个表tmp.txt，里面存放除符合where条件以外的记录，然后对tmp.txt改名，并移除原来的文件
	if (flag) {
		while (!feof(tab[pos]->fp)) {
			memset(sentence, 0, sizeof(sentence));//sentence初始化为0
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
        cout << "影响0条记录" <<endl;
    if (0 == rename(pathName2.c_str(), tab[pos]->pathName.c_str()))
        cout << "删除成功!" << endl;
    else
        cout << "删除失败!" << endl;
}

//更新数据
//update 列名 = 新值 from 表名 where 列名 = 值(值可为all,表全部范围)
void myDBMS::myUpdate(string tableName, string toColName,string newValue, string isWhere) {
	if (!open) {
		cout << "无选中数据库!" << endl;
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
	for (int i = 0; i < tab[pos]->colName.size(); i++) //找到范围对应的colName下标
		if (tab[pos]->colName[i] == typeName) {
			wherePos = i;
			break;
		}
	for (int i = 0; i < tab[pos]->colName.size(); i++) //找到需要修改的colName下标
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
	//读原文件的内容放到新文件
	bool flag = true;
	if (toValue.length() == 3 && tolower(toValue[0]) == 'a' && tolower(toValue[1]) == 'l' && tolower(toValue[2]) == 'l')
		flag = false;
	if (!flag) {//全部修改
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
						if (i == updataPos)//如果是要修改的列
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
				if (i != tab[pos]->type.size() - 1)//如果不是最后一项输入空格
					fprintf(tmpfptr, "%c", ' ');
			}
			fprintf(tmpfptr, "%c", '\n');
		}
	}
	else {//范围内修改
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
		cout << "更新成功!" << endl;
	else
		cout << "更新失败!" << endl;
}

//查询数据
void myDBMS::myQuery(string toColName, string tableName, string isWhere = "") {
	if (!open) {
		cout << "无选中数据库!" << endl;
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

	if (isWhere == "") {//全输出
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
            for (int v = 0; v < tab[pos]->size.size(); v++) {//先输出colNames
                ss >> x;
                //cout<<x<<y<<endl;
                if(x == y){
                    //确定输出数据显示的宽度
                    int width = 15;
                    if (tab[pos]->size[v] != 1)
                    width = tab[pos]->size[v];
                    cout << left << setw(width) << x;//左对齐并设置宽度输出
                    sign[num] = v;
                    //cout<<"sign:"<<*sign<<' ';
                    num++;
                }
                ss >> x;//类型
                ss >> x;//大小
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
	else {//部分输出
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
			string x, check, out;//out:存放需要输出的列
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

//获取所有列名
string myDBMS::get_toColName(string tableName){
    if (!open) {
		cout << "无选中数据库!" << endl;
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
		for (int i = 0; i < dimvalue[layer].size(); i++){//对每一个容器
			vector<string> sb;//新建一个容器sb
			sb.clear();//清空容器

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
		cout << "无选中数据库!" << endl;
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
        cout << left << setw(width) << str1;//左对齐并设置宽度输出
        ss1 >> str1;//类型
        ss1 >> str1;//大小
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
        cout << left << setw(width) << str2;//左对齐并设置宽度输出
        ss2 >> str2;//类型
        ss2 >> str2;//大小
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
			else cout << "命令语句有误!" << endl;
		}
		else if (db.cmd == "drop") {
			string name;
			cin >> db.cmd >> name;
			db.transfer();
			if (db.cmd == "database")
				db.myDropDataBase(name);
			else if (db.cmd == "table")
				db.myDropTable(name);
			else cout << "命令语句有误!" << endl;
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
			else cout << "命令语句有误!" << endl;
		}
		else if (db.cmd == "insert") {
			//insert into 表名(field1,field2,…) values(value1,value2,…)"
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
			//delete from 表名 where 范围
			string name;
			cin >> db.cmd >> name;
			string isWhere;
			getline(cin, isWhere);
			db.myDelete(name, isWhere);
		}
		else if (db.cmd == "update") {
			//update 列名 = 新值 from 表名 where 列名 = 值(值可为all, 表全部范围)
			string toColName,deng, newValue, tableName, isWhere;
			cin >> toColName >> deng >> newValue >> db.cmd >> tableName;
			getline(cin, isWhere);
			db.myUpdate(tableName, toColName, newValue, isWhere);
		}
		else if (db.cmd == "select") {
			//"查看全表  : select all from 表名"
			//"查询单值  : select 列名 from 表名 where 列名 = 值(值可为all,表全部范围)"
			string toColName, tableName;
			cin >> toColName >> db.cmd >> tableName;
			string isWhere;
            getline(cin,isWhere);
            //cout<<isWhere<<endl;
			int flag=0;//判断是否需要做连接操作
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
			cout << "输入命令错误,请检查!" << endl;
		}

	}
	return 0;
}
//select all from student,course
