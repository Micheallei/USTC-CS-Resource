delimiter //
CREATE PROCEDURE Check_status(OUT count INT,OUT state INT)
BEGIN
    DECLARE bid CHAR(8);
    DECLARE bdate DATE;
    DECLARE stat INT;
    DECLARE rdate DATE Default NULL;
    DECLARE ct CURSOR FOR (
        SELECT ID, MAX(status) as status, MAX(Borrow_Date) as br_date FROM
        (SELECT book.ID,book.status,borrow.Borrow_Date,borrow.Return_Date FROM book ,borrow WHERE book.id=borrow.book_ID) as temp
        GROUP BY ID);#把每本书的ID,status以及最后一次借出的日期取出来
    DECLARE CONTINUE HANDLER FOR NOT FOUND SET state= 1;
    OPEN ct;
    Set count = 0;
    Set state=0;
    REPEAT
        FETCH ct INTO bid,stat,bdate;
        IF state=0 THEN
            SELECT Return_Date FROM borrow WHERE book_ID=bid AND Borrow_Date=bdate INTO rdate;
            IF rdate IS NULL AND stat=0 THEN
                Set count = count +1;
            ELSEIF rdate IS NOT NULL AND stat=1 THEN
                Set count = count +1;
            END IF;
        END IF;
        UNTIL state=1
    END REPEAT;
    CLOSE ct;
    Select COUNT(*) From book WHERE status=1 AND ID NOT IN (
      Select distinct book_ID FROM borrow
    ) INTO @acount;
    Set count=count+@acount;
END //
delimiter ;



call Check_status(@count,@state);
SELECT @state ;
SELECT @count ;
