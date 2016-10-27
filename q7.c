#include <stdio.h>
#include <sqlite3.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

double mindist(double p1, double p2, double s1, double s2, double t1, double t2){
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

double rmCalc(double s,double p, double t){
  if (p<((s+t)/2)){
    return s;
  }
  else {
    return t;
  }
}

double minmax(double p1, double p2, double s1, double s2, double t1, double t2){
  double number1 = pow((p1 - rmCalc(s1,p1,t1)),2) + pow(p2 - rmCalc(s2,p2,t2),2);
  double number2 = pow((p2 - rmCalc(s2,p2,t2)),2) + pow(p1 - rmCalc(s1,p1,t1),2);
  if (number1 < number2){
    return number1;
  }
  else {
    return number2;
  }

}

/* Callback to print result rows */
int print_result(void *a_param, int argc, char **argv, char **column) {
  for (int i=0; i< argc; i++)
        printf("%s \t", argv[i]);
    printf("\n");
    return 0;
}

/* Callback to store somewhere */
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
    printf("%s\n",array[i-1]);


  }
  printf("%s\n",ins);


  return 0;
}

// boolean prune(int child_id) {
//
// }

/*** main ***/

int main(int argc, char **argv){


  sqlite3 *db; //the database
  char database_name[]= "as2.db";

  int rc;

  rc = sqlite3_open(database_name, &db);
  if( rc ){
    fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
    sqlite3_close(db);
    return(1);
  }

  char *sql = sqlite3_mprintf("SELECT rtreenode(2, data)"\
                              "FROM poi_rtree_node "\
                              "WHERE nodeno=1;");
  char ins[9999];

  rc = sqlite3_exec(db,sql,hold_result,ins,0);
  // printf("%s\n",ins);
  char *sql_temp = sqlite3_mprintf("CREATE TABLE inners(id int, x1 float, x2 float, y1 float, y2 float);");
  char *sql_ins = sqlite3_mprintf("INSERT INTO inners(id, x1, x2, y1, y2) "\
                              "VALUES %s;",
                              ins);

  rc = sqlite3_exec(db,sql_temp,0,0,0);
  rc = sqlite3_exec(db,sql_ins,0,0,0);



  sqlite3_free(sql);
  sqlite3_free(sql_temp);
  sqlite3_free(sql_ins);

  sqlite3_close(db);
}
/*
WITH RECURSIVE nodes(parent, child) AS
  SELECT nodeno, parentnode
  FROM  poi_rtree_parent
  WHERE prune(nodenoe)=TRUE;
  UNION
  SELECT n.child, p.parentnode
  FROM nodes n, poi_rtree_parent p
  WHERE  n.parent=p.child;
  WEHERE prune(child)=TRUE;
  */
