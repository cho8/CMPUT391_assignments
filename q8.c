#include <stdio.h>
#include <sqlite3.h>
#include <stdlib.h>
#include <string.h>
#include "nodes.h"
#include "dist.h"

int depth;      // depth of tree
int near_count, k;			// counter, k nearest neighbors
float max_nearest=0;
/*
  Callback function to store depth
  */
int depth_result(void *a_param, int argc, char **argv, char **column) {
  depth = atoi(argv[0]);
  return(0);
}

/*
  Parse blob and fill the ABL with children of non-leaf node
*/
void populateBranchList(char* nodeBlob, Node* ABL) {
  // parse the data blob and insert into a temp table
  char *array[512];
  //active branch list

  // strcpy(ins,"");
  int i=0;
  int ABL_i=0;

  unsigned long id;
  float x1, x2, y1, y2;
  float mind, minm;

  array[i] = strtok(nodeBlob," ");

  while(array[i]!=NULL) {
    if (array[i][0] == '{') {
      array[i]++;

    } else if (array[i][strlen(array[i])-1] == '}'){
      array[i][strlen(array[i])-1]='\0';
    }
    char *ptr;
    switch (i%5) {
      case 0 :
        id=strtol(array[i], &ptr,10);
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
        ABL[ABL_i].id=id;
        ABL[ABL_i].x1=x1;
        ABL[ABL_i].x2=x2;
        ABL[ABL_i].y1=y1;
        ABL[ABL_i].y2=y2;
        ABL_i++;

        break;
    }

    array[++i] = strtok(NULL," ");
    // printf("%s\n",array[i-1]);
  }
}

/*
  From sqlite obtain the number of non-leaf children nodes and
  the blob containing node MBRs.
*/

