//Avery Tan
//CHRISTINA HO
//assignment1 CMPUT391
#include <stdio.h>
#include <sqlite3.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

//calculating from the centre of the object ((x1+x2)/2, (y1+y2)/2)
double mindist(double p1, double p2, double s1, double s2, double t1, double t2){
  double r1, r2
  if (p1 < s1){
    r1 = s1;
  }
  else if(p1>t1){
    r1 = t1;
  }
  else {
    r1 = p1;
  }
  if (p2 < s2){
    r2 = s2;
  }
  else if(p2>t2){
    r2 = t2;
  }
  else {
    r2 = p2;
  }
  return pow((p2-r2),2) + pow((p1 - r1),2); 
}

double rmCalc(double s,double p, double t){
  if (p<((s+t)/2)){
    return s; 
  }
  else {
    return t;
  }
}

double minmax(double p1, double p2, double s1, double s2, double t1, double t2){
  double number1 = (pow(p1 - rmCalc(s1,p1,t1)),2) + pow(p2 - rmCalc(s2,p2,t2),2);
  double number2 = (pow(p2 - rmCalc(s2,p2,t2)),2) + pow(p1 - rmCalc(s1,p1,t1),2);
  if (number1 < number2){
    return number1;
  }
  else {
    return number2;
  }

}


int main(int argc, char **argv){


  sqlite3 *db; //the database
  sqlite3_stmt *stmt; //the update statement
  //char database_name[25] = "munich.db";
  char database_name[]= "as2.db";
  //container for assembling our query
  char sql_statement[999];

  if( argc!=1 ){
    	fprintf(stderr, "Usage: %s <x1> <x2> <y1> <y2>\n", argv[0]);
    	return(1);
  }

  /*
  Q9 (15 pts)

  Write a C program, in a file called q9.c that takes an airport IATA
  code as a parameter and produces the equivalent answer of question 8
  above for that airport.
  */
  char sql_statement1[] = "SELECT rtreenode(2, data) FROM poi_rtree_node where nodeno = 1;";



  //assembling our query
  strcpy(sql_statement, sql_statement1);


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
