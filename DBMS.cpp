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

const int inf = 0x3f3f3f3f;//int型整数的最大值

struct Tables {
	string name;//表名
	string pathName;//存储路径名称
	vector<string>colName;//列名称
	vector<string>type;//类型
	vector<string>key;//主键
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
		cout << "            列1(字段)名 数据类型1(大小:若为1可省略) primarykey(若不是可省略)"  << endl;
		cout << "            列2(字段)名 数据类型2(大小:若为1可省略) primarykey" << endl;
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
	void myInsertwithCol(string,string,string); //插入数据(带列名)
	void myDelete(string,string); //删除数据
	void myUpdate(string, string, string, string); //更新数据
	void myQuery(string, string, string); //查询数据
	void AddColumn(string);//增加表中的列
	void DropColumn(string);//删除表中的列
	void connect(string,string,string,string,string);
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
	string pathName = "E:\\CS346\\DBMS_Course_Desgin\\DB\\" + dataBaseName;//拼接存储路径
	//检查数据库是否存在
	if (0 != access(pathName.c_str(), 0))printf("该数据库不存在\n");//c_str:把C++中的string转换成C的字符串,access函数用于判断目标文件是否存在
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
	string pathName = "E:\\CS346\\DBMS_Course_Desgin\\DB\\" + dataBaseName;
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
	string pathName = "E:\\CS346\\DBMS_Course_Desgin\\DB\\" + dataBaseName;
	if (0 == access(pathName.c_str(), 0)) {//如果查询到该数据库
		pathName = "rd " + pathName;//rd:删除目录
		if (0 == system(pathName.c_str()))//system(),发出一个dos命令，这里rd命令为删除指令，返回值为0或-1
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
	vector<string>key;//主键
	vector<int>size;//大小
	string tmp;
	getchar();//读去回车
	//getline:读取整行，包括前导和嵌入的空格，并将其存储在字符串对象中
	getline(cin, tmp);//读（这一行
	getline(cin, tmp);//读列名，字段名,是否主键
	while (tmp != ")") {//当没有读到最后一行
		stringstream ss(tmp);//将数据传递给一个stringstream 对象
		string x;
		ss >> x; 
		colName.push_back(x);//通过该stringstream对象将数值赋给一个string对象并加到vector的最后面
		ss >> x;//再次赋值
		//识别(大小)
		int pos = x.find('(');//find返回的是索引，寻找的是流中标注长度的左括号
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
		string iskey;
		ss>>iskey;
		if(iskey=="primarykey"){
			key.push_back("isKey");
		}
		else{
			key.push_back("notKey");
		}
		getline(cin, tmp);//获取下一行
	}
	tableName += ".txt";//将表存为.TXT文件
	string pathName = prePath + tableName;//拼接数据库的文件路径和表名
	//cout << pathName.c_str() << endl;
	if (0 != access(pathName.c_str(), 0)) {//如果没有这个表
		Tables* ptr = new Tables;//新建一个表的动态空间
		ptr->name = tableName;
		ptr->pathName = pathName;
		ptr->colName = colName;
		ptr->type = type;
		ptr->size = size;
		ptr->key =key;
		ptr->fp = fopen(pathName.c_str(), "w");
		string wrin;
		wrin.clear();
		//将该表的结构存在一行里
		for (int i = 0; i < colName.size(); i++)
			if(key[i]=="isKey"){
				wrin += colName[i] + ";" + type[i] + ";" + to_string(size[i]) + ";"+"isKey"+";";
			}else{
            //以 列名;类型;大小; 的形式存入文件
			wrin += colName[i] + ";" + type[i] + ";" + to_string(size[i]) + ";"+"notKey"+";";
			}
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
				tab.erase(tab.begin() + i);//删除该位置tables，容器位置前移
			}
		remove(pathName.c_str());//删除表对应的文件
		cout << "删除成功!" << endl;
	}
}

//alter table table_name
//add column_name datatype(size)
void myDBMS::AddColumn(string tableName){
	if(!open){
		cout << "无选中数据库!" << endl;
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
			//把string形式的数字转换为int型
		for (int i = pos + 1; i < x.length() - 1; i++)
			num = num * 10 + x[i] - '0';
		tab[pos]->size.push_back(num);//把大小加入size的最后
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
            //以 列名;类型;大小; 的形式存入文件
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
		cout<<"添加列成功！"<<endl;
		return;
	}
	cout<<"该表不存在";
}

