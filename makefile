q4:
	gcc q4.c sqlite3.c dist.c -DSQLITE_ENABLE_RTREE=1 -lpthread -ldl -o q4
q5:
	gcc q5.c sqlite3.c dist.c -DSQLITE_ENABLE_RTREE=1 -lpthread -ldl -o q5
q7:
	gcc q7.c sqlite3.c dist.c -DSQLITE_ENABLE_RTREE=1 -lpthread -ldl -o q7
q8:
	gcc q8.c sqlite3.c dist.c -DSQLITE_ENABLE_RTREE=1 -lpthread -ldl -o q8

clean:
	rm -f *.o q4 q5 q6 q7 q8
