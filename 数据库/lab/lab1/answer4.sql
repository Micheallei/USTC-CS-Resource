delimiter //
CREATE PROCEDURE Change_bid(IN id_old CHAR (8), IN id_new CHAR (8), OUT state INT)
BEGIN
	DECLARE s INT DEFAULT 0;
    DECLARE CONTINUE HANDLER FOR SQLEXCEPTION SET s=1;
    START TRANSACTION;
    SELECT name,author,price,status FROM book WHERE ID=id_old INTO @name ,@author ,@price ,@status ;
    INSERT INTO book (ID,name,author,price,status) Values(id_new, @name ,@author ,@price ,@status );
    Update borrow SET book_ID=id_new WHERE book_ID=id_old;
    DELETE FROM book WHERE ID=id_old;
    IF s=0 THEN
		SET state=0;
        COMMIT;
	ELSE
		SET state=-1000;
        ROLLBACK;
	END IF;
END //
delimiter ;