//alter table table_name
//drop column column_name
void myDBMS::DropColumn(string tableName){
	if(!open){
		cout << "无选中数据库!" << endl;
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
            		//以 列名;类型;大小; 的形式存入文件
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
		cout<<"删除列成功！"<<endl;
		return;
	}
	cout<<"该表不存在";
}

//辅助函数,参数：表名,用于读一个txt表文件并读入tab容器中
int myDBMS::posIsNos(string tableName) {
	string pathName = prePath + tableName + ".txt";
	if (0 != access(pathName.c_str(), 0)) {
		cout << "该表不存在!" << endl;
		return -1;
	}
	//以读的形式打开表
	FILE* tempfptr = fopen(pathName.c_str(), "r");
	char contant[1024];
	fscanf(tempfptr, "%s", contant);
	string tmp = contant;
	//把;改为‘ ’
	for (int i = 0; i < tmp.size(); i++)
		if (tmp[i] == ';')
			tmp[i] = ' ';//核心代码，将表的关系模式部分;分隔符改为空格
	stringstream check(tmp);
	string x, y, z, q;//x:列名 y:类型 z:大小 q:主键情况
	Tables* nxt = new Tables;//新建一个动态表nxt
	nxt->name = tableName;//赋值表名
	while (check >> x) {
		check >> y >> z>> q;
		nxt->colName.push_back(x);
		nxt->type.push_back(y);
		nxt->size.push_back(atoi(z.c_str()));//atoi():把string转换为int
		if(q=="isKey"){
			nxt->key.push_back("isKey");
		}
		else{
			nxt->key.push_back("notKey");
		}
	}
	nxt->pathName = pathName;
	tab.push_back(nxt);//
	fclose(tempfptr);//关闭文件
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

	tab[pos]->fp = fopen(tab[pos]->pathName.c_str(), "r");//打开表
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
        memset(contant, 0, sizeof(contant));//sentence初始化为0
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
        cout<<"插入失败"<<endl;
	}else{
        stringstream ss(value);
        for (int i = 0; i < tab[pos]->type.size(); i++) {
            string tmp = tab[pos]->type[i];//把类型赋给tmp
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
            tab[pos]->fp = fopen(tab[pos]->pathName.c_str(), "a");//打开表
            for (int i = 0; i < tab[pos]->type.size(); i++) {
                string tmp = tab[pos]->type[i];//把类型赋给tmp
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
            fprintf(tab[pos]->fp, "%c", '\n');//换行
            cout<<"插入成功"<<endl;
        }else{
			
            cout<<"插入失败"<<endl;
        }
        fclose(tab[pos]->fp);
	}
}
void myDBMS::myInsertwithCol(string tableName,string fieldname,string value) {
	vector<string>fieldlist;
	vector<string>valuelist;
	int flag;
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
	tab[pos]->fp = fopen(tab[pos]->pathName.c_str(), "a");//打开表
	for (int i = 0; i < tab[pos]->type.size(); i++) {
		string tmp = tab[pos]->type[i];//把类型赋给tmp
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
	stringstream ss(contant);
	string x;
	int keyPos = 0;

	if (isWhere == "") {//全输出
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
                    size[num] = tab[pos]->size[v];
                    type[num] = tab[pos]->type[v];
                    //cout<<"sign:"<<*sign<<' ';
                    num++;
                    break;
                }
                ss >> x;//类型
                ss >> x;//大小
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
//select all from student,sc where student.Sno = sc.Sno
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

void myDBMS::connect(string toColName,string tableName1,string tableName2,string colName1,string colName2){
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
			string tmp, name, value,fieldname;
			cin >> db.cmd >> name >> tmp;
			//cout<<"db.cmd:"<<db.cmd<<" name:"<<name<<" tmp:"<<tmp<<endl;
			value = tmp.substr(7);//截到values(
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
			cout << "输入命令错误,请检查!" << endl;
		}

	}
	return 0;
}
//select all from student,course
