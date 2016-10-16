/*
Q4 (10 pts)

Write a C program, in a file called q4.c that takes as parameters the coordinates of a bounding rectangle (x1,y1) (x2, y2) and POI class c and prints the list of objects within that rectangle and of that class.
*/
#include <stdio.h>
#include <sqlite3.h>

int main(int argc, char **argv){
	sqlite3 *db; //the database
	sqlite3_stmt *stmt; //the update statement

  int rc;

  if( argc!=2 ){
    fprintf(stderr, "Usage: %s <database file> \n", argv[0]);
    return(1);
  }

  rc = sqlite3_open(argv[1], &db);
  if( rc ){
    fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
    sqlite3_close(db);
    return(1);
  }

  // Query to find all airports reachable from YEG w/ < 3 connections
  char *sql_stmt =
    ;

  rc = sqlite3_prepare_v2(db, sql_stmt, -1, &stmt, 0);
  
  if (rc != SQLITE_OK) {
      fprintf(stderr, "Preparation failed: %s\n", sqlite3_errmsg(db));
      sqlite3_close(db);
      return 1;
  }

  while((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
      int col;
      for(col=0; col<sqlite3_column_count(stmt)-1; col++) {
        printf("%s|", sqlite3_column_text(stmt, col));
      }
      printf("%s", sqlite3_column_text(stmt, col));
      printf("\n");
  }
  sqlite3_finalize(stmt); //always finalize a statement
}
