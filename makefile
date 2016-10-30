
q7:
	gcc q7_v2.c sqlite3.c dist.c -DSQLITE_ENABLE_RTREE=1 -lpthread -ldl -o q7

clean:
	rm *.o 