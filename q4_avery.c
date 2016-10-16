//Avery Tan
//assignment1 CMPUT391
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

  if( argc!=5 ){
    	fprintf(stderr, "Usage: %s <x1> <x2> <y1> <y2>\n", argv[0]);
    	return(1);
  }

  /*
  Q9 (15 pts)

  Write a C program, in a file called q9.c that takes an airport IATA
  code as a parameter and produces the equivalent answer of question 8
  above for that airport.
  */
  char sql_statement1[] = "SELECT pt.id "\
                            "FROM poi_tag pt, poi_rtree pi "\
                                  "WHERE pt.id=pi.id "\
                                    "AND pi.minX>= ";
                                //  "AND pt.key = \"class\" "\
                                //  "AND pt.value LIKE \"c%%\" "\


  char sql_statement2[] =" AND pi.maxX<= ";

  char sql_statement3[] =" AND pi.minY>= ";

  char sql_statement4[] =" AND pi.maxY<= ";


  //assembling our query
  strcpy(sql_statement, sql_statement1);
  strncat(sql_statement, argv[1], sizeof(argv[1]));
  strncat(sql_statement, sql_statement2, sizeof(sql_statement2)/sizeof(char));
  strncat(sql_statement, argv[2], sizeof(argv[2]));
  strncat(sql_statement, sql_statement3, sizeof(sql_statement3)/sizeof(char));
  strncat(sql_statement, argv[3], sizeof(argv[3]));
  strncat(sql_statement, sql_statement4, sizeof(sql_statement4)/sizeof(char));
  strncat(sql_statement, argv[4], sizeof(argv[4]));

  printf("%s",sql_statement);
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
}
