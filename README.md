# DBMS_Course_Desginb

This is a DBMS for Database System Course Desgin at ECUST.

2021/1/7
b.cpp:查看表内容
bms2.cpp：建立dat文件
dbms3.cpp:实现插入功能
a.cpp:不小心传上去的，可以忽略
现存问题：
插入第二条数据的时候会出问题，但是之后的插入都正常。
dat里有多个表的时候就会出错。
对sql语句里获取value值可能还不是最精确，可能会运行不出来。
现在用的例子是student:Sno int,Sname char(15)   course:Cno int,Cname char(15)  数据库是school。

2021/1/8
DBMS.C
增加了注释，原来的插入有问题，现已修正。
原来的查询只能实现单值，目前经过修改实现了类似于：select Sno,Sname from student where Sno = 100以及select Sno,Sname from student
未完成的查询包括：包含and、or语句，连接查询等
目前的存储结构：数据库（文件夹）、表（txt）
已完成的功能：建立删除数据库，建立删除表，插入删除记录(不包括复杂的where语句)，简单查询

2021/1/10
DBMS_1.10.C
实现了select all
初步实现了笛卡尔积
未完成的查询包括：包含and、or语句，连接查询等

2021/1/11
DBMS_1.11.C
连接操作能够实现：select all from 表名1.列名 = 表名2.列名
（我自己的电脑上codeBlocks编译后有时这个功能会出问题，但是再编译一下就好了，目前还在检查哪里出的问题）

2021/1/11
DBMS.cpp
原本的insert语句只能整行插入，fieldname部分不起作用
该次变更加入了insert into tablename values(value1,....)以及允许只插入部分field的insert语句

2021/1/12
新增文件dms 李颖 
1.增加了string key，在create表语句里面的最后一行使用，但是只能写作primarykey，中间不能有空格，文件打开也可以看到primarykey的内容
2.修改了insert函数，可以判断char类型和int类型是否超过限制的范围。

2021/1/12
合并了上一次变更dms部分的代码
重写了主键部分，实现了建表时主键的指定，可指定单个或多个主键
变更时记得及时merge最新的代码进度，避免产生无效代码，耗费工作量

2021/1/13
DDL部分
1.数据插入（已完成数据类型判断，以及防止主键冲突）
*数据结构：列名；类型；大小；是否为主键
数据类型判断，以及防止主键冲突我根据新的数据结构重新写了一下
报错的形式可能需要细化，现在是是统一反馈“插入失败”
2.数据更新
3.查询(以下为现在能运行的例子)
  select all from student
  select Sno from student
  select Sname from student
  select Sno,Sname from student(目前顺序还不能调换，有空再调)
  *以上语句都可以带简单的where查询，如where Sno = 200(注意要有空格)
  select all from student,sc where student.Sno = sc.Sno
  select Cno from student,sc where student.Sno = sc.Sno
  select Cno,score from student,sc where student.Sno = sc.Sno
4.建表、删表
5.数据删除
