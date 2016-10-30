#include <stdio.h>
#include <sqlite3.h>
#include <stdlib.h>
#include <math.h>
#include "nodes.h"

double mindist_c(Point p, Node n){
  double r1, r2;
  double p1 = p.x;
  double p2 = p.y;
  double s1 = n.x1;
  double s2 = n.y1;
  double t1 = n.x2;
  double t2 = n.y2;

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
double minmax_c(Point p, Node n){
  double p1 = p.x;
  double p2 = p.y;
  double s1 = n.x1;
  double s2 = n.y1;
  double t1 = n.x2;
  double t2 = n.y2;

  double number1 = pow((p1 - rmCalc(s1,p1,t1)),2) + pow(p2 - rMCalc(s2,p2,t2),2);
  double number2 = pow((p2 - rmCalc(s2,p2,t2)),2) + pow(p1 - rMCalc(s1,p1,t1),2);
  if (number1 < number2){
    return number1;
  }
  else {
    return number2;
  }

}


// sqlite function to calculate mindist
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

// sqlite function to calculate minimax distance in c
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
