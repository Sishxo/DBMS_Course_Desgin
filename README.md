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
dmc
1.增加了string key，在create表语句里面的最后一行使用，但是只能写作primarykey，中间不能有空格，文件打开也可以看到primarykey的内容
2.修改了insert函数，可以判断char类型和int类型是否超过限制的范围。
