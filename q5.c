//Avery Tan
//Christina Ho
//assignment2 CMPUT391
#include <stdio.h>
#include <sqlite3.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>


void randCoord(int* arry, int newSeed) {
  srand(newSeed);
  for (int i=0; i<100; i++) {
    arry[i] = rand() % 1000+1;
  }

}

int print_result(void *a_param, int argc, char **argv, char **column) {
  for (int i=0; i< argc; i++)
        printf("%s \t", argv[i]);
    printf("\n");
    return 0;
}

int main(int argc, char **argv){


  sqlite3 *db; //the database
  char database_name[]= "as2.db";


  if( argc!=2 ){
    	fprintf(stderr, "Usage: %s <length> \n", argv[0]);
    	return(1);
  }

  int rc;

  rc = sqlite3_open(database_name, &db);
  if( rc ){
    fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
    sqlite3_close(db);
    return(1);
  }

  int l = atoi(argv[1]);

  // array of randomly generated coordinates
  int xarry[100];
  int yarry[100];

  randCoord(xarry,4);
  randCoord(yarry,128);

  double rtree_times[100]={0};
  double index_times[100]={0};

  // 100 bounding squares
  for (int i=0; i<100; i++) {

    // run each query rectangle 20 times for rtree and standard indexes
    for (int j=0; j<20; j++) {

      //rtree execution
      char *sql = sqlite3_mprintf( "SELECT id "\
                                  "FROM poi_rtree "\
                                  "WHERE minX>=%d "\
                                  "AND maxX<=%d "\
                                  "AND minY>=%d "\
                                  "AND maxY<=%d;",
                                  xarry[i], xarry[i]+l, yarry[i], yarry[i]+l);

      //time the rtree execution
      clock_t rtree_begin = clock();
      //rc = sqlite3_exec(db,sql,print_result,0,0); //print the result
      rc = sqlite3_exec(db,sql,0,0,0);
      clock_t rtree_end = clock();

      // add current execution time to total
      rtree_times[i] += (double)(rtree_end-rtree_begin);

      // printf("%f\n", times[i]);
      sqlite3_free(sql);


      // standard index execution
      char *sql_index = sqlite3_mprintf( "SELECT id "\
                                  "FROM poi "\
                                  "WHERE x1>=%d "\
                                  "AND x2<=%d "\
                                  "AND y1>=%d "\
                                  "AND y2<=%d;",
                                  xarry[i], xarry[i]+l, yarry[i], yarry[i]+l);

      clock_t index_begin = clock();
      //rc = sqlite3_exec(db,sql,print_result,0,0); //print the result
      rc = sqlite3_exec(db,sql_index,0,0,0);
      clock_t index_end = clock();

      // add current execution time to total
      index_times[i] += (double)(index_end-index_begin);
      printf("%d %d %d %lu %f\n",i, xarry[i], yarry[i],index_end-index_begin, index_times[i]);
      sqlite3_free(sql_index);

    }
    // average total execution times
    rtree_times[i] = rtree_times[i]/20;
    // average total execution times
    index_times[i] = index_times[i]/20;

    // printf("%d %lu %lu\n", i, rtree_times[i], index_times[i]);

  }
  double tot_rtree=0;
  double tot_index=0;
  // sum the imtes
  for (int i=0; i<100;i++){
      tot_rtree += rtree_times[i];
      tot_index += index_times[i];
  }

  // print param and averages
  printf("Parameter l: %s\n", argv[1]);
  printf("Average runtime with rtree: %f ms\n", (tot_rtree/100)*1000/(CLOCKS_PER_SEC) );
  printf("Average runtime without rtree: %f s\n", (tot_index/100)/(CLOCKS_PER_SEC) );

}
