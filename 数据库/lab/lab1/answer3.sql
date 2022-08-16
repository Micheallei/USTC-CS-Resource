#1
SELECT ID,address FROM reader WHERE name='Rose';
#2
SELECT book.name,Borrow_Date FROM book,reader,borrow 
WHERE borrow.book_ID=book.ID AND borrow.Reader_ID=reader.ID AND reader.name='Rose';
#3
SELECT reader.name FROM reader 
WHERE reader.ID NOT IN(SELECT distinct Reader_ID FROM borrow);
#4
SELECT name,price FROM book WHERE author='Ullman';
#5
SELECT book.ID,book.name FROM reader,book,borrow
WHERE borrow.book_ID=book.ID AND borrow.Reader_ID=reader.ID AND reader.name='李林' AND borrow.Return_Date IS NULL ;
#6
SELECT reader.name FROM reader WHERE reader.ID IN(
    SELECT borrow.Reader_ID FROM borrow
    GROUP BY borrow.Reader_ID
    HAVING COUNT(borrow.book_ID)>3
);
#7，先取出李林借阅的所有书，再看哪些人借了在其中的书，再排除
SELECT reader.name,reader.ID FROM reader
WHERE reader.ID NOT IN (SELECT distinct reader.ID FROM reader,borrow WHERE borrow.Reader_ID=reader.ID AND book_ID IN(
    SELECT borrow.book_ID FROM borrow,reader WHERE borrow.Reader_ID=reader.ID AND reader.name='李林'
));

#8
SELECT name,ID FROM book WHERE name LIKE '%Oracle%';


#9 最近一年里借过书的读者的读者号以及这一年里借阅不同图书数
CREATE VIEW reader_borrow(reader_id,reader_name,book_id,book_name,borrow_date)
As SELECT reader.ID,reader.name,book.ID,book.name,borrow.Borrow_Date FROM book,reader,borrow
WHERE borrow.book_ID=book.ID AND borrow.Reader_ID=reader.ID;

SELECT reader_id,Count(book_id) FROM reader_borrow a
WHERE year(now())-year(borrow_date)<=1
GROUP BY reader_id
