#初始化三个table
CREATE TABLE Book(  
    ID CHAR (8) NOT NULL PRIMARY KEY,
    name VARCHAR (10) NOT NULL,
    author VARCHAR (10),
    price FLOAT,
    status INT DEFAULT 0);

CREATE TABLE Reader(  
    ID CHAR (8) NOT NULL PRIMARY KEY,
    name VARCHAR (10) ,
    age INT ,
    address VARCHAR (20));

CREATE TABLE Borrow(  
    book_ID CHAR (8) NOT NULL,
    Reader_ID CHAR (8) NOT NULL ,
    Borrow_Date DATE ,
    Return_Date DATE ,
    Constraint PK_Borrow PRIMARY KEY (book_ID,Reader_ID),
    Constraint FK_book FOREIGN KEY (book_ID) REFERENCES Book(ID),
    Constraint FK_reader FOREIGN KEY (Reader_ID) REFERENCES Reader(ID))