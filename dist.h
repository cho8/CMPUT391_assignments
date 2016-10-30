#ifndef DIST_H
#define DIST_H

double mindist_c(double p1, double p2, double s1, double s2, double t1, double t2);
double minmax_c(double p1, double p2, double s1, double s2, double t1, double t2);
void mindist(sqlite3_context* ctx, int nargs, sqlite3_value** values );
void minmax(sqlite3_context* ctx, int nargs, sqlite3_value** values );

#endif
