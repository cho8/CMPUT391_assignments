#include <stdio.h>
#include <sqlite3.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

// c function
double mindist_c(double p1, double p2, double s1, double s2, double t1, double t2){
  double r1, r2;
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



//sqlite function
void mindist(sqlite3_context* ctx, int nargs, sqlite3_value** values ){
  //values : double p1, double p2, double s1, double s2, double t1, double t2){
  double r1, r2;
  double p1=sqlite3_value_double(values[0]);
  double p2=sqlite3_value_double(values[1]);
  double s1=sqlite3_value_double(values[2]);
  double s2=sqlite3_value_double(values[3]);
  double t1=sqlite3_value_double(values[4]);
  double t2=sqlite3_value_double(values[5]);

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
  double y = pow((p2-r2),2) + pow((p1 - r1),2);
  sqlite3_result_double(ctx, y);
}

double rmCalc(double s,double p, double t){
  if (p<=((s+t)/2)){
    return s;
  }
  else {
    return t;
  }
}

double rMCalc(double s,double p, double t){
  if (p>=((s+t)/2)){
    return s;
  }
  else {
    return t;
  }
}

// calculate minimax distance in c
double minmax_c(double p1, double p2, double s1, double s2, double t1, double t2){
  double number1 = pow((p1 - rmCalc(s1,p1,t1)),2) + pow(p2 - rMCalc(s2,p2,t2),2);
  double number2 = pow((p2 - rmCalc(s2,p2,t2)),2) + pow(p1 - rMCalc(s1,p1,t1),2);
  if (number1 < number2){
    return number1;
  }
  else {
    return number2;
  }

}

// sqlite function
void minmax(sqlite3_context* ctx, int nargs, sqlite3_value** values ){
  double p1=sqlite3_value_double(values[0]);
  double p2=sqlite3_value_double(values[1]);
  double s1=sqlite3_value_double(values[2]);
  double s2=sqlite3_value_double(values[3]);
  double t1=sqlite3_value_double(values[4]);
  double t2=sqlite3_value_double(values[5]);

  double number1 = pow((p1 - rmCalc(s1,p1,t1)),2) + pow(p2 - rMCalc(s2,p2,t2),2);
  double number2 = pow((p2 - rmCalc(s2,p2,t2)),2) + pow(p1 - rMCalc(s1,p1,t1),2);
  if (number1 < number2){
    sqlite3_result_double(ctx, number1);
  }
  else {
    sqlite3_result_double(ctx, number2);
  }

}

/* Callback to print result rows */
int print_result_cb(void *a_param, int argc, char **argv, char **column) {
  for (int i=0; i< argc; i++)
        printf("%s \t", argv[i]);
    printf("\n");
    return 0;
}

void step_result(sqlite3_stmt *stmt){
	int rc;

	while((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
		int col;
		for(col=0; col<sqlite3_column_count(stmt)-1; col++) {
			printf("%s|", sqlite3_column_text(stmt, col));
		}
		printf("%s", sqlite3_column_text(stmt, col));
		printf("\n");
	}


}

/* Callback to store inners somewhere */
int hold_result(void *a_param, int argc, char **argv, char **column) {
  // parse the data blob and insert into a temp table
  char *array[512];
  char *ins = (char*)a_param;
  strcpy(ins,"");
  int i=0;

  array[i] = strtok(argv[0]," ");

  while(array[i]!=NULL)
  {
    if (array[i][0] == '{') {
       array[i][0]='(';
       if (i!=0) {
         strcat(ins,",");
       }
       strcat(ins,array[i]);


     } else if (array[i][strlen(array[i])-1] == '}'){
      array[i][strlen(array[i])-1]=')';
      strcat(ins,",");
      strcat(ins,array[i]);
    } else {
      strcat(ins,",");
      strcat(ins,array[i]);
    }

    array[++i] = strtok(NULL," ");
    // printf("%s\n",array[i-1]);


  }
  // printf("%s\n",ins);


  return 0;
}

// boolean prune(int child_id) {
//
// }

/*** main ***/

int main(int argc, char **argv){

  if (argc != 3) {
    printf("USAGE: %s <x> <y>\n", argv[0]);
    return(0);
  }

  sqlite3 *db; //the database
  sqlite3_stmt    *stmt; //the query
  char database_name[]= "as2.db";

  int rc;

  rc = sqlite3_open(database_name, &db);
  if( rc ){
    fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
    sqlite3_close(db);
    return(1);
  }

  sqlite3_create_function(db, "mindist", 6, SQLITE_UTF8, NULL, mindist, NULL, NULL);
  sqlite3_create_function(db, "minmax", 6, SQLITE_UTF8, NULL, minmax, NULL, NULL);

  // find all first level non-root inners
  char *sql = sqlite3_mprintf("SELECT rtreenode(2, data)"\
                              "FROM poi_rtree_node "\
                              "WHERE nodeno=1;");
  char ins[9999];

  // get the blob of data and parse to string of values
  rc = sqlite3_exec(db,sql,hold_result,ins,0);

  // store the string of values as a table of mbr coordinates
  char *sql_temp = sqlite3_mprintf("CREATE TABLE inners(id int, x1 float, x2 float, y1 float, y2 float, mind float, minm float);");
  char *sql_ins = sqlite3_mprintf("INSERT INTO inners(id, x1, x2, y1, y2) "\
                              "VALUES %s;",
                              ins);

  rc = sqlite3_exec(db,sql_temp,0,0,0);
  rc = sqlite3_exec(db,sql_ins,0,0,0);

  // loop
  // while (1) {
    char *sql_dist = sqlite3_mprintf("UPDATE inners "\
                                      "SET mind = mindist(%s,%s,x1,x2,y1,y2), minm = minmax(%s,%s,x1,x2,y1,y2); ",
                                      argv[1],argv[2], argv[1], argv[2]);

  char *sql_prune1 = sqlite3_mprintf("SELECT id, mind,  minm "\
                                      "FROM inners "\
                                      "WHERE mind < minm;");

    // rc = sqlite3_exec(db,sql_dist,print_result_cb,0,0);
  rc = sqlite3_prepare_v2(db, sql_dist, -1, &stmt, 0);

	if (rc != SQLITE_OK) {
	  fprintf(stderr, "Preparation failed: %s\n", sqlite3_errmsg(db));
	  sqlite3_close(db);
	  return 1;
  }

  step_result(stmt);

  rc = sqlite3_prepare_v2(db, sql_prune1, -1, &stmt, 0);

  if (rc != SQLITE_OK) {
	  fprintf(stderr, "Preparation failed: %s\n", sqlite3_errmsg(db));
	  sqlite3_close(db);
	  return 1;
  }
  step_result(stmt);


  // }



  sqlite3_free(sql);
  sqlite3_free(sql_temp);
  sqlite3_free(sql_ins);

  sqlite3_close(db);
}


/*
  ======== recursively find path that is pruned
  WITH RECURSIVE nodes(child, parent) AS (
    SELECT nodeno, parentnode
    FROM  poi_rtree_parent
    WHERE prune(nodeno)=TRUE;
    UNION
    SELECT n.child, p.parentnode
    FROM nodes n, poi_rtree_parent p
    WHERE  n.parent=p.nodeno
    AND prune(child)=TRUE
  )
  SELECT * FROM nodes where parent=1;

  ========= find all nodes reachable from 1

  WITH RECURSIVE nodes(child, parent, depth) AS
  (
    SELECT p.nodeno, p.parentnode, 0 as depth
    FROM  inners i, poi_rtree_parent p
    where i.id = p.parentnode
    UNION
    SELECT n.child, p.parentnode, depth+1
    FROM nodes n, poi_rtree_parent p
    WHERE  n.parent=p.nodeno
  )
  SELECT * FROM nodes where parent = 1;


  ============ print tree :)

  WITH RECURSIVE nodes(child,depth) AS (
    VALUES(1,0)
    UNION ALL
    SELECT p.nodeno, depth+1
    FROM nodes n, poi_rtree_parent p
    WHERE  n.child=p.parentnode
    ORDER BY 2 DESC
  )
  SELECT substr('..........',1,depth*3) || child FROM nodes;

  */
