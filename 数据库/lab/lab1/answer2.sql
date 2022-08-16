#实体完整性
insert into Borrow value(NULL, 'r5', '2021-04-10', NULL);
insert into Borrow value('b4', NULL, '2021-04-10', NULL);


#参照完整性
Update book Set ID='b0' Where ID='b11';
insert into Borrow value('b0', 'r3', '2021-04-02', NULL);



#用户自定义完整性
insert into Book value('b1', NULL, 'Ullman', 59.0, 1);