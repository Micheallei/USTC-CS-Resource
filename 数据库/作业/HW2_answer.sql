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