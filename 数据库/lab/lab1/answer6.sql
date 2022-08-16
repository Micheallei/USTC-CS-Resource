#借书
DELIMITER //
DROP TRIGGER IF EXISTS updateStatus1;
CREATE TRIGGER updateStatus1 AFTER INSERT ON borrow FOR EACH ROW
BEGIN 
    UPDATE book SET status=1 WHERE ID=new.book_ID;
END //
DELIMITER ;

#还书
DELIMITER //
DROP TRIGGER IF EXISTS updateStatus2;
CREATE TRIGGER updateStatus2 AFTER UPDATE ON borrow FOR EACH ROW
BEGIN 
    UPDATE book SET status=0 WHERE ID=new.book_ID;
END //
DELIMITER ;



Update Borrow Set Return_Date=Str_to_Date('15,4,2021','%d,%m,%Y')
WHERE book_ID='b9' AND Reader_ID='r3';

insert into Borrow value('b9', 'r3', '2021-04-016',NULL);