int genBranchList(sqlite3 *db, sqlite3_stmt *stmt, Node node, char* nodeBlob) {
  int nChildren=0;
  int rc;
  // get the MBRs contained in the node
  char *sql = sqlite3_mprintf("SELECT rtreenode(2, data) as data, COUNT(p.parentnode) "\
                              "FROM poi_rtree_node n, poi_rtree_parent p "\
                              "WHERE n.nodeno = p.parentnode AND n.nodeno=%lu;", node.id);
  rc = sqlite3_prepare_v2(db, sql, -1, &stmt, 0);
  if (rc != SQLITE_OK) {
    fprintf(stderr, "Preparation failed sql_children: %s\n", sqlite3_errmsg(db));
    sqlite3_close(db);
  }

  while((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
    sprintf(nodeBlob, "%s", sqlite3_column_text(stmt, 0));
    nChildren = sqlite3_column_int(stmt, 1);
  }
  sqlite3_free(sql);
  sqlite3_finalize(stmt);
  return nChildren;
}

/*
  From sqlite obtain the number of leaf children and
  the blob containing the leaf MBRs.
*/
int genChildrenList(sqlite3 *db, sqlite3_stmt *stmt, Node node, char* nodeBlob) {
  int nChildren=0;
  int rc;
  // get the MBRs contained in the node
  char *sql_leaf = sqlite3_mprintf("SELECT rtreenode(2, data) as data, COUNT(r.nodeno) "\
                              "FROM poi_rtree_node n, poi_rtree_rowid r "\
                              "WHERE n.nodeno = r.nodeno AND n.nodeno=%lu;", node.id);

  rc = sqlite3_prepare_v2(db, sql_leaf, -1, &stmt, 0);

  if (rc != SQLITE_OK) {
    fprintf(stderr, "Preparation failed sql_children: %s\n", sqlite3_errmsg(db));
    sqlite3_close(db);
  }

  while((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
    sprintf(nodeBlob, "%s", sqlite3_column_text(stmt, 0));
    nChildren = sqlite3_column_int(stmt, 1);
  }
  sqlite3_free(sql_leaf);
  sqlite3_finalize(stmt);
  return nChildren;
}

void swap(Node a[], int num1, int num2) {
   Node temp = a[num1];
   a[num1] = a[num2];
   a[num2] = temp;
}

int partition(Node a[], int left, int right, float pivot, Point point) {
  int leftPointer = left -1;
  int rightPointer = right;

  while(1) {
    while(mindist_c(point,a[++leftPointer]) < pivot) {
         //do nothing
    }

    while(rightPointer > 0 && mindist_c(point,a[--rightPointer]) > pivot) {
         //do nothing
    }

    if(leftPointer >= rightPointer) {
       break;
    } else {
        //  printf(" item swapped :%d,%d\n", a[leftPointer],a[rightPointer]);
      swap(a, leftPointer,rightPointer);
    }
  }

  //  printf(" pivot swapped :%d,%d\n", intArray[leftPointer],intArray[right]);
  swap(a,leftPointer,right);
  //  printf("Updated Array: ");
  //  display();
  return leftPointer;
}

void quickSort(Node a[], int left, int right, Point point) {
  if(right-left <= 0) {
    return;
  } else {
    float pivot = mindist_c(point,a[right]);
    int partitionPoint = partition(a, left, right, pivot, point);
    quickSort(a, left, partitionPoint-1, point);
    quickSort(a, partitionPoint+1, right, point);
  }
}

int downwardPruneBranchList(Node node, Point point, Node* ABL, int nChildren) {
  Node t;     // temp node pointer
  float min=minmax_c(point,ABL[0]);       // minimum minmax
  float mm;

  // Iterate through all combinations applying pruning strategy 1
  // find the minimum minmax
  for (int i=0; i<nChildren; i++) {
    mm=minmax_c(point,ABL[i]);
    if (mm < min) {
      printf("mm %f min %f", mm, min);
      min=mm;
    }
  }
  // prune out all that are smaller than minmax
  for (int j=0; j<nChildren; j++) {
      if (mindist_c(point, ABL[j]) >= min) {
      // i is pruned, swap it to the end and "shrink" the array max
      t = ABL[j];
      ABL[j]=ABL[nChildren-1];
      ABL[nChildren-1]=t;
      nChildren--;
      j--; //rewind and check this index again
    }
  }
  quickSort(ABL,0,nChildren-1,point);

  // Pruning strategy 2
  for (int i=0; i<nChildren;i++) {
    if(mindist_c(point,ABL[i]) >= minmax_c(point,node)) {
      t = ABL[i];
      ABL[i]=ABL[nChildren-1];
      ABL[nChildren-1]=t;
      nChildren--;
      i--; //rewind and check this index again
    }
  }
  quickSort(ABL,0,nChildren-1,point);
  return nChildren;

}

int upwardPruneBranchList(Node node, Point point, Node* ABL, int nChildren) {
  Node t;     // temp node pointer

  // Iterate through all combinations applying pruning strategy 3
  for (int i=0; i<nChildren; i++) {
    if (mindist_c(point, node) >= mindist_c(point, ABL[i])) {
      // i is pruned, swap it to the end and "shrink" the array max
      t = ABL[i];
      ABL[i]=ABL[nChildren-1];
      ABL[nChildren-1]=t;
      nChildren--;
      i--;
    }
  }
  quickSort(ABL,0,nChildren-1,point);
  return nChildren;
}


/*
  Print out the ABL (for debugging purposes)
*/
void printABL(Node a[], int nChildren) {
  for (int i=0; i<nChildren; i++) {
    printf("%lu ", a[i].id);
  } printf("\n");
}

void nearestNeighborSearch(sqlite3 *db, sqlite3_stmt *stmt, Node currNode, Point point, Nearest* nearest, int depth_count) {
  Node newNode;   // newNode
  Node *ABL;    // branchList
  char nodeBlob[9999];
  int nChildren;
  float dist;
  //  At leaf level - compute distance to actual objects
  //  If Node.type = LEAF
  if (depth_count==depth) {

    nChildren = genChildrenList(db, stmt, currNode, nodeBlob);
    ABL = malloc(nChildren * sizeof(Node));
    populateBranchList((char*)nodeBlob,ABL);

    // For i := 1 to Node.count
    for (int i=0; i<nChildren; i++) {
      dist = mindist_c(point, ABL[i]);

				if (near_count<k) {
					nearest[near_count].dist=dist;
					nearest[near_count].rect=ABL[i];
					near_count++;
					if (dist>max_nearest) { max_nearest=dist;};
				} else {
					for (int i=0; i<k; i++) {
						//kNN array filled, check each and replace with next nearest
						if (dist < max_nearest && nearest[i].dist) {
							printf("%d Nearest %f Calc %f\n", i, nearest[i].dist, dist);
		    			nearest[i].dist = dist;
		    			nearest[i].rect = ABL[i];
							break;
		  			}
					}

			}
    }
  }
  //  Non-leaf level - order, prune and visit nodes
  else {
    //  Generate Active Branch list
    nChildren = genBranchList(db, stmt,  currNode, nodeBlob);

    ABL = malloc(nChildren * sizeof(Node));
    populateBranchList((char*)nodeBlob,ABL);

    // sortBranchList(branchList)
    quickSort(ABL,0, nChildren-1, point);
    // printABL(ABL,nChildren);

    //  Perform Downward Pruning
    //  (may discard all branches)
    nChildren = downwardPruneBranchList(currNode, point, ABL, nChildren-1);
    quickSort(ABL,0,nChildren-1,point);


    //  Iterate through the Active Branch List
    for(int i=0; i<nChildren; i++) {
      //  newNode := Node.branch_branchlist
      newNode = ABL[i];
      //  Recursively visit child nodes
      nearestNeighborSearch(db, stmt, newNode, point, nearest, depth_count+1);

      //  Perform upward Pruning
      nChildren = upwardPruneBranchList(currNode, point, ABL, nChildren);
      quickSort(ABL,0,nChildren-1,point);

      // printABL(ABL,nChildren);
    }
  }
}


int main(int argc, char **argv){

  if (argc != 4) {
    printf("USAGE: %s <x> <y> <k>\n", argv[0]);
    return(0);
  }

  sqlite3 *db; //the database
  sqlite3_stmt    *stmt; //the query
  char database_name[]= "as2.db";
  char blob[999];

  int rc;

  rc = sqlite3_open(database_name, &db);
  if( rc ){
    fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
    sqlite3_close(db);
    return(1);
  }

  // get the depth of the whole rtree
  char *sql_depth = sqlite3_mprintf("SELECT rtreedepth(data) "\
                                    "FROM poi_rtree_node "\
                                    "WHERE nodeno=1;");

  rc = sqlite3_exec(db, sql_depth, depth_result,0,0);
  sqlite3_free(sql_depth);
  // set the query point
  Point point= (Point){ .x = atof(argv[1]), .y = atof(argv[2])};

  // start at root node, arbitrary node dimensions
  Node nodeN = (Node){ .id=1 , .x1=1000, .x2=1000, .y1=1000, .y2=1000}; // Current Node

  // Initialize Nearest neighbors as things really far away
	k = atoi(argv[3]);
  Nearest nearest[k];  // Nearest Neighbor
	for (int i=0; i<k; i++) {
		nearest[i].dist = 99999999999;
	}
	near_count=0;

  // Initialize depth counter
  int depth_count = 0;

  nearestNeighborSearch(db, stmt, nodeN, point, nearest, depth_count);
	printf("%d Nearest Neighbors: \n",k);
	for (int i=0; i<k; i++) {
  	printf("[%d] id: %lu | dist: %f | x1: %f | y1: %f | x2: %f | y2: %f\n", i+1, nearest[i].rect.id, nearest[i].dist, nearest[i].rect.x1, nearest[i].rect.y1, nearest[i].rect.x2, nearest[i].rect.y2);
  }
	sqlite3_close(db);
}
