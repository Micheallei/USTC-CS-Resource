1. 查询姓名中含有“科” 字的学生学号和姓名  

   ```sql
   Select sno,sname From Student 
   Where sname LIKE '%科%'
   ```

2. 查询学分不低于 3 分的必修课课程号和课程名  

   ```sql
   Select cno,cname From Course
   Where credit >= 3 and type = 0
   ```

3. 查询选修了公选课但是缺少成绩的学生学号和姓名  

   ```sql
   Select Distinct a.sno,a.sname From Student a,SC b,Course c 
   Where a.sno = b.sno and b.cno = c.cno and c.type = 3 and b.score IS NULL
   ```

4. 查询年龄大于 20 的学生学号、姓名和年龄  

   ```sql
   Select sno,sname, year(now())-year(birthdate) as age From Student Where year(now())-year(birthdate) > 20
   ```

5. 查询已选必修课总学分大于 16 并且所选通识课成绩都大于 75 分的学生姓名  

   ```sql
   Create View greater16(sno) 
   As Select s.sno From SC s,Course c
   Where s.cno = c.cno and c.type = 0 and s.score >= 60 #这里要注意，要及格的必修课才能算学分
   Group By s.sno
   Having Sum(c.credit)>16;
   
   Create View greater75(sno) 
   As Select s.sno From SC s,Course c
   Where s.cno = c.cno and c.type = 2 
   Group By s.sno
   Having Min(s.score)>75;
   
   Select a.sname From Student a,greater16 b,greater75 c Where a.sno = b.sno and a.sno = c.sno;
   
   Drop View greater16;
   Drop View greater75;
   ```

6. 查询已经修完所有必修课且成绩合格的学生学号和姓名  

   ```sql
   Select sno,sname From Student Where NOT EXISTS
   (Select * From Course Where type = 0 and cno NOT IN
   (Select cno From SC Where sno = Student.sno and score >= 60))
   ```

   最底层是学生所有选了且成绩>60的课，不在这里面的且是必修课的课的集合应该为空

7. 查询总平均成绩排名在前 50%（向上取整）的学生中必修课平均分最高的前 10 位同学，要求返回这些学生的学号、姓名、必修课平均分以及课程总平均成绩（不足 10 位时则全部返回）  

   ```sql
   #学生总平均成绩
   CREATE VIEW total_avg(sno,total_avg)
   As SELECT sno,AVG(score) FROM sc GROUP BY sno;
   
   #学生必修课平均分
   CREATE VIEW M_avg(sno,m_avg)
   AS SELECT sc.sno, AVG(sc.score) FROM sc,course 
   WHERE sc.cno=course.cno AND course.type=0
   GROUP BY sc.sno;
   
   
   SELECT a.sno, a.sname, b.m_avg, c.total_avg 
   FROM (SELECT *,RANK() OVER (ORDER BY total_avg DESC ) rk FROM total_avg) c ,student a, M_avg b
   WHERE a.sno=b.sno AND a.sno=c.sno AND c.rk <= (SELECT CEIL(COUNT(*)/2) FROM student)
   ORDER BY b.m_avg DESC 
   LIMIT 10;
   
   
   DROP VIEW total_avg;
   DROP VIEW M_avg;
   
   
   #实际上，若不用RANK函数也可以自己计算排名：通过嵌套查询，有n个人分比你高，你就排n+1位
   ```

8. 查询每门课程的课程名、 课程类型、 最高成绩、最低成绩、平均成绩和不及格率，要求结果按通识课、必修课、选修课、公选课顺序排列（提示： 课程名可能有重名）  

   注：没有被任何学生选的课程不会参与计算、不会显示出来;     没有成绩的课程不视为不及格

   ```sql
   #这门课的课程号，以及选了这门课的学生里的课程最高分，最低分，平均分
   Create View course_score 
   As Select cno,MAX(score) as max,MIN(score) as min,AVG(score) as avg From SC Group By cno;
   
   CREATE View Fail_Rate(cno,fail_rate)
   As SELECT cno,SUM(CASE WHEN sc.score<60 then 1 else 0 END) / COUNT(sc.score)
   FROM sc GROUP BY cno;
   
   Select a.cname, a.type, b.max, b.min, b.avg, c.fail_rate
   From Course a, course_score b,Fail_Rate c
   Where a.cno = b.cno and b.cno = c.cno
   Order By field(type,2,0,1,3);
   
   Drop View course_score;
   Drop View Fail_Rate;
   ```

   

9. 查询存在课程重修不及格情况的学生学号、 姓名以及重修不及格的课程号和课程名  

   ```sql
   #创建存在课程重修的学生的学号，课程号以及第一学期
   Create View Again
   As Select sno,cno,MIN(term) as min_term From SC Group By sno,cno Having Count(*)>1;
   
   Create View Fail
   As Select sno,cno,term From SC Where score<60;
   
   #存在课程重修不及格情况的学生学号和课程号
   Create View Final
   As Select Distinct a.sno,a.cno From Again a,Fail b 
   Where a.sno=b.sno and a.cno=b.cno and a.min_term <> b.term;
   
   Select a.sno,a.sname,b.cno,b.cname From Student a,Course b,Final c
   Where a.sno = c.sno and b.cno = c.cno;
   
   Drop View Again;
   Drop View Fail;
   Drop View Final;
   ```

10. SC 表中重复的 sno 和 cno 意味着该学生重修了课程（在不同的学期里）， 现在我们希望删除学生重复选课的信息，只保留最近一个学期的选课记录以及成绩，请给出相应的SQL 语句  

    ```sql
    Create View max_t
    As Select sno, cno, MAX(term) as max_term From SC Group By sno,cno;
    
    Delete SC From SC, max_t
    Where SC.sno=max_t.sno and SC.cno=max_t.cno and SC.term <> max_t.max_term;
    ```

    