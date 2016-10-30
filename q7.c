#include <stdio.h>
#include <sqlite3.h>
#include <stdlib.h>
#include <string.h>
#include "ll_nodes.h"
#include "dist.h"

float p1=0;
float p2=0;

int depth=0;                    //depth of tree
int NN=0;                       //nearest neighbor id
float NN_dist = 999999;       //nearest neighbor distance

// active branch list
struct node* ABL_head=NULL;


/* Exec Callback to print result rows */
int print_result_cb(void *a_param, int argc, char **argv, char **column) {
  for (int i=0; i< argc; i++)
        printf("%s \t", argv[i]);
    printf("\n");
    return 0;
}

/* step through result */
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

int depth_result(void *a_param, int argc, char **argv, char **column) {
  depth = atoi(argv[0]);
  return(0);
}

/* Callback to store inners somewhere */
int pushABL(void *a_param, int argc, char **argv, char **column) {
  // parse the data blob and insert into a temp table
  char *array[512];
  //active branch list head
  // strcpy(ins,"");
  int i=0;

  int id;
  float x1, x2, y1, y2;
  float mind, minm;

  array[i] = strtok(argv[0]," ");

  while(array[i]!=NULL)
  {
    if (array[i][0] == '{') {
      array[i]++;

     } else if (array[i][strlen(array[i])-1] == '}'){
      array[i][strlen(array[i])-1]='\0';

    }


    switch (i%5) {
      case 0 :
        id=atoi(array[i]);
        break;
      case 1 :
        x1=atof(array[i]);
        break;
      case 2 :
        x2=atof(array[i]);
        break;
      case 3 :
        y1=atof(array[i]);
        break;
      case 4 :
        y2=atof(array[i]);
        mind=mindist_c(p1,p2,x1,x2,y1,y2);
        minm=minmax_c(p1,p2,x1,x2,y1,y2);

        insert(&ABL_head, id, x1, x2, y1, y2, mind, minm);
        break;
    }

    array[++i] = strtok(NULL," ");
  //  printf("%s\n",array[i-1]);x

  }


  struct node* curr = ABL_head;
  struct node* temp = curr;
  float minmm = curr->minm;

  while(curr){
    if (curr->minm < minmm) {

      minmm = curr->minm;
      printf("%f\n", minmm);
    }
    temp = curr;
    curr = curr->next;
  }

  curr = ABL_head;
  temp=curr;

  while(curr){

    if (curr->mind >= minmm) {
      printf("Prune!\n");
      temp = curr;
      curr = curr->prev;
      delete(&ABL_head, temp->prev);

    }
    temp = curr;
    curr = curr->next;
  }



  ABL_head = mergeSort(ABL_head);

  // int maxmm = 0
  // struct node* curr = ABL_head;
  // struct node* temp = curr;
  // struct node* comp = curr;
  // // struct node* temp2 = comp;
  // while(curr){
  //   *comp = *ABL_head;
  //   printf("curr is %d\n", curr->id);
  //   // while(comp) {
  //   //   printf("comp is %d\n", comp->id);
  //   //   if (curr != comp && curr->mind >= comp->minm) {
  //   //     // curr = curr->next;
  //   //       // delete(&ABL_head, curr->prev);
  //   //     printf("prune!\n");
  //   //   }
  //   //   temp2 = comp;
  //   //   comp = comp->next;
  //   // }
  //   temp = curr;
  //   curr = curr->next;
  // }
  return 0;
}

// boolean prune(int child_id) {
//
// }

/*** main ***/

int main(int argc, char **argv){

  int depth_i;            // depth traversal counter

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

  // create mindist and minmax
  sqlite3_create_function(db, "mindist", 6, SQLITE_UTF8, NULL, mindist, NULL, NULL);
  sqlite3_create_function(db, "minmax", 6, SQLITE_UTF8, NULL, minmax, NULL, NULL);

  // find depth of rtree
  char *sql_depth = sqlite3_mprintf("SELECT rtreedepth(data) "\
                                    "FROM poi_rtree_node "\
                                    "WHERE nodeno=1;");

  rc = sqlite3_exec(db, sql_depth, depth_result,0,0);

  // find all first level non-root inners
  char *sql_rootnode = sqlite3_mprintf("SELECT rtreenode(2, data)"\
                              "FROM poi_rtree_node "\
                              "WHERE nodeno=%d;", 1);


  // while (1) {

    if (depth_i==depth+1) {
      // looking at leaves
      // find leaves

    }
    // get the children of the current node and prune into ABL
    rc = sqlite3_exec(db,sql_rootnode,pushABL,0,0);

    depth_i++;
  // }
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
