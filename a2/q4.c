// Avery Tan
// Christina Ho
// Assignment2 CMPUT391

/*
  Q4 (10 pts)

Write a C program, in a file called q4.c that takes as parameters the
coordinates of a bounding rectangle (x1,y1) (x2, y2) and POI class c and
prints the list of objects within that rectangle and of that class.
*/

#include <stdio.h>
#include <sqlite3.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char **argv){

  sqlite3 *db; //the database
  sqlite3_stmt *stmt; //the update statement
  //char database_name[25] = "munich.db";
  char database_name[]= "as2.db";
  //container for assembling our query
  char sql_statement[999];

  if( argc!=6 ){
    	fprintf(stderr, "Usage: %s <c> <x1> <x2> <y1> <y2>\n", argv[0]);
    	return(1);
  }

  char sql_statement1[] = "SELECT pt.id "\
                            "FROM poi_tag pt, poi_rtree pi "\
                                  "WHERE pt.id=pi.id "\
                                  "AND pt.key = \"class\" "\
                                  "AND pt.value = \"";

  char sql_statement2[] ="\" AND pi.minX>= ";
  char sql_statement3[] =" AND pi.maxX<= ";
  char sql_statement4[] =" AND pi.minY>= ";
  char sql_statement5[] =" AND pi.maxY<= ";

  //assembling our query
  strcpy(sql_statement, sql_statement1);
  strncat(sql_statement, argv[1], sizeof(argv[1]));
  strncat(sql_statement, sql_statement2, sizeof(sql_statement2)/sizeof(char));
  strncat(sql_statement, argv[2], sizeof(argv[2]));
  strncat(sql_statement, sql_statement3, sizeof(sql_statement3)/sizeof(char));
  strncat(sql_statement, argv[3], sizeof(argv[3]));
  strncat(sql_statement, sql_statement4, sizeof(sql_statement4)/sizeof(char));
  strncat(sql_statement, argv[4], sizeof(argv[4]));
  strncat(sql_statement, sql_statement5, sizeof(sql_statement5)/sizeof(char));
  strncat(sql_statement, argv[5], sizeof(argv[5]));

  // printf("%s\n",sql_statement);
  int rc;

  rc = sqlite3_open(database_name, &db);
  if( rc ){
    fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
    sqlite3_close(db);
    return(1);
  }

  rc = sqlite3_prepare_v2(db, sql_statement, -1, &stmt, 0);

  if (rc != SQLITE_OK) {
    fprintf(stderr, "Preparation failed: %s\n", sqlite3_errmsg(db));
    sqlite3_close(db);
    return 1;
  }
  //printing our list
  while((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
    int col;
    for(col=0; col<sqlite3_column_count(stmt)-1; col++) {
      printf("%s|", sqlite3_column_text(stmt, col));
    }
    printf("%s", sqlite3_column_text(stmt, col));
    printf("\n");
  }

  sqlite3_finalize(stmt); //always finalize a statement
  sqlite3_close(db);
}